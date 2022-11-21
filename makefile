PNETCDF_DIR=${LOCAL_HOME}/PnetCDF/1.12.3

FLAGS=-DWRF_IO_DEBUG

all:
	mpicxx ${FLAGS} \
		wrf_io.cpp \
		wrf_io_utils.cpp \
		wrf_io_core.cpp \
		-I${PNETCDF_DIR}/include \
		-L${PNETCDF_DIR}/lib \
		-lpnetcdf \
		-o test

run:
	LD_LIBRARY_PATH=${PNETCDF_DIR}/lib:${LD_LIBRARY_PATH} \
	mpirun -np 2 ./test \
	--api=pnetcdf --strategy=blob \
    --ts_total=10 \
    --ts_hist_interval=2 \
    --ts_rst_interval=5 \
    --ts_per_hist_file=4 \
    --domain_we=50 \
    --domain_sn=50 \
	-o outs \
    --sleep_sec=0


clean:
	rm -rf test