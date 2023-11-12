#include "wrf_io_utils.hpp"

using wrf_io_utils::attribute;
vector<attribute> attribute::global_attrs;

void attribute::print (int indent = 0) {
    string base_indent_str (indent, ' ');
    string indent_str (indent + 4, ' ');
    printf ("%sAttr name: %s\n", base_indent_str.c_str (), this->name.c_str ());
    printf ("%stype: %s\n", indent_str.c_str (), this->type.c_str ());
    if (this->type == "int") {
        printf ("%svalue: %d\n", indent_str.c_str (), this->int_value);
    } else if (this->type == "float") {
        printf ("%svalue: %f\n", indent_str.c_str (), this->float_value);
    } else if (this->type == "str") {
        printf ("%svalue: %s\n", indent_str.c_str (), this->str_value.c_str ());
    } else {
        check_err (ERROR, __LINE__, __FILE__, "Error printing attributes value.");
        return;
    }
}