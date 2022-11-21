#include <getopt.h>
#include <stdlib.h>  // atoi
#include <string.h>  // strcmp(), strcpy(), strncpy()
#include <unistd.h>  // getopt()

#include "wrf_io.h"
#include "wrf_io_err.h"

static void usage (char *argv0);

int init_config (wrf_io_config &cfg) {
    cfg.io_comm = MPI_COMM_WORLD;
    cfg.info    = MPI_INFO_NULL;
    MPI_Comm_rank (MPI_COMM_WORLD, &(cfg.rank));
    MPI_Comm_size (MPI_COMM_WORLD, &(cfg.np));

    cfg.api      = undef_api;
    cfg.strategy = canonical;

    cfg.ts_total         = 5;             // total number of time steps;
    cfg.ts_hist_interval = 1;             // produce a history frame every this interval
    cfg.ts_rst_interval  = cfg.ts_total;  // produce a restart frame every this interval
    cfg.ts_per_hist_file = cfg.ts_total;  //  num of time steps per history files

    // domain size
    cfg.domain_we = 40;  // domain size along west-east
    cfg.domain_sn = 40;  // domain size along south-north

    // out_put_folder
    cfg.out_prefix[0] = '.';
    cfg.out_prefix[1] = '\0';

    // sleep time
    cfg.sleep_sec = 0;
    return 0;
}
int parse_user_args (wrf_io_config &cfg, int argc, char **argv) {
    int i, err = 0;
    int opt_idx = 0;

    static struct option long_options[] = {{"api", required_argument, NULL, 'a'},
                                           {"strategy", required_argument, NULL, 'x'},
                                           {"ts_total", required_argument, NULL, 0},
                                           {"ts_hist_interval", required_argument, NULL, 1},
                                           {"ts_rst_interval", required_argument, NULL, 2},
                                           {"ts_per_hist_file", required_argument, NULL, 3},
                                           {"domain_we", required_argument, NULL, 4},
                                           {"domain_sn", required_argument, NULL, 5},
                                           {"out_prefix", required_argument, NULL, 'o'},
                                           {"sleep_sec", required_argument, NULL, 't'},
                                           {"help", no_argument, NULL, 'h'},
                                           {NULL, 0, NULL, 0}};

    while ((i = getopt_long (argc, argv, "a:x:o:", long_options, &opt_idx)) != EOF) {
        switch (i) {
            case 'a':
                if (strcmp (optarg, "pnetcdf") == 0) cfg.api = pnetcdf;
                // else if (strcmp (optarg, "hdf5") == 0)
                //     cfg.api = hdf5;
                // else if (strcmp (optarg, "hdf5_md") == 0)
                //     cfg.api = hdf5_md;
                // else if (strcmp (optarg, "hdf5_log") == 0)
                //     cfg.api = hdf5_log;
                // else if (strcmp (optarg, "netcdf4") == 0)
                //     cfg.api = netcdf4;
                // else if (strcmp (optarg, "adios") == 0)
                //     cfg.api = adios;
                else
                    ERR_OUT ("Unknown API");
                break;
            case 'x':
                if (strcmp (optarg, "canonical") == 0)
                    cfg.strategy = canonical;
                else if (strcmp (optarg, "log") == 0)
                    cfg.strategy = log;
                else if (strcmp (optarg, "blob") == 0)
                    cfg.strategy = blob;
                else
                    ERR_OUT ("Unknown I/O strategy")
                break;
            case 't':
                cfg.sleep_sec = atoi (optarg);
                break;
            case 'o':
                strncpy (cfg.out_prefix, optarg, WRF_IO_MAX_PATH);
                break;
            case 0:
                cfg.ts_total = atoi (optarg);
                break;
            case 1:
                cfg.ts_hist_interval = atoi (optarg);
                break;
            case 2:
                cfg.ts_rst_interval = atoi (optarg);
                break;
            case 3:
                cfg.ts_per_hist_file = atoi (optarg);
                break;
            case 4:
                cfg.domain_we = atoi (optarg);
                break;
            case 5:
                cfg.domain_sn = atoi (optarg);
                break;
            case 'h':
            default:
                if (cfg.rank == 0) usage (argv[0]);
                goto err_out;
        }
    }
    // if (optind >= argc || argv[optind] == NULL) { /* input file is mandatory */
    //     if (!cfg.rank) usage (argv[0]);
    //     ERR_OUT ("Decomposition file not provided")
    // }

err_out:
    return err;
}

