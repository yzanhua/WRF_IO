#include "wrf_io_utils.hpp"

#include <fstream>

#include "json.hpp"
#include "mpi.h"

using json = nlohmann::json;
using wrf_io_utils::attribute;
using wrf_io_utils::dimension;
using wrf_io_utils::variable;

using wrf_io_utils::check_err;
using wrf_io_utils::ERROR;

void parse_json_data (json &jsonData);
void parse_dim (json &jsonData, dimension &dim);
void parse_attr (json &jsonData, attribute &attr);
void parse_var (json &jsonData, variable &var);

int wrf_io_utils::read_json_file (const char *file_name, wrf_io_config &cfg) {
    json jsonData;
    std::ifstream file (file_name);

    const string keys[] = {"api",
                           "strategy",
                           "ts_total",
                           "ts_hist_interval",
                           "ts_rst_interval",
                           "ts_per_hist_file",
                           "e_we",
                           "e_sn",
                           "sleep_seconds",
                           "out_prefix",
                           "var_def_file"};

    if (!file.is_open ()) {
        printf ("Error opening file.\n");
        return ERROR;
    }
    file >> jsonData;
    file.close ();

    if (!jsonData.is_object ()) {
        printf ("Error: json data is not an object.\n");
        return ERROR;
    }

    for (const auto &key : keys) {
        if (jsonData.find (key) == jsonData.end ()) {
            printf ("Error: %s is not found.\n", key.c_str ());
            return ERROR;
        }
    }

    // api
    if (jsonData["api"] == "pnc_b")
        cfg.api = pnc_b;
    else if (jsonData["api"] == "pnc_nb") {
        cfg.api = pnc_nb;
    } else {
        cfg.api = undef_api;
        check_err (ERROR, __LINE__, __FILE__, "Error: unknown api.\n");
        return ERROR;
    }

    // strategy
    if (jsonData["strategy"] == "canonical")
        cfg.strategy = canonical;
    else if (jsonData["strategy"] == "blob")
        cfg.strategy = blob;
    else if (jsonData["strategy"] == "log")
        cfg.strategy = log;
    else {
        cfg.strategy = undef_io;
        check_err (ERROR, __LINE__, __FILE__, "Error: unknown strategy.\n");
        return ERROR;
    }

    // ts_total
    if (!jsonData["ts_total"].is_number_integer ()) {
        check_err (ERROR, __LINE__, __FILE__, "Error: ts_total is not an integer.\n");
        return ERROR;
    } else {
        cfg.ts_total = jsonData["ts_total"];
    }

    // ts_hist_interval
    if (!jsonData["ts_hist_interval"].is_number_integer ()) {
        check_err (ERROR, __LINE__, __FILE__, "Error: ts_hist_interval is not an integer.\n");
        return ERROR;
    } else {
        cfg.ts_hist_interval = jsonData["ts_hist_interval"];
    }

    // ts_rst_interval
    if (!jsonData["ts_rst_interval"].is_number_integer ()) {
        check_err (ERROR, __LINE__, __FILE__, "Error: ts_rst_interval is not an integer.\n");
        return ERROR;
    } else {
        cfg.ts_rst_interval = jsonData["ts_rst_interval"];
    }

    // ts_per_hist_file
    if (!jsonData["ts_per_hist_file"].is_number_integer ()) {
        check_err (ERROR, __LINE__, __FILE__, "Error: ts_per_hist_file is not an integer.\n");
        return ERROR;
    } else {
        cfg.ts_per_hist_file = jsonData["ts_per_hist_file"];
    }

    // e_we
    if (!jsonData["e_we"].is_number_integer ()) {
        check_err (ERROR, __LINE__, __FILE__, "Error: e_we is not an integer.\n");
        return ERROR;
    } else {
        cfg.e_we = jsonData["e_we"];
    }

    // e_sn
    if (!jsonData["e_sn"].is_number_integer ()) {
        check_err (ERROR, __LINE__, __FILE__, "Error: e_sn is not an integer.\n");
        return ERROR;
    } else {
        cfg.e_sn = jsonData["e_sn"];
    }

    // sleep_seconds
    if (!jsonData["sleep_seconds"].is_number_integer ()) {
        check_err (ERROR, __LINE__, __FILE__, "Error: sleep_seconds is not an integer.\n");
        return ERROR;
    } else {
        cfg.sleep_sec = jsonData["sleep_seconds"];
    }

    // out_prefix
    // init out_prefix to 0
    if (!jsonData["out_prefix"].is_string ()) {
        check_err (ERROR, __LINE__, __FILE__, "Error: out_prefix is not a string.\n");
        return ERROR;
    } else {
        strncpy (cfg.out_prefix, jsonData["out_prefix"].get<std::string> ().c_str (),
                 sizeof (cfg.out_prefix));
    }

    // var_def_file
    if (!jsonData["var_def_file"].is_string ()) {
        check_err (ERROR, __LINE__, __FILE__, "Error: var_def_file is not a string.\n");
        return ERROR;
    } else {
        strncpy (cfg.var_def_file, jsonData["var_def_file"].get<std::string> ().c_str (),
                 sizeof (cfg.var_def_file));
    }

    // debug_level
    if (!jsonData["debug_level"].is_number_integer ()) {
        check_err (ERROR, __LINE__, __FILE__, "Error: debug_level is not an integer.\n");
        return ERROR;
    } else {
        cfg.debug_level = jsonData["debug_level"];
    }

    return NO_ERROR;
}  // end of read_json_file

