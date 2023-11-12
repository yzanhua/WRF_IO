#include "wrf_io_utils.hpp"

using wrf_io_utils::dimension;

unordered_map<string, dimension> dimension::dim_name_to_dim;
dimension *dimension::unlimited_dim_ptr = nullptr;

void dimension::print (int indent) {
    string base_indent_str (indent, ' ');
    string indent_str (indent + 4, ' ');
    printf ("%sDim name: %s\n", base_indent_str.c_str (), this->name.c_str ());
    printf ("%svalue: %d\n", indent_str.c_str (), this->value);
    printf ("%sdimid: %d\n", indent_str.c_str (), this->dimid);
}