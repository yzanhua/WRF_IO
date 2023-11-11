#include "wrf_io_utils.hpp"

using wrf_io_utils::wrf_io_file;

wrf_io_file::wrf_io_file (const file_type type) : type (type), status (invalid), curr_ts (0) {}

void wrf_io_file::print () {
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

void wrf_io_file::clear () {
    this->ts_created = 0;
    this->curr_ts    = 0;
    this->file_name  = "";
    this->status     = invalid;
}

void wrf_io_file::reset (unsigned ts_created, char *out_prefix) {
    this->ts_created = ts_created;
    switch (this->type) {
        case history:
            this->file_name = string (out_prefix) + "wrfout_" + std::to_string (ts_created);
            break;
        case restart:
            this->file_name = string (out_prefix) + "wrfrst_" + std::to_string (ts_created);
            break;
    }
}