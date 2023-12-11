#include "wrf_io_utils.hpp"

#include <mpi.h>

static MPI_Comm static_comm = MPI_COMM_WORLD;
void wrf_io_utils::check_err (int err, int line, const char *file_name, const char *msg) {
    if (err != NO_ERROR) {
        printf ("Error at %s:%d: %s\n", file_name, line, msg);
        MPI_Abort (static_comm, err);
    }
}

void wrf_io_utils::check_err_set_comm (MPI_Comm comm) {
    static_comm = comm;
}