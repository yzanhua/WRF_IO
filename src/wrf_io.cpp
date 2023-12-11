#include "wrf_io.hpp"

#include <math.h>
#include <mpi.h>
#include <unistd.h>

#include <iostream>
#include <vector>

#include "wrf_io_drivers.hpp"
#include "wrf_io_logging.hpp"
#include "wrf_io_utils.hpp"

using std::vector;
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
void split_communicator (wrf_io_config &cfg);

vector<string> files_to_remove;

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
    file.reset (ts, cfg);

    io_driver_ptr->create (file, cfg);
    files_to_remove.push_back (file.file_name);

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
    io_driver_ptr->begin_step (file);
    for (auto &var : variable::variables) { io_driver_ptr->put_var (file, var, cfg); }

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
    double start_time, total_time, max_total_time;

    io_driver_ptr = get_io_driver (cfg);

    wrf_io_file hist_file (wrf_io_utils::history);
    wrf_io_file rst_file (wrf_io_utils::restart);

    start_time = MPI_Wtime ();

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
        }
    }

    if (hist_file.status == wrf_io_utils::valid) {
        ts--;
        err = wrf_io_close (cfg, hist_file, ts);
        check_err (err, __LINE__, __FILE__, "wrf_io_close failed");
    }

    total_time = MPI_Wtime () - start_time;
    err        = MPI_Reduce (&total_time, &max_total_time, 1, MPI_DOUBLE, MPI_MAX, 0, cfg.io_comm);
    check_err (err, __LINE__, __FILE__, "MPI_Reduce failed");
    if (cfg.rank == 0) {
        string out = "Group " + std::to_string (cfg.group_num);
        out += ", Iter: " + std::to_string (cfg.iter);
        out += ", Total time: " + std::to_string (max_total_time);
        out += ", e_we: " + std::to_string (cfg.e_we);
        out += ", e_sn: " + std::to_string (cfg.e_sn);
        out += ", ts_total: " + std::to_string (cfg.ts_total);
        out += ", np: " + std::to_string (cfg.np);
        std::cout << out << std::endl;
    }
    for (auto &file_name : files_to_remove) { remove (file_name.c_str ()); }
    files_to_remove.clear ();
    LEAVE;
    return err;
}  // wrf_io_core

int main (int argc, char **argv) {
    wrf_io_config cfg;
    int err, rank;

    err = MPI_Init (&argc, &argv);
    check_err (err, __LINE__, __FILE__, "MPI_Init failed");

    err = MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    check_err (err, __LINE__, __FILE__, "MPI_Comm_rank failed");

    log::rank     = rank;
    cfg.group_num = 0;
    cfg.io_comm   = MPI_COMM_WORLD;
    cfg.iter      = 0;

    if (argc < 2) { check_err (-1, __LINE__, __FILE__, "No config file provided"); }

    wrf_io_utils::get_config (argv[1], cfg);
    cfg.print (true);
    log::log_level = cfg.debug_level;

    split_communicator (cfg);

    variable::calculate_parition (cfg);
    wrf_io_utils::get_variables_attrs (cfg);

    unsigned org_e_we = cfg.e_we;
    unsigned org_e_sn = cfg.e_sn;
    for (int iter = 0; iter < 3; iter++) {
        cfg.iter = iter;

        for (unsigned e_we = 50; e_we < org_e_we; e_we += 50) {
            for (unsigned e_sn = 50; e_sn < org_e_sn; e_sn += 50) {
                cfg.e_we = e_we;
                cfg.e_sn = e_sn;
                wrf_io_core (cfg);
            }
        }
        cfg.e_we = org_e_we;
        cfg.e_sn = org_e_sn;
        wrf_io_core (cfg);
    }

err_out:;
    if (cfg.info != MPI_INFO_NULL) MPI_Info_free (&(cfg.info));
    if (cfg.io_comm != MPI_COMM_WORLD && cfg.io_comm != MPI_COMM_NULL)
        MPI_Comm_free (&(cfg.io_comm));
    MPI_Finalize ();
    return (err < 0) ? 1 : 0;
}

void split_communicator (wrf_io_config &cfg) {
    int err;
    int world_size, world_rank;

    // get world size and rank
    err = MPI_Comm_size (MPI_COMM_WORLD, &world_size);
    check_err (err, __LINE__, __FILE__, "MPI_Comm_size failed");
    err = MPI_Comm_rank (MPI_COMM_WORLD, &world_rank);
    check_err (err, __LINE__, __FILE__, "MPI_Comm_rank failed");

    // get num of processes per node
    int processes_per_node = -1;
    char *ppn_str          = getenv ("SLURM_NTASKS_PER_NODE");
    if (ppn_str == NULL) {
        processes_per_node = 128;
    } else {
        processes_per_node = atoi (ppn_str);
    }
    if (processes_per_node <= 0) {
        fprintf (stderr, "Invalid PPN value. Exiting.\n");
        MPI_Abort (MPI_COMM_WORLD, 1);
    }

    // get total num of nodes
    int nodes = world_size / processes_per_node;

    // Calculate the number of groups (n)
    int n = (int)(log (nodes + 1) / log (2));

    // Determine the group for this process
    int group_number = 0;
    int node_number  = world_rank / processes_per_node;
    int count        = 0;
    for (int i = 0; i < n; i++) {
        count += (int)pow (2, i);
        if (node_number < count) {
            group_number = i;
            break;
        }
    }

    err = MPI_Comm_split (MPI_COMM_WORLD, group_number, world_rank, &cfg.io_comm);
    check_err (err, __LINE__, __FILE__, "MPI_Comm_split failed");
    err = MPI_Comm_rank (cfg.io_comm, &cfg.rank);
    check_err (err, __LINE__, __FILE__, "MPI_Comm_rank failed");
    err = MPI_Comm_size (cfg.io_comm, &cfg.np);
    check_err (err, __LINE__, __FILE__, "MPI_Comm_size failed");

    wrf_io_utils::check_err_set_comm (cfg.io_comm);
    cfg.group_num = group_number;

    return;
}
