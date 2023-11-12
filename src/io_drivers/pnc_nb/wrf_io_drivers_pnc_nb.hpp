#pragma once
#ifndef _SRC_IO_DRIVERS_WRF_IO_DRIVERS_PNC_NB_HPP__
#define _SRC_IO_DRIVERS_WRF_IO_DRIVERS_PNC_NB_HPP__

#include <mpi.h>

#include "wrf_io_drivers.hpp"
#include "wrf_io_utils.hpp"


using wrf_io_utils::wrf_io_config;

using wrf_io_utils::attribute;
using wrf_io_utils::dimension;
using wrf_io_utils::variable;
using wrf_io_utils::wrf_io_file;


namespace wrf_io_driver {

class io_driver_pnc_nb : public io_driver {
   protected:
    // MPI_Offset amount_WR;
    // MPI_Offset amount_RD;

   public:
    io_driver_pnc_nb () {};
    ~io_driver_pnc_nb () {};

    int create (wrf_io_file &file, wrf_io_config &config);
    int open (wrf_io_file &file, wrf_io_config &config);
    int close (wrf_io_file &file);
    int def_var (wrf_io_file &file, variable &var);
    int def_dim (wrf_io_file &file, dimension &dim, MPI_Offset dim_size);
    int enddef (wrf_io_file &file);
    int redef (wrf_io_file &file);
    int put_att (wrf_io_file &file, variable &var, attribute &attr);
    int put_var (wrf_io_file &file, variable &var, wrf_io_config &config);
    int begin_step (wrf_io_file &file);
    int end_step (wrf_io_file &file);
};  // class io_driver_pnc_nb

}  // namespace wrf_io_driver

#endif  // _SRC_IO_DRIVERS_WRF_IO_DRIVERS_PNC_NB_HPP__