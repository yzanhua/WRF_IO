#include "wrf_io_drivers.hpp"

#include "wrf_io.hpp"
#include "wrf_io_utils.hpp"

#ifdef USE_PNETCDF
#include "pnc_nb/wrf_io_drivers_pnc_nb.hpp"
#endif

using wrf_io_driver::io_driver;
using wrf_io_utils::wrf_io_config;

io_driver *wrf_io_driver::get_io_driver (wrf_io_config &cfg) {
    if (cfg.api == wrf_io_utils::pnc_nb) {
        if (cfg.strategy == wrf_io_utils::canonical) {
#ifdef USE_PNETCDF
            return new wrf_io_driver::io_driver_pnc_nb ();
#else
            return nullptr;
#endif
        }
    }
    return nullptr;
}