int wrf_io_utils::get_config (const char *file_name, wrf_io_config &config) {
    int rank, np;
    int err;

    err = MPI_Comm_rank (config.io_comm, &rank);
    check_err (err, __LINE__, __FILE__, "Error getting rank.");
    err = MPI_Comm_size (config.io_comm, &np);
    check_err (err, __LINE__, __FILE__, "Error getting np.");

    if (rank == 0) {
        err = read_json_file (file_name, config);
        check_err (err, __LINE__, __FILE__, "Error reading json file.");
    }

    err = MPI_Bcast (&config, sizeof (wrf_io_config), MPI_BYTE, 0, MPI_COMM_WORLD);
    check_err (err, __LINE__, __FILE__, "Error broadcasting config.");

    config.rank    = rank;
    config.np      = np;
    config.io_comm = MPI_COMM_WORLD;
    config.info    = MPI_INFO_NULL;

    return NO_ERROR;
}  // end of get_config

int wrf_io_utils::get_variables_attrs (wrf_io_config &config) {
    char *file_contents = nullptr;
    size_t length;

    if (config.rank == 0) {
        std::ifstream file (config.var_def_file);
        if (!file.is_open ()) {
            check_err (ERROR, __LINE__, __FILE__, "Error opening file.");
            return ERROR;
        }

        file.seekg (0, std::ios::end);
        length = file.tellg ();
        file.seekg (0, std::ios::beg);
        file_contents = (char *)malloc (length);
        file.read (file_contents, length);
        file.close ();
    }

    MPI_Bcast (&length, 1, MPI_UNSIGNED_LONG, 0, config.io_comm);
    if (config.rank != 0) { file_contents = (char *)malloc (length); }
    MPI_Bcast (file_contents, length, MPI_CHAR, 0, config.io_comm);

    json jsonData = json::parse (file_contents);

    parse_json_data (jsonData);

    // set data
    for (size_t i = 0; i < variable::variables.size (); i++) { variable::variables[i].set_data (); }

    // Don't forget to free the memory when you're done
    if (file_contents) {
        free (file_contents);
        file_contents = nullptr;
    }

    return NO_ERROR;
}  // end of get_variables_attrs

void parse_json_data (json &jsonData) {
    size_t cnt = jsonData["dimensions"].size ();
    for (size_t i = 0; i < cnt; i++) {
        dimension dim;
        parse_dim (jsonData["dimensions"][i], dim);

        dimension::dim_name_to_dim[dim.name] = dim;
        if (dim.value == -3) {
            dimension::unlimited_dim_ptr = &dimension::dim_name_to_dim[dim.name];
        }
    }

    cnt = jsonData["global_attributes"].size ();
    // iterate jsonData["global_attributes"]
    for (size_t i = 0; i < cnt; i++) {
        attribute attr;
        parse_attr (jsonData["global_attributes"][i], attr);
        attribute::global_attrs.push_back (attr);
    }

    cnt = jsonData["variables"].size ();
    // iterate jsonData["variables"]
    for (size_t i = 0; i < cnt; i++) {
        variable var;
        parse_var (jsonData["variables"][i], var);
        variable::variables.push_back (var);
    }
}  // end of parse_json_data

void parse_dim (json &jsonData, dimension &dim) {
    dim.name  = jsonData["name"].get<std::string> ();
    dim.value = jsonData["value"].get<int> ();
    dim.dimid = -1;
}  // end of parse_dim

void parse_attr (json &jsonData, attribute &attr) {
    attr.name        = jsonData["attr_name"].get<std::string> ();
    attr.type        = jsonData["type"].get<std::string> ();
    attr.int_value   = 0;
    attr.float_value = 0.0;
    attr.str_value   = "";
    if (attr.type == "int") {
        attr.int_value = jsonData["value"].get<int> ();
    } else if (attr.type == "float") {
        attr.float_value = jsonData["value"].get<float> ();
    } else if (attr.type == "str") {
        attr.str_value = jsonData["value"].get<std::string> ();
    } else {
        check_err (ERROR, __LINE__, __FILE__, "Error parsing attributes.");
        return;
    }
}  // end of parse_attr

void parse_var (json &jsonData, variable &var) {
    var.name         = jsonData["name"].get<std::string> ();
    var.type         = jsonData["type"].get<std::string> ();
    var.num_attrs    = jsonData["num_attributes"].get<int> ();
    var.num_dims     = jsonData["num_dimensions"].get<int> ();
    var.is_partition = jsonData["is_partition"].get<bool> ();
    var.data         = NULL;

    if (var.num_attrs != jsonData["attributes"].size ()) {
        check_err (ERROR, __LINE__, __FILE__,
                   "Error: unmatched size when parsing variable attributes.");
        return;
    }
    for (size_t i = 0; i < var.num_attrs; i++) {
        attribute attr;
        parse_attr (jsonData["attributes"][i], attr);
        var.attrs.push_back (attr);
    }

    if (var.num_dims != jsonData["dimensions"].size ()) {
        check_err (ERROR, __LINE__, __FILE__,
                   "Error: unmatched size when parsing variable dimensions.");
        return;
    }

    for (size_t i = 0; i < var.num_dims; i++) {
        string dim_name = jsonData["dimensions"][i].get<string> ();
        var.dim_names.push_back (dim_name);
    }
}  // end of parse_var
