#include <unistd.h>

#include "wrf_io.h"
#include "wrf_io_err.h"
#include "wrf_io_core.h"

int wrf_io_create_and_define (wrf_io_config &cfg, file_stat &stat, unsigned ts) {
    int err = 0;
    // create files
    stat.reset (ts, cfg.out_prefix);
    if (cfg.rank == 0) {
        printf ("%s: create file at time step %u.\n", stat.file_name.c_str (), ts);
    }

    // define variables
    if (cfg.rank == 0) {
        printf ("%s: create and define variables at time step %u.\n", stat.file_name.c_str (), ts);
    }
    stat.status = valid;

err_out:;
    return err;
}

int wrf_io_close (wrf_io_config &cfg, file_stat &stat, unsigned ts) {
    int err = 0;
    if (cfg.rank == 0) {
        printf ("%s: Close file at time step %u. Currently contains %u time step \n",
                stat.file_name.c_str (), ts, stat.curr_ts);
    }
    stat.clear ();

err_out:;
    return err;
}
int wrf_io_write (wrf_io_config &cfg, file_stat &stat, unsigned ts) {
    int err = 0;
    if (stat.status == invalid) { err = wrf_io_create_and_define (cfg, stat, ts); }

    // write variables
    if (cfg.rank == 0) {
        printf ("%s: write variables at time step %u.\n", stat.file_name.c_str (), ts);
    }
    stat.curr_ts++;

    if (stat.type == restart || stat.curr_ts % cfg.ts_per_hist_file == 0) {
        err = wrf_io_close (cfg, stat, ts);
    }

err_out:;
    return err;
}
int wrf_io_core (wrf_io_config &cfg) {
    int err     = 0;
    unsigned ts = 0;  // current time step; must init to 0

    file_stat hist_file (history);
    file_stat rst_file (restart);

    if (cfg.rank == 0) hist_file.print ();
    if (cfg.rank == 0) rst_file.print ();

    if (cfg.rank == 0) printf ("\n=========== time step %u\n", ts);
    // write history file 0; this time step does not need computation
    err = wrf_io_write (cfg, hist_file, ts);
    CHECK_ERR;

    for (ts = 1; ts <= cfg.ts_total; ts++) {
        if (cfg.rank == 0) printf ("\n=========== time step %u\n", ts);
        // computation time
        if (cfg.sleep_sec > 0) { sleep (cfg.sleep_sec); }

        if (ts % cfg.ts_hist_interval == 0) {
            err = wrf_io_write (cfg, hist_file, ts);
            CHECK_ERR;
        }

        if (ts % cfg.ts_rst_interval == 0) {
            err = wrf_io_write (cfg, rst_file, ts);
            CHECK_ERR;
        }
    }

    if (hist_file.status == valid) {
        ts--;
        wrf_io_close (cfg, hist_file, ts);
    }

err_out:;
    return err;
}