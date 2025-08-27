#include "substr_wrapper.h"

/* 
    private strdup if it is not available in system */
#if !defined(_WIN32) && (!defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 200809L)
char * strdup( const char *str1 ) {
    char *new_str;
    size_t len = strlen(str1);
    new_str = malloc(len + 1); // Allocate enough memory for the string and null terminator
    if (new_str == NULL) {
        return NULL; // Handle allocation failure
    }
    memcpy(new_str, str1, len + 1); // Copy the string data
    return new_str;
}
#endif

/* 
    generate a translated substring function.
    return 0 if success, -1 if error */
FunctionStatus gen_substr_func(const substr_func_syntax *f_syntax, const substr_func *f_struct_in,
                    substr_func *f_struct_out) 
{
    if (!f_syntax || !f_struct_in || !f_struct_out) {
        return NULL_INPUT_POINTER; // Error: Null pointer
    }

    f_struct_out->func_name = strdup(f_syntax->func_name);
    if (!f_struct_out->func_name) {
        return MEMORY_ALLOCATION_ERR; // Error: Memory allocation failed
    }

    f_struct_out->col_name = strdup(f_struct_in->col_name);
    if (!f_struct_out->col_name) {
        free(f_struct_out->func_name);
        f_struct_out->func_name = NULL;
        return MEMORY_ALLOCATION_ERR; // Error: Memory allocation failed
    }
    
    // check if negative start position is allowed
    if (f_struct_in->start_pos < 0) {
        if (f_syntax->neg_start > 0) {
            // Negative start position is allowed
            f_struct_out->start_pos = f_struct_in->start_pos;
        } else {
            return SUBSTR_STARTPOS_NEGATIVE;
        }
    } else {
        f_struct_out->start_pos = f_struct_in->start_pos;
    }   

    // make corrections based on shift_start
    f_struct_out->start_pos += f_syntax->shift_start;

    // assign length
    f_struct_out->length = f_struct_in->length;

    return RET_SUCCESS; // Success
}

/* 
    write out a substring function command using given parameters
    user should make sure the output string size is long enough.
    return: <= 0: failure
            > 0: number of chars written (successful) 
*/
long int gen_substr_cmd(const substr_func *f_struct_in,  
                    char *substr_string, size_t str_len)
{
    if (!f_struct_in || !substr_string || str_len == 0) {
        return NULL_INPUT_POINTER; // Error: Null pointer or zero length
    }

    size_t cmd_len = 0;
    cmd_len += strlen(f_struct_in->func_name) + 1; // function name and '('
    cmd_len += strlen(f_struct_in->col_name) + 2; // input string and ',  ' 

    cmd_len += floor(log10(labs(f_struct_in->start_pos))) + 2;

    if (f_struct_in->length > 0) {
        cmd_len += floor(log10(labs(f_struct_in->length))) + 2; // length and ')'
    } else {
        cmd_len += 1; // just ')'
    }   

    if (cmd_len >= str_len) {
        return TOO_SHORT_OUTPUT_BUFFER; // Error: Output buffer too small
    }

    long int written = 0;

    if (f_struct_in->length > 0) {
        written = snprintf(substr_string, str_len, "%s(%s, %ld, %ld)", 
                           f_struct_in->func_name,  f_struct_in->col_name, 
                           f_struct_in->start_pos, 
                           f_struct_in->length);
    } else {
        written = snprintf(substr_string, str_len, "%s(%s, %ld)", 
                           f_struct_in->func_name,   f_struct_in->col_name, 
                           f_struct_in->start_pos);
    }

    if (written < 0 || (size_t)written >= str_len) {
        return TOO_SHORT_OUTPUT_BUFFER; // Error: Encoding error or output was truncated
    }

    return written; // Success: number of chars written
}

