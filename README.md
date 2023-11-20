## Parallel I/O Kernel Case Study -- WRF

This repository contains a case study of parallel I/O kernel from the
[WRF](https://https://github.com/wrf-model/WRF) climate simulation model.


## Installation Guide
Below is an exmaple.
```shell
autoreconf -i

./configure \
    --with-mpi=${HOME}/mpich/4.1.1 \
    --with-pnetcdf=${HOME}/pnetcdf/1.12.3 \
    --prefix=${HOME}/WRF_IO

# or on Perlmutter:
MPICC=cc \
MPICXX=CC \
./configure \
    --with-pnetcdf=${HOME}/pnetcdf/1.12.3 \
    --prefix=${HOME}/WRF_IO

make

make install
```

### Configure Options:
```
--with-mpi=/path/to/implementation
                        The installation prefix path for MPI implementation.
--with-pnetcdf[=INC,LIB | =DIR]
                        Provide the PnetCDF installation path(s):
                        --with-pnetcdf=INC,LIB for include and lib paths
                        separated by a comma. --with-pnetcdf=DIR for the
                        path containing include/ and lib/ subdirectories.
                        [default: enabled]
--enable-logging        Enable logging feature. Process 0 will log some important
                        events (creating a file, writing a file, etc)
                        [default: disabled]
--enable-debug          Enable WRF_IO internal debug mode. (compile with -g -O0
                        option) [default: disabled]
```
HDF5 is not yet supported.
## Usage Guide:
```shell
# the following export might be redundent
exdport LD_LIBRARY_PATH=${PNC_DIR}/lib:${LD_LIBRARY_PATH}

# mpirun -np [num_processes] [path/to/wrf_io] [path/to/config.json]
# a config.json file is available under examples folder
mpirun -np 2 wrf_io config.json

# content of config.json
cat config.json
{
    "api": "pnc_nb",
    "strategy": "canonical",
     "ts_total": 360,      
     "ts_hist_interval": 30,
     "ts_rst_interval": 3600,
     "ts_per_hist_file": 10000,
     "e_we": 55,
     "e_sn": 67,
     "sleep_seconds": 0,
     "out_prefix": "./wrfout_",
     "var_def_file": "./header.json",
     "debug_level": 30
}
```

### Explanation of config.json
1. `api`: currently only support `pnc_nb`, which stands for PnetCDF non-blocking apis. Will support PnetCDF blocking apis, HDF5, etc in the future.
1. `strategy`: currently not used.
1. `ts_total`: num of total simulation time steps.
1. `ts_hist_interval`: the wrf_io program will write to the history file every `ts_hist_interval`  time steps. So the toal time steps of history file(s) would be `1 + int(ts_total / ts_hist_interval)`.
1. `ts_rst_interval`: the wrf_io program will write to the restart file every `ts_rst_interval`  time steps. If `ts_rst_interval > ts_total`, then no restart file will be generated.
1. `ts_per_hist_file`: how many time steps a history file can maximumly contain.
1. `e_we` and `e_sn`: grid sizes
1. `sleep_seconds`: time to sleep per time step, to simulate computation. Currently not used.
1. `out_prefix`: prefix of output files, for example, we can set to `/pscratch/sd/u/user_name/FS_1M_8/wrf_io_`
1. `var_def_file`: path to the `header.json` file. The `header.json` file is the json version of a NetCDF file header. An example `header.json` is under the `examples` folder, which is produced by converting the header of a valid WRF output (`exmamples/header.txt`) using the python program `examples/parse_header.py`.
1. `debug_level`: (later will be renamed to `logging_level`) possible options: `0`, `10`, `20`, `30`, and `40`. The higher the more details when process 0 outputs logging. Only takes effect when the program is configured with `--enable-logging`.

For both the `config.json` file and the `header.json` file,
WRF_IO will use process 0 to read the files, and broadcast the contents to other processes.

## TODO:
1. add profiling
1. add other I/O options (pnc-b, hdf5, adios, etc)



 
