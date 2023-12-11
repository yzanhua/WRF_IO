#include "wrf_io_utils.hpp"

using wrf_io_utils::wrf_io_config;
using wrf_io_utils::wrf_io_file;

wrf_io_file::wrf_io_file (const file_type type)
    : type (type), status (invalid), curr_ts (0), mode (read_write) {}
wrf_io_file::wrf_io_file (const file_type type, const wrf_io_file_mode mode)
    : type (type), status (invalid), curr_ts (0), mode (mode) {}
void wrf_io_file::print () {
#ifndef WRF_IO_LOGGING
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

void wrf_io_file::reset (unsigned ts_created, wrf_io_config &cfg) {
    this->ts_created = ts_created;
    switch (this->type) {
        case history:
            this->file_name = string (cfg.out_prefix) + "wrfout";
            this->file_name += "_i" + std::to_string (cfg.iter);
            this->file_name += "_g" + std::to_string (cfg.group_num);
            this->file_name += "_we" + std::to_string (cfg.e_we);
            this->file_name += "_sn" + std::to_string (cfg.e_sn);
            this->file_name += "_c" + std::to_string (ts_created);
            break;
        case restart:
            this->file_name = string (cfg.out_prefix) + "wrfrst";
            this->file_name += "_i" + std::to_string (cfg.iter);
            this->file_name += "_g" + std::to_string (cfg.group_num);
            this->file_name += "_we" + std::to_string (cfg.e_we);
            this->file_name += "_sn" + std::to_string (cfg.e_sn);
            this->file_name += "_c" + std::to_string (ts_created);
            break;
    }
}