/*
    parse an input substring function call based on SAS-syntax-like rule.
    This function adjusts the start position and validates the input.
    Returns RET_SUCCESS if successful, or an error code otherwise.

    Example input strings: "SUBSTR(col, -3, 5)" or "SUBSTR(col, 1)"

    !!! Note the use of strtok function which will modify the input string !!!
*/
FunctionStatus parse_substr_call(const char *input_str_const, substr_func *f_struct_out)
{
    // vaidate input
    if (!input_str_const || !f_struct_out) {
        return NULL_INPUT_POINTER; // Error: Null pointer
    }
    FunctionStatus rc = RET_SUCCESS;

    char *input_str = strdup(input_str_const);
    if (!input_str) {
        rc = MEMORY_ALLOCATION_ERR; // Error: Memory allocation failed
        goto END;
    }

    // reset function paramenets 
    if (f_struct_out->func_name) {
        free(f_struct_out->func_name);
        f_struct_out->func_name = NULL;
    }
    if (f_struct_out->col_name) {
        free(f_struct_out->col_name);
        f_struct_out->col_name = NULL;
    }

    // matach parentheses
    char *left_paren    = strchr(input_str, '(');
    char *right_paren   = strrchr(input_str, ')');

    if (!left_paren || !right_paren || left_paren >= right_paren) {
        rc = FUNC_CALL_PARENS_MISMATCH; // Error: Invalid format
        goto END;
    }

    // match double quotes if input is a literal string
    char *left_dquote   = strchr(left_paren, '"');;
    char *right_dquote  = strrchr(left_paren, '"');

    // if one pointer is real, the other is real too
    if (left_dquote && left_dquote >= right_dquote) {
        rc = FUNC_CALL_DQUOTE_MISMATCH; // Error: Mismatched quotes
        goto END;
    }   

    const char func_tokens[] = " ,()";
    char *nxtoken = NULL;

    // char * col_name = NULL;
    char * comma_pos = NULL;
    // char * end_token = NULL;

    // col name is input here, col name does not contain space or comma or parens !!!
    if (!left_dquote) {
        comma_pos = strchr(left_paren + 1, ',');
        if (!comma_pos || comma_pos >= right_paren) {
            rc = FUNC_CALL_WRONG_COL_NAME; // Error: Invalid format
            goto END;
        }

        f_struct_out->col_name = (char *)malloc((comma_pos - left_paren   ) * sizeof(char));
        if (!f_struct_out->col_name) {
            rc = MEMORY_ALLOCATION_ERR; // Error: Memory allocation failed
            goto END;
        }
        strncpy(f_struct_out->col_name, left_paren + 1, comma_pos - left_paren - 1);
        f_struct_out->col_name[comma_pos - left_paren - 1] = '\0';
    } else {
        comma_pos = strchr(right_dquote + 1, ',');
        if (!comma_pos || comma_pos >= right_paren) {
            rc = FUNC_CALL_WRONG_COL_NAME; // Error: Invalid format
            goto END;
        }

        f_struct_out->col_name = (char *)malloc((right_dquote - left_dquote + 2) * sizeof(char));
        if (!f_struct_out->col_name) {
            rc = MEMORY_ALLOCATION_ERR; // Error: Memory allocation failed
            goto END;
        }
        strncpy(f_struct_out->col_name, left_dquote, right_dquote - left_dquote + 1);
        f_struct_out->col_name[right_dquote - left_dquote+1] = '\0';

    }

    // start position
    nxtoken = strtok(comma_pos, func_tokens);
    if (!nxtoken || strlen(nxtoken) == 0) {
        rc = FUNC_CALL_WRONG_START_POS; // Error: Invalid format
        goto END;
   }

    char* endptr;
    f_struct_out->start_pos = strtol(nxtoken, &endptr, 10);
    if (endptr == nxtoken || *endptr != '\0') {
        rc = FUNC_CALL_WRONG_START_POS; // Error: Not a valid integer
        goto END;
    }

    // length (optional)
    nxtoken = strtok(NULL, func_tokens);
    if (!nxtoken || strlen(nxtoken) == 0) {
        f_struct_out->length = 0; // length is optional
    } else {
        f_struct_out->length = strtol(nxtoken, &endptr, 10);
        if (endptr == nxtoken || *endptr != '\0') {
            rc = FUNC_CALL_WRONG_LENGTH; // Error: Not a valid integer
            goto END;
        }
    }

    if (f_struct_out->length < 0) {
        f_struct_out->length = 0; // treat negative length as 0
    }

    END:
    if (input_str) {
        free(input_str);
    }

    return rc; // Success
}

/*
    Given an input substr function call (such as SAS substr call) and a target DBMS syntax,
    translate the input substr function to the target DBMS syntax and write to output string.
    User should make sure the output string is long enough to hold the result.
    return: 0: success
           != 0: error code passed by children functions
*/
FunctionStatus translate_substr_func(const char *input_str, const substr_func_syntax *f_syntax,
                        char *out_substr_string, size_t out_str_len, size_t *out_str_wrt)
{
    if (!input_str || !f_syntax || !out_substr_string || out_str_len == 0) {
        return NULL_INPUT_POINTER; // Error: Null pointer or zero length
    }

    substr_func f_struct_in  = {0};

    FunctionStatus rc = parse_substr_call(input_str, &f_struct_in);
    if (rc != RET_SUCCESS) {
        printf("    Error parsing input: %d\n", rc);
        if (f_struct_in.func_name) free(f_struct_in.func_name);
        if (f_struct_in.col_name) free(f_struct_in.col_name);

        return rc;
    }

    substr_func f_struct_out = {0};

    rc = gen_substr_func(f_syntax, &f_struct_in, &f_struct_out);
    if (rc != RET_SUCCESS) {
        printf("    Error generating function: %d\n", rc);
        if (f_struct_in.func_name) free(f_struct_in.func_name);
        if (f_struct_in.col_name) free(f_struct_in.col_name);
        if (f_struct_out.func_name) free(f_struct_out.func_name);
        if (f_struct_out.col_name) free(f_struct_out.col_name);
        return rc;
    }

    long int wrt_size = gen_substr_cmd(&f_struct_out, out_substr_string, out_str_len);
    if (f_struct_in.func_name) free(f_struct_in.func_name);
    if (f_struct_in.col_name) free(f_struct_in.col_name);
    if (f_struct_out.func_name) free(f_struct_out.func_name);
    if (f_struct_out.col_name) free(f_struct_out.col_name);

    if (wrt_size <= 0) {
        printf("    Error generating command: %ld\n", wrt_size);
        return wrt_size;
    }

    out_str_wrt[0] = wrt_size;

    return RET_SUCCESS; // return number of chars written
}