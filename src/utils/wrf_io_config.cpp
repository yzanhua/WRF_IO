#include "wrf_io_utils.hpp"

using wrf_io_utils::wrf_io_config;

void wrf_io_config::print (bool rank0_only = true) {
    if (rank0_only && this->rank != 0) { return; }
    printf ("=============== config options:\n");

    switch (this->api) {
        case pnc_b:
            printf ("IO method: PnetCDF blocking APIs\n");
            break;
        case pnc_nb:
            printf ("IO method: PnetCDF non-blocking APIs\n");
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

    printf ("Domain size west-east: %u\n", this->e_we);
    printf ("Domain size south-north: %u\n", this->e_sn);

    printf ("Sleep time to smulate computations: %u sec\n", this->sleep_sec);

    printf ("Output prefix: %s\n", this->out_prefix);
    printf ("Var def file: %s\n", this->var_def_file);
}

bool wrf_io_config::check_config() {return true;}