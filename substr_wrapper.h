#ifndef __substr_wrapper_h__
#define __substr_wrapper_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// #include <stdarg.h>

#include "func_status.h"

#if defined(_WIN32)
#define strdup _strdup
#elif !defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 200809L
char * strdup( const char *str1 );
#endif

typedef enum {
    DBMS_ORACLE = 0,
    DBMS_SQLSERVER,
    DBMS_POSTGRESQL,
    DBMS_MYSQL,
    DBMS_SQLITE,

    DBMS_UNKNOWN ,
} DBMS_ID;

// elements for a substr function 
typedef struct substr_func_struc {
    char *func_name ;  /* function name */
    char *col_name  ;  /* column name or string literal */
    long int  start_pos  ;
    long int  length     ;
} substr_func;

// Define the syntax conversion between a DBMS and the input syntax
typedef struct substr_func_sybtax_struct {
    char *func_name ;   /* substr function name */
    int neg_start   ;   /* if negative start_pos is allowed, default is false (0) */  
    int shift_start ;   /*position shift caused by the difference btw the index scheme of input and target DBMS: 1-based or 0-based */
} substr_func_syntax;

// convert the elements of an input substr function to another one
FunctionStatus gen_substr_func(const substr_func_syntax *f_syntax, const substr_func *f_struct_in,
                    substr_func *f_struct_out);

// Generate command string using the elements of an input substr function 
long int gen_substr_cmd(const substr_func *f_struct_in, char *substr_string, size_t str_len);

// Parse input substr function call string into different elements 
FunctionStatus parse_substr_call(const char *input_str, substr_func *f_struct_out);


// Main wrapper function
FunctionStatus translate_substr_func(const char *input_str, const substr_func_syntax *f_syntax,
                        char *out_substr_string, size_t out_str_len, size_t *out_str_wrt);

// Given 
//    input_str: an input substr function call string, 
//    DBMS_id:   ID of input target DBMS, 
//    f_syntax:  library for syntax conversion
//    out_substr_string: holder for converted substr function call string
//    out_str_wrt:       size of the output string
FunctionStatus translate_substr_func_useID(const char *input_str, const int DBMS_id, const substr_func_syntax *f_syntax,
                        char *out_substr_string, size_t out_str_len, size_t *out_str_wrt);

#endif // __substr_wrapper_h__
