#pragma once
#ifndef _SRC_IO_DRIVERS_WRF_IO_DRIVERS_HPP__
#define _SRC_IO_DRIVERS_WRF_IO_DRIVERS_HPP__

#include <mpi.h>

#include "wrf_io_utils.hpp"

using wrf_io_utils::attribute;
using wrf_io_utils::dimension;
using wrf_io_utils::variable;
using wrf_io_utils::wrf_io_config;
using wrf_io_utils::wrf_io_file;

namespace wrf_io_driver {

class io_driver {
   protected:
    // MPI_Offset amount_WR;
    // MPI_Offset amount_RD;

   public:
    io_driver () {};
    virtual ~io_driver () {};
    virtual int create (wrf_io_file &file, wrf_io_config &config) = 0;
    virtual int open (wrf_io_file &file, wrf_io_config &config)   = 0;
    virtual int close (wrf_io_file &file)                         = 0;
    virtual int def_var (wrf_io_file &file, variable &var)        = 0;
    // virtual int def_local_var ()                = 0;
    virtual int def_dim (wrf_io_file &file, dimension &dim, MPI_Offset dim_size)  = 0;
    virtual int enddef (wrf_io_file &file)                                        = 0;
    virtual int redef (wrf_io_file &file)                                         = 0;
    virtual int put_att (wrf_io_file &file, variable &var, attribute &attr)       = 0;
    virtual int put_var (wrf_io_file &file, variable &var, wrf_io_config &config) = 0;
    virtual int begin_step (wrf_io_file &file)                                    = 0;
    virtual int end_step (wrf_io_file &file)                                      = 0;
};  // class io_driver

io_driver *get_io_driver (wrf_io_config &cfg);

}  // namespace wrf_io_driver

#endif  // _SRC_IO_DRIVERS_WRF_IO_DRIVERS_HPP__