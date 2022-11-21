#include "wrf_io.h"

#include <mpi.h>

#include "wrf_io_err.h"
#include "wrf_io_core.h"

int main (int argc, char **argv) {
    int err;

    wrf_io_config cfg;

#ifdef WRF_IO_THREADING
    {
        int mpi_provided = 0;
        err              = MPI_Init_thread (&argc, &argv, MPI_THREAD_MULTIPLE, &mpi_provided);
        CHECK_ERR;

        if (mpi_provided != MPI_THREAD_MULTIPLE) { ERR_OUT ("MPI_Init_thread failed"); }
    }
#else
    err = MPI_Init (&argc, &argv);
    CHECK_ERR;
#endif

    err = init_config (cfg);
    CHECK_ERR;
    err = parse_user_args (cfg, argc, argv);
    CHECK_ERR;

    cfg.print ();

    err = cfg.check_config ();
    CHECK_ERR;

    err = wrf_io_core (cfg);

err_out:;
    if (cfg.info != MPI_INFO_NULL) MPI_Info_free (&(cfg.info));
    if (cfg.io_comm != MPI_COMM_WORLD && cfg.io_comm != MPI_COMM_NULL)
        MPI_Comm_free (&(cfg.io_comm));
    MPI_Finalize ();

    return (err < 0) ? 1 : 0;
}