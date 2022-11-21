#pragma once

#include <mpi.h>

#include <string>

#define WRF_IO_MAX_PATH 1024

using std::string;

typedef enum wrf_io_api { pnetcdf, undef_api } wrf_io_api;

typedef enum wrf_io_strategy { canonical, blob, log, undef_io } wrf_io_strategy;

typedef enum { history, restart } file_type;

typedef enum { invalid, valid } io_status;

typedef struct wrf_io_config {
    // mpi related
    int rank;
    int np;
    MPI_Comm io_comm;
    MPI_Info info;

    // io methods
    wrf_io_api api;
    wrf_io_strategy strategy;

    unsigned ts_total;          // total number of time steps;
    unsigned ts_hist_interval;  // produce a history frame every this interval
    unsigned ts_rst_interval;   // produce a restart frame every this interval
    unsigned ts_per_hist_file;  //  num of time steps per history files

    // domain size
    unsigned domain_we;  // domain size along west-east
    unsigned domain_sn;  // domain size along south-north

    // out_put_folder
    char out_prefix[WRF_IO_MAX_PATH];

    // sleep time
    unsigned sleep_sec;  // sleep time to emulate computations.

    void print ();
    int check_config ();

} wrf_io_config;

typedef struct file_stat {
    unsigned ts_created;
    unsigned curr_ts;
    string file_name;
    io_status status;
    const file_type type;

    // file_stat(unsigned ts_created, file_type type, char* out_prefix);
    file_stat (file_type type);
    void print ();
    void clear ();
    void reset (unsigned ts_created, char *out_prefix);
} file_stat;

int init_config (wrf_io_config &cfg);
int parse_user_args (wrf_io_config &cfg, int argc, char **argv);
