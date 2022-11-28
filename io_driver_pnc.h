#pragma once

#include "io_driver.h"

class io_driver_pnc : public io_driver {
   private:
    bool is_blocking;  // whether blocking PnetCDF

   public:
    io_driver_pnc () {};
    ~io_driver_pnc () {};
    int create (std::string path, MPI_Comm comm, MPI_Info info, int *fid);
    int open (std::string path, MPI_Comm comm, MPI_Info info, int *fid);
    int close (int fid);
    int def_var ();
    int def_local_var ();
    int def_dim (int fid, std::string name, MPI_Offset size, int *dimid);
    int enddef (int fid);
    int redef (int fid);
    int put_att ();
    int put_vara ();
    int begin_step ();
    int end_step ();
};