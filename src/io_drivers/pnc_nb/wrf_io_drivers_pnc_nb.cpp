#include "wrf_io_drivers_pnc_nb.hpp"

#include "pnetcdf.h"
#include "wrf_io.hpp"
#include "wrf_io_utils.hpp"
#include "wrf_io_logging.hpp"

using wrf_io_driver::io_driver_pnc_nb;

using wrf_io_utils::attribute;
using wrf_io_utils::check_err;
using wrf_io_utils::variable;

nc_type str2nctype (string type);
void *get_attr_buf_from_type (attribute &attr);
MPI_Offset get_attr_len_from_type (attribute &attr);
MPI_Datatype str2MPIDatatype (string type);

int io_driver_pnc_nb::create (wrf_io_file &file, wrf_io_config &config) {
    ENTER;
    int err   = 0;
    int cmode = NC_CLOBBER | NC_64BIT_OFFSET;
    err = ncmpi_create (config.io_comm, file.file_name.c_str (), cmode, config.info, &(file.fid));
    // ncmpi_set_fill(*fid, NC_NOFILL, NULL);
    check_err (err, __LINE__, __FILE__, "Error in ncmpi_create");
    LEAVE;
    return err;
}  // end io_driver_pnc_nb::create

int io_driver_pnc_nb::open (wrf_io_file &file, wrf_io_config &config) {
    ENTER;
    int err   = 0;
    int omode = 0;
    if (file.mode == wrf_io_utils::read_only) {
        omode = NC_NOWRITE;
    } else {
        omode = NC_WRITE;
    }
    err = ncmpi_open (config.io_comm, file.file_name.c_str (), omode, config.info, &(file.fid));
    // ncmpi_set_fill(*fid, NC_NOFILL, NULL);
    check_err (err, __LINE__, __FILE__, "Error in ncmpi_create");
    LEAVE;

    return err;
}  // end io_driver_pnc_nb::open

int io_driver_pnc_nb::close (wrf_io_file &file) {
    ENTER;
    int err = 0;
    err     = ncmpi_close (file.fid);
    check_err (err, __LINE__, __FILE__, "Error in ncmpi_create");
    LEAVE;

    return err;
}  // end io_driver_pnc_nb::close

int io_driver_pnc_nb::def_var (wrf_io_file &file, variable &var) {
    ENTER;
    int err     = 0;
    int ndims   = var.num_dims + 1;
    int *dimids = new int[ndims];
    dimids[0]   = dimension::unlimited_dim_ptr->dimid;
    for (int i = 0; i < var.num_dims; i++) {
        dimids[i + 1] = dimension::dim_name_to_dim[var.dim_names[i]].dimid;
    }
    err = ncmpi_def_var (file.fid, var.name.c_str (), str2nctype (var.type), ndims, dimids,
                         &(var.varid));
    delete[] dimids;
    check_err (err, __LINE__, __FILE__, "Error in ncmpi_def_var");
    LEAVE;

    return err;
}  // end io_driver_pnc_nb::def_var

int io_driver_pnc_nb::def_dim (wrf_io_file &file, dimension &dim, MPI_Offset dim_size) {
    ENTER;
    int err = 0;
    if (dim_size == -1) { dim_size = NC_UNLIMITED; }
    err = ncmpi_def_dim (file.fid, dim.name.c_str (), dim_size, &(dim.dimid));
    LEAVE;

    return err;
}  // end io_driver_pnc_nb::def_dim

int io_driver_pnc_nb::enddef (wrf_io_file &file) {
    ENTER;
    int err = ncmpi_enddef (file.fid);
    check_err (err, __LINE__, __FILE__, "Error in ncmpi_create");
    LEAVE;

    return err;
}  // end io_driver_pnc_nb::enddef

int io_driver_pnc_nb::redef (wrf_io_file &file) {
    ENTER;
    int err = ncmpi_redef (file.fid);
    check_err (err, __LINE__, __FILE__, "Error in ncmpi_create");
    LEAVE;

    return err;
}  // end io_driver_pnc_nb::redef

int io_driver_pnc_nb::put_att (wrf_io_file &file, variable &var, attribute &attr) {
    ENTER;
    int err        = 0;
    int varid      = var.varid;
    void *buf      = get_attr_buf_from_type (attr);
    MPI_Offset len = get_attr_len_from_type (attr);
    if (varid == -1) varid = NC_GLOBAL;
    err = ncmpi_put_att (file.fid, varid, attr.name.c_str (), str2nctype (attr.type), len, buf);
    check_err (err, __LINE__, __FILE__, "Error in ncmpi_create");
    LEAVE;

    return err;
}  // end io_driver_pnc_nb::put_att

