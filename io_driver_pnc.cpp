#include "io_driver_pnc.h"

int io_driver_pnc::create (std::string path, MPI_Comm comm, MPI_Info info, int *fid) {}
int io_driver_pnc::open (std::string path, MPI_Comm comm, MPI_Info info, int *fid) {}
int io_driver_pnc::close (int fid) {}
int io_driver_pnc::def_var () {}
int io_driver_pnc::def_local_var () {}
int io_driver_pnc::def_dim (int fid, std::string name, MPI_Offset size, int *dimid) {}
int io_driver_pnc::enddef (int fid) {}
int io_driver_pnc::redef (int fid) {}
int io_driver_pnc::put_att () {}
int io_driver_pnc::put_vara () {}
int io_driver_pnc::begin_step () {}
int io_driver_pnc::end_step () {}