#include "wrf_io_utils.hpp"

#include <mpi.h>

void wrf_io_utils::check_err (int err, int line, const char *file_name, const char *msg) {
    if (err != NO_ERROR) {
        printf ("Error at %s:%d: %s\n", file_name, line, msg);
        MPI_Abort (MPI_COMM_WORLD, err);
    }
}