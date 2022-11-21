#pragma once

#include "wrf_io.h"

int wrf_io_create_and_define (wrf_io_config &cfg, file_stat &stat, unsigned ts);
int wrf_io_close (wrf_io_config &cfg, file_stat &stat, unsigned ts);
int wrf_io_write (wrf_io_config &cfg, file_stat &stat, unsigned ts);
int wrf_io_core (wrf_io_config &cfg);