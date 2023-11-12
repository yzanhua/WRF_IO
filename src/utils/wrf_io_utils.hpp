#pragma once
#ifndef _SRC_UTILS_WRF_IO_UTILS_H__
#define _SRC_UTILS_WRF_IO_UTILS_H__

#include <mpi.h>

#include <string>
#include <unordered_map>
#include <vector>

using std::string;
using std::unordered_map;
using std::vector;

namespace wrf_io_utils {
const size_t MAX_PATH_LEN = 1024;
const int ERROR           = -1;
const int NO_ERROR        = 0;

typedef enum { history, restart } file_type;
typedef enum { invalid, valid } io_status;
typedef enum wrf_io_api { pnc_b, pnc_nb, undef_api } wrf_io_api;
typedef enum wrf_io_strategy { canonical, blob, log, undef_io } wrf_io_strategy;
typedef enum wrf_io_file_mode { read_only, write_only, read_write } wrf_io_file_mode ;

class wrf_io_file {
   public:
    unsigned ts_created;
    unsigned curr_ts;
    string file_name;
    io_status status;
    int fid;
    const file_type type;

    // for pnc_nb
    size_t write_size_per_ts     = 0;
    bool write_size_per_ts_valid = false;
    wrf_io_file_mode mode;

    // file_stat(unsigned ts_created, file_type type, char* out_prefix);
    wrf_io_file (file_type type);
    wrf_io_file (file_type type, wrf_io_file_mode mode);
    void print ();
    void clear ();
    void reset (unsigned ts_created, char *out_prefix);
};

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
    unsigned e_we;  // domain size along west-east
    unsigned e_sn;  // domain size along south-north

    // out_put_folder
    char out_prefix[MAX_PATH_LEN];
    char var_def_file[MAX_PATH_LEN];

    // sleep time
    unsigned sleep_sec;  // sleep time to emulate computations.

    // debug level
    int debug_level;

    void print (bool rank0_only);
    bool check_config ();

} wrf_io_config;

class dimension {
   public:
    static unordered_map<string, dimension> dim_name_to_dim;
    static dimension *unlimited_dim_ptr;

    string name;
    int value;
    int dimid = -1;

    void print (int indent);
};

class attribute {
   public:
    static vector<attribute> global_attrs;

    string name;
    string type;
    string str_value;
    int int_value;
    float float_value;
    int attrid = -1;

    void print (int indent);
};

class variable {
   public:
    static vector<variable> variables;
    static vector<int> pt_starts;  // [sn, we]
    static vector<int> pt_counts;  // [sn, we]
    static void calculate_parition (wrf_io_config &config);
    static void print_parition ();

    string name;
    string type;
    int varid = -1;
    int num_attrs;
    int num_dims;
    bool is_partition;

    char *data        = NULL;
    size_t data_size  = 0;  // size in bytes
    size_t data_count = 0;  // size in data elements
    void set_data ();

    vector<attribute> attrs;
    vector<string> dim_names;

    void print (int indent);

    ~variable () {
        if (data != NULL) {
            free (data);
            data = NULL;
        }
    }
};

void check_err (int err, int line, const char *file_name, const char *msg);
int read_json_file (const char *file_name, wrf_io_config &cfg);
int get_config (const char *file_name, wrf_io_config &config);
int get_variables_attrs (wrf_io_config &config);

}  // namespace wrf_io_utils

#endif