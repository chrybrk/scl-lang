#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdbool.h>
#include "array.h"

extern char* scl_current_file;
extern char* scl_out_file;
extern char* scl_c_file;
extern bool  scl_flags[3];
extern array_T* scl_in_file;
extern array_T* scl_libs;
extern array_T* scl_libs_path;
extern array_T* scl_libs_include_path;
extern array_T* scl_error_list;
extern array_T* scl_python_readlines;

#endif
