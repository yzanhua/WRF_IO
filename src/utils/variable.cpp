#include "wrf_io_utils.hpp"

using wrf_io_utils::variable;

int get_size_of_type (string type);

void variable::set_data () {
    if (this->is_partition) {
        if (pt_starts.empty () || pt_counts.empty ()) {
            check_err (ERROR, __LINE__, __FILE__, "Error setting data.");
            return;
        }

        this->data_count = 1;
        for (size_t i = 0; i < this->num_dims; i++) {
            int dim = dimension::dim_name_to_dim[this->dim_names[i]].value;
            // dim == -1 means E_WE, pt_counts[1]
            // dim == -2 means E_SN, pt_counts[0]
            if (dim < 0) { dim = pt_counts[dim + 2]; }
            this->data_count *= dim;
        }
        int type_size = get_size_of_type (this->type);
        if (type_size == ERROR) {
            check_err (ERROR, __LINE__, __FILE__, "Error setting data.");
            return;
        }
        this->data_size = type_size * this->data_count;
        this->data      = (char *)malloc (this->data_size);
    } else {
        this->data_count = 1;
        for (size_t i = 0; i < this->num_dims; i++) {
            this->data_count *= dimension::dim_name_to_dim[this->dim_names[i]].value;
        }
        int type_size = get_size_of_type (this->type);
        if (type_size == ERROR) {
            check_err (ERROR, __LINE__, __FILE__, "Error setting data.");
            return;
        }
        this->data_size = type_size * this->data_count;
        this->data      = (char *)malloc (this->data_size);
    }
}

void variable::calculate_parition (wrf_io_config &config) {
    pt_starts.clear ();
    pt_counts.clear ();

    int np_row_col[2] = {0, 0};
    int len, start;

    // np_row X np_col = np
    MPI_Dims_create (config.np, 2, np_row_col);
    int np_row = np_row_col[0];
    int np_col = np_row_col[1];
    int row_id = config.rank / np_col;
    int col_id = config.rank % np_col;

    // count of rows, sn
    len = config.e_sn / np_row;
    if (row_id < config.e_sn % np_row) len++;
    pt_counts.push_back (len);

    // count of cols, we
    len = config.e_we / np_col;
    if (col_id < config.e_we % np_col) len++;
    pt_counts.push_back (len);

    // start of row, sn
    len   = config.e_sn / np_row;
    start = (row_id * len);
    if (row_id < config.e_sn % np_row)
        start += row_id;
    else
        start += config.e_sn % np_row;
    pt_starts.push_back (start);

    // start of col, we
    len   = config.e_we / np_col;
    start = (col_id * len);
    if (col_id < config.e_we % np_col)
        start += col_id;
    else
        start += config.e_we % np_col;
    pt_starts.push_back (start);
}

void variable::print_parition () {
    printf ("partition: \n");
    string pt_indent_str (4, ' ');
    printf ("%sstarts:", pt_indent_str.c_str ());
    for (size_t i = 0; i < pt_starts.size (); i++) { printf (" %d", pt_starts[i]); }
    printf ("\n");
    printf ("%scounts:", pt_indent_str.c_str ());
    for (size_t i = 0; i < pt_counts.size (); i++) { printf (" %d", pt_counts[i]); }
    printf ("\n");
}

void variable::print (int indent = 0) {
    string base_indent_str (indent, ' ');
    string indent_str (indent + 4, ' ');
    string pt_indent_str (indent + 8, ' ');
    printf ("%sVar name: %s\n", base_indent_str.c_str (), this->name.c_str ());
    printf ("%stype: %s\n", indent_str.c_str (), this->type.c_str ());
    printf ("%snum_attrs: %d\n", indent_str.c_str (), this->num_attrs);
    printf ("%snum_dims: %d\n", indent_str.c_str (), this->num_dims);
    printf ("%sdims:", indent_str.c_str ());
    for (size_t i = 0; i < this->num_dims; i++) { printf (" %s", this->dim_names[i].c_str ()); }
    printf ("\n");

    printf ("%sattributes:\n", indent_str.c_str ());
    for (size_t i = 0; i < this->num_attrs; i++) { this->attrs[i].print (indent + 4); }
}

int get_size_of_type (string type) {
    if (type == "int") {
        return sizeof (int);
    } else if (type == "float") {
        return sizeof (float);
    } else if (type == "str" || type == "char") {
        return sizeof (char);
    } else {
        return wrf_io_utils::ERROR;
    }
}