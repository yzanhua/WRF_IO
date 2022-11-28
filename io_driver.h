#pragma once

#include <string>

#include "mpi.h"

class io_driver {
   protected:
    // MPI_Offset amount_WR;
    // MPI_Offset amount_RD;

   public:
    // e3sm_io_config *cfg;

    io_driver () {};
    virtual ~io_driver () {};
    virtual int create (std::string path, MPI_Comm comm, MPI_Info info, int *fid) = 0;
    virtual int open (std::string path, MPI_Comm comm, MPI_Info info, int *fid)   = 0;
    virtual int close (int fid)                                                   = 0;
    virtual int def_var ()                                                        = 0;
    virtual int def_local_var ()                                                  = 0;
    virtual int def_dim (int fid, std::string name, MPI_Offset size, int *dimid)  = 0;
    virtual int enddef (int fid) = 0;
    virtual int redef (int fid)  = 0;
    virtual int put_att ()       = 0;
    virtual int put_vara ()      = 0;
    virtual int begin_step ()    = 0;
    virtual int end_step ()      = 0;
};