int io_driver_pnc_nb::put_var (wrf_io_file &file, variable &var, wrf_io_config &config) {
    // int io_driver_pnc_nb::put_var (file_stat &stat, variable &var, int rank) {
    if (!var.is_partition && config.rank != 0) return 0;
    ENTER_MSG(var.name.c_str ());
    int err             = 0;
    MPI_Offset start[4] = {file.curr_ts, 0, 0, 0};
    MPI_Offset count[4] = {1, 0, 0, 0};
    int ndims           = var.num_dims + 1;
    int reqid           = -1;
    for (int i = 0; i < var.num_dims; i++) {
        count[i + 1] = dimension::dim_name_to_dim[var.dim_names[i]].value;
        // static vector<int> pt_starts;   // [sn, we]
        // static vector<int> pt_counts;   // [sn, we]
        if (count[i + 1] == -1) {  // E_WE
            count[i + 1] = variable::pt_counts[1];
            start[i + 1] = variable::pt_starts[1];
        } else if (count[i + 1] == -2) {  // E_SN
            count[i + 1] = variable::pt_counts[0];
            start[i + 1] = variable::pt_starts[0];
        }
    }
    err = ncmpi_bput_vara (file.fid, var.varid, start, count, var.data, var.data_count,
                           str2MPIDatatype (var.type), &reqid);
    LEAVE_MSG(var.name.c_str ());

    return 0;
}  // end io_driver_pnc_nb::put_var

int io_driver_pnc_nb::begin_step (wrf_io_file &file) {
    ENTER;
    int err = 0;
    if (!file.write_size_per_ts_valid) {
        file.write_size_per_ts = 0;
        for (const auto &var : variable::variables) { file.write_size_per_ts += var.data_size; }
        file.write_size_per_ts_valid = true;
    }
    err = ncmpi_buffer_attach (file.fid, file.write_size_per_ts);
    check_err (err, __LINE__, __FILE__, "Error in ncmpi_buffer_attach");
    LEAVE;

    return err;
}  // end io_driver_pnc_nb::begin_step

int io_driver_pnc_nb::end_step (wrf_io_file &file) {
    ENTER;
    int err = 0;
    err     = ncmpi_wait_all (file.fid, NC_REQ_ALL, NULL, NULL);
    check_err (err, __LINE__, __FILE__, "Error in ncmpi_wait_all");

    err = ncmpi_buffer_detach (file.fid);
    check_err (err, __LINE__, __FILE__, "Error in ncmpi_buffer_detach");
    LEAVE;

    return err;
}  // end io_driver_pnc_nb::end_step

void *get_attr_buf_from_type (attribute &attr) {
    if (attr.type == "int") {
        return &(attr.int_value);
    } else if (attr.type == "float") {
        return &(attr.float_value);
    } else if (attr.type == "double") {
        return &(attr.float_value);
    } else if (attr.type == "char" || attr.type == "str") {
        return (void *)(attr.str_value.c_str ());
    } else {
        return NULL;
    }
}  // end get_attr_buf_from_type

MPI_Offset get_attr_len_from_type (attribute &attr) {
    if (attr.type == "int") {
        return 1;
    } else if (attr.type == "float") {
        return 1;
    } else if (attr.type == "double") {
        return 1;
    } else if (attr.type == "char" || attr.type == "str") {
        return attr.str_value.size ();
    } else {
        return -1;
    }
}  // end get_attr_len_from_type

nc_type str2nctype (string type) {
    if (type == "int") {
        return NC_INT;
    } else if (type == "float") {
        return NC_FLOAT;
    } else if (type == "double") {
        return NC_DOUBLE;
    } else if (type == "char" || type == "str") {
        return NC_CHAR;
    } else {
        return NC_NAT;
    }
}  // end str2nctype

MPI_Datatype str2MPIDatatype (string type) {
    if (type == "int") {
        return MPI_INT;
    } else if (type == "float") {
        return MPI_FLOAT;
    } else if (type == "double") {
        return MPI_DOUBLE;
    } else if (type == "char" || type == "str") {
        return MPI_CHAR;
    } else {
        return MPI_DATATYPE_NULL;
    }
}
