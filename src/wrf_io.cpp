#include "wrf_io.hpp"

#include <mpi.h>
#include <unistd.h>

#include <iostream>

#include "wrf_io_drivers.hpp"
#include "wrf_io_logging.hpp"
#include "wrf_io_utils.hpp"

using wrf_io_driver::get_io_driver;
using wrf_io_driver::io_driver;
using wrf_io_logging::log;
using wrf_io_utils::attribute;
using wrf_io_utils::check_err;
using wrf_io_utils::dimension;
using wrf_io_utils::variable;
using wrf_io_utils::wrf_io_config;
using wrf_io_utils::wrf_io_file;

static io_driver *io_driver_ptr;

int wrf_io_core (wrf_io_config &cfg);
int wrf_io_close (wrf_io_config &cfg, wrf_io_file &file, unsigned ts);
int wrf_io_write (wrf_io_config &cfg, wrf_io_file &file, unsigned ts);

int wrf_io_close (wrf_io_config &cfg, wrf_io_file &file, unsigned ts) {
    ENTER;
    io_driver_ptr->close (file);
    file.clear ();
    LEAVE;
    return 0;
}  // wrf_io_close

int wrf_io_create_and_define (wrf_io_config &cfg, wrf_io_file &file, unsigned ts) {
    ENTER;
    int err = 0;
    // create files
    file.reset (ts, cfg.out_prefix);

    io_driver_ptr->create (file, cfg);

    // define dimensions

    // iterate over the unordered_map dimension::dim_name_to_dim
    // if (cfg.rank == 0) { printf ("%u: \tdefine dims:\n", ts); }
    for (auto &dim_pair : dimension::dim_name_to_dim) {
        dimension *dim_ptr = &dim_pair.second;
        int dim_value      = dim_ptr->value;
        if (dim_value == -1) {  // E_WE
            dim_value = cfg.e_we;
        } else if (dim_value == -2) {  // E_SN
            dim_value = cfg.e_sn;
        } else if (dim_value == -3) {  // UNLIMITED
            dim_value = -1;
        }
        io_driver_ptr->def_dim (file, dim_pair.second, dim_value);
    }

    // if (cfg.rank == 0) { printf ("%u: \tdefine global attrs:\n", ts); }
    // put global attributes
    variable global;
    global.varid = -1;
    for (auto &attr : attribute::global_attrs) { io_driver_ptr->put_att (file, global, attr); }
    // define variables and write variable attributes
    for (auto &var : variable::variables) {
        io_driver_ptr->def_var (file, var);
        for (auto &attr : var.attrs) { io_driver_ptr->put_att (file, var, attr); }
    }

    // end define mode
    // if (cfg.rank == 0) { printf ("%u: \tend define:\n", ts); }
    io_driver_ptr->enddef (file);

    file.status = wrf_io_utils::valid;

    LEAVE;

    return err;
}  // wrf_io_create_and_define

int wrf_io_write (wrf_io_config &cfg, wrf_io_file &file, unsigned ts) {
    ENTER;
    int err = 0;
    if (file.status == wrf_io_utils::invalid) { err = wrf_io_create_and_define (cfg, file, ts); }

    // write variables
    // begin step
    // if (cfg.rank == 0) {
    //     printf ("%u: \twrf_io_write: begin_step timestep=%d\n", ts, file.curr_ts);
    // }
    io_driver_ptr->begin_step (file);
    for (auto &var : variable::variables) {
        // if (cfg.rank == 0) {
        //     printf ("%u: \t\twrf_io_write: put_var timestep=%d var=%s\n", ts, file.curr_ts,
        //             var.name.c_str ());
        // }
        io_driver_ptr->put_var (file, var, cfg);
    }

    // if (cfg.rank == 0) { printf ("%u: \twrf_io_write: end_step timestep=%d\n", ts, file.curr_ts); }
    io_driver_ptr->end_step (file);

    file.curr_ts++;

    if (file.type == wrf_io_utils::restart || file.curr_ts % cfg.ts_per_hist_file == 0) {
        err = wrf_io_close (cfg, file, ts);
    }
    LEAVE;

    return err;
}  // wrf_io_write
int wrf_io_core (wrf_io_config &cfg) {
    ENTER;
    int err     = 0;
    unsigned ts = 0;  // current time step; must init to 0

    io_driver_ptr = get_io_driver (cfg);

    wrf_io_file hist_file (wrf_io_utils::history);
    wrf_io_file rst_file (wrf_io_utils::restart);

    if (cfg.rank == 0) hist_file.print ();
    if (cfg.rank == 0) rst_file.print ();

    // if (cfg.rank == 0) printf ("\n=========== time step %u\n", ts);
    // write history file 0; this time step does not need computation
    err = wrf_io_write (cfg, hist_file, ts);
    check_err (err, __LINE__, __FILE__, "wrf_io_write failed");

    for (ts = 1; ts <= cfg.ts_total; ts++) {
        // if (cfg.rank == 0) printf ("\n=========== time step %u\n", ts);
        // computation time
        if (cfg.sleep_sec > 0) { sleep (cfg.sleep_sec); }

        if (ts % cfg.ts_hist_interval == 0) {
            err = wrf_io_write (cfg, hist_file, ts);
            check_err (err, __LINE__, __FILE__, "wrf_io_write failed");
        }

        if (ts % cfg.ts_rst_interval == 0) {
            err = wrf_io_write (cfg, rst_file, ts);
            check_err (err, __LINE__, __FILE__, "wrf_io_write failed");
            err = wrf_io_close (cfg, hist_file, ts);
        }
    }

    if (hist_file.status == wrf_io_utils::valid) {
        ts--;
        err = wrf_io_close (cfg, hist_file, ts);
        check_err (err, __LINE__, __FILE__, "wrf_io_close failed");
    }
    LEAVE;
    return err;
}  // wrf_io_core

int main (int argc, char **argv) {
    wrf_io_config cfg;
    int err, rank;
    double start_time, total_time, max_total_time;

    err = MPI_Init (&argc, &argv);
    check_err (err, __LINE__, __FILE__, "MPI_Init failed");

    err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    check_err (err, __LINE__, __FILE__, "MPI_Comm_rank failed");

    log::rank      = rank;

    if (argc < 2) { check_err (-1, __LINE__, __FILE__, "No config file provided"); }
    wrf_io_utils::get_config (argv[1], cfg);
    cfg.print (true);
    log::log_level = cfg.debug_level;

    variable::calculate_parition (cfg);
    wrf_io_utils::get_variables_attrs (cfg);

    start_time = MPI_Wtime ();
    wrf_io_core (cfg);
    total_time = MPI_Wtime () - start_time;

    err = MPI_Reduce (&total_time, &max_total_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    check_err (err, __LINE__, __FILE__, "MPI_Reduce failed");
    if (cfg.rank == 0) { std::cout << "Total time: " << max_total_time << std::endl; }

err_out:;
    if (cfg.info != MPI_INFO_NULL) MPI_Info_free (&(cfg.info));
    if (cfg.io_comm != MPI_COMM_WORLD && cfg.io_comm != MPI_COMM_NULL)
        MPI_Comm_free (&(cfg.io_comm));
    MPI_Finalize ();
    return (err < 0) ? 1 : 0;
}