/*----< usage() >------------------------------------------------------------*/
static void usage (char *argv0) {
    const char *help =
        "Usage: %s [OPTION]\n\
       [-h, --help] Print this help message\n\
       [-t, --time; time] Add sleep time to emulate the computation in order to \n\
                 overlapping I/O when Async VOL is used.\n\
       [-o, --out_prefix; path] Prefix for output files \n\
       [-a, --api; api]  I/O library name\n\
           pnetcdf:   PnetCDF library (default)\n\
           netcdf4:   NetCDF-4 library\n\
           hdf5:      HDF5 library\n\
           hdf5_md:   HDF5 library using multi-dataset I/O APIs\n\
           hdf5_log:  HDF5 library with Log-based VOL\n\
           adios:     ADIOS library using BP3 format\n\
       [-x, --strategy; strategy] I/O strategy\n\
           canonical: Store variables in the canonical layout (default).\n\
           log:       Store variables in the log-based storage layout.\n\
           blob:      Pack and store all data written locally in a contiguous\n\
                      block (blob), ignoring variable's canonical order.\n\
";
    fprintf (stderr, help, argv0);
}

void wrf_io_config::print () {
#ifndef WRF_IO_DEBUG
    return;
#endif
    if (this->rank != 0) { return; }

    // io methods
    switch (this->api) {
        case pnetcdf:
            printf ("IO method: PnetCDF\n");
            break;
        case undef_api:
        default:
            printf ("IO method: UNDEF\n");
            break;
    }
    switch (this->strategy) {
        case canonical:
            printf ("IO strategy: canonical\n");
            break;
        case blob:
            printf ("IO strategy: blob\n");
            break;
        case log:
            printf ("IO strategy: log\n");
            break;
        case undef_io:
        default:
            printf ("IO strategy: undef_io\n");
            break;
    };

    printf ("Total number of time steps: %u\n", this->ts_total);
    printf ("Hist files interval: %u time steps\n", this->ts_hist_interval);
    printf ("Restart files interval: %u time steps\n", this->ts_rst_interval);
    printf ("Number of time steps per history file: %u\n", this->ts_per_hist_file);

    printf ("Domain size west-east: %u\n", this->domain_we);
    printf ("Domain size south-north: %u\n", this->domain_sn);

    printf ("Sleep time to smulate computations: %u sec\n", this->sleep_sec);

    printf ("Output prefix: %s\n", this->out_prefix);
}

int wrf_io_config::check_config () {
    int err = 0;
err_out:;
    return err;
}

file_stat::file_stat (const file_type type) : type (type), status (invalid), curr_ts (0) {}

void file_stat::print () {
#ifndef WRF_IO_DEBUG
    return;
#endif

    switch (this->status) {
        case invalid:
            printf ("File status: invalid\n");
            return;
            break;
        case valid:
            printf ("File status: Opened, vars defined.\n");
            break;
        default:
            printf ("Error: Should not acchieve here.\n");
            break;
    }
    printf ("File name: %s\n", this->file_name.c_str ());
    printf ("Created time step: %u\n", this->ts_created);
    switch (type) {
        case history:
            printf ("File type: history\n");
            break;
        case restart:
            printf ("File type: restart\n");
            break;
        default:
            break;
    }

    printf ("File current time steps: %u\n", this->curr_ts);
}

void file_stat::clear () {
    this->ts_created = 0;
    this->curr_ts    = 0;
    this->file_name  = "";
    this->status     = invalid;
}

void file_stat::reset (unsigned ts_created, char *out_prefix) {
    this->ts_created = ts_created;
    switch (this->type)
    {
    case history:
        this->file_name  = string (out_prefix) + "/wrfout_" + std::to_string (ts_created);
        break;
    case restart:
        this->file_name  = string (out_prefix) + "/restart_" + std::to_string (ts_created);
        break;
    }
    
}