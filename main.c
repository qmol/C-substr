#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "substr_wrapper.h"

int main() {

    // examples of DBMS syntax rules, not-validate against real DBMS
    substr_func_syntax dbms_substr_func_lib[5] = {
        {"substr",    1, 0},    // Oracle: allows negative start, 1-based index
        {"substring", 1, 0},    // SQL Server: does not allow negative start, 1-based index
        {"sbstr",     0, 0},    // PostgreSQL: does not allow negative start, 1-based index
        {"sstr",      0, 0},    // MySQL: does not allow negative start, 1-based index
        {"SUBSTR",    0, 0},    // SQLite: does not allow negative start, 0-based index
    };

    char *test_inputs [3] = {
        "SUBSTR(col_name, -1, 5)",
        "SUBSTR(\"an interesting test for substring function(), cool\", 1)",
        "SUBSTR(\"testtestcol_name\", 1, 0)",
    };

    char output_cmd_long[1024] = {0};
    char output_cmd_short[40] = {0};

    // test-1, negative start position
    FunctionStatus expected_results_1[5] = {
        RET_SUCCESS,               // Oracle: allows negative start
        RET_SUCCESS,                // SQL Server: does not allow negative start
        SUBSTR_STARTPOS_NEGATIVE,  // PostgreSQL: does not allow negative start
        SUBSTR_STARTPOS_NEGATIVE,  // MySQL: does not allow negative start
        SUBSTR_STARTPOS_NEGATIVE,  // SQLite: does not allow negative start
    };

    int itest = 0;
    size_t out_str_wrt = 0;
    for (int dbms_id = DBMS_ORACLE; dbms_id <= DBMS_SQLITE; dbms_id++) {
        FunctionStatus rc = translate_substr_func(test_inputs[itest], &dbms_substr_func_lib[dbms_id], 
                                output_cmd_long, sizeof(output_cmd_long), &out_str_wrt);
        if (rc != expected_results_1[dbms_id]) {
            printf("Test-1 DBMS ID %d FAILED: expected %d, got %d\n", dbms_id, expected_results_1[dbms_id], rc);
        } else {
            printf("Test-1 DBMS ID %d passed.\n", dbms_id);
        }

        if (rc == RET_SUCCESS) {
            printf("  Output: %s\n", output_cmd_long);
        }
    }

    // test-2, long string input
    FunctionStatus expected_results_2[5] = {
        RET_SUCCESS,               // Oracle: allows negative start
        RET_SUCCESS,                // SQL Server: does not allow negative start
        RET_SUCCESS,                // PostgreSQL: does not allow negative start
        RET_SUCCESS,                // MySQL: does not allow negative start
        RET_SUCCESS,                // SQLite: does not allow negative start
    };
    itest = 1;
    out_str_wrt = 0;
    for (int dbms_id = DBMS_ORACLE; dbms_id <= DBMS_SQLITE; dbms_id++) {
        FunctionStatus rc = translate_substr_func(test_inputs[itest], &dbms_substr_func_lib[dbms_id], 
                                output_cmd_long, sizeof(output_cmd_long), &out_str_wrt);
        if (rc != expected_results_2[dbms_id]) {
            printf("Test-2 DBMS ID %d FAILED: expected %d, got %d\n", dbms_id, expected_results_2[dbms_id], rc);
        } else {
            printf("Test-2 DBMS ID %d passed.\n", dbms_id);
        }
        if (rc == RET_SUCCESS) {
            printf("  Output: %s\n", output_cmd_long);
        }
    }

    
    // test-3, long string input, short output buffer
    FunctionStatus expected_results_3[5] = {
        TOO_SHORT_OUTPUT_BUFFER,    // Oracle: allows negative start
        TOO_SHORT_OUTPUT_BUFFER,    // SQL Server: does not allow negative start
        TOO_SHORT_OUTPUT_BUFFER,    // PostgreSQL: does not allow negative start
        TOO_SHORT_OUTPUT_BUFFER,    // MySQL: does not allow negative start
        TOO_SHORT_OUTPUT_BUFFER,    // SQLite: does not allow negative start
    };
    itest = 1;
    out_str_wrt = 0;
    for (int dbms_id = DBMS_ORACLE; dbms_id <= DBMS_SQLITE; dbms_id++) {
        FunctionStatus rc = translate_substr_func(test_inputs[itest], &dbms_substr_func_lib[dbms_id], 
                                output_cmd_short, sizeof(output_cmd_short), &out_str_wrt);
        if (rc != expected_results_3[dbms_id]) {
            printf("Test-3 DBMS ID %d FAILED: expected %d, got %d\n", dbms_id, expected_results_3[dbms_id], rc);
        } else {
            printf("Test-3 DBMS ID %d passed.\n", dbms_id);
        }
        if (rc == RET_SUCCESS) {
            printf("  Output: %s\n", output_cmd_long);
        }
    }

    // for (int i = 0; i < 3; i++) {
    //     printf("  Input: %s\n", test_inputs[i]);

    //     substr_func f_struct_in  = {0};

    //     FunctionStatus rc = parse_substr_call(test_inputs[i], &f_struct_in);
    //     if (rc != RET_SUCCESS) {
    //         printf("    Error parsing input: %d\n", rc);
    //         continue;
    //     }

    //     printf("Parsed substr function:\n");
    //     printf("    Function Name: %s\n", f_struct_in.func_name);
    //     printf("    Column Name : %s\n", f_struct_in.col_name);
    //     printf("    Start Pos   : %ld\n", f_struct_in.start_pos);
    //     printf("    Length      : %ld\n", f_struct_in.length);
        
    //     for (int dbms_id = DBMS_ORACLE; dbms_id <= DBMS_SQLITE; dbms_id++) {
    //         printf("Testing DBMS ID: %d\n", dbms_id);
    //         substr_func f_struct_out = {0};

    //         rc = gen_substr_func(&dbms_substr_func_lib[dbms_id], &f_struct_in, &f_struct_out);
    //         if (rc != RET_SUCCESS) {
    //             printf("    Error generating function: %d\n", rc);
    //             continue;
    //         }

    //         // printf("Parsed substr function:\n");
    //         // printf("    Function Name: %s\n", f_struct_out.func_name);
    //         // printf("    Column Name : %s\n", f_struct_out.col_name);
    //         // printf("    Start Pos   : %ld\n", f_struct_out.start_pos);
    //         // printf("    Length      : %ld\n", f_struct_out.length);

    //         char output_cmd[1024] = {0};
    //         size_t wrt_size = gen_substr_cmd(&f_struct_out, output_cmd, sizeof(output_cmd));
    //         if (wrt_size <= 0) {
    //             printf("    Error generating command: %d\n", wrt_size);
    //             continue;
    //         }

    //         printf("    Output: %s\n", output_cmd);

    //         if (f_struct_out.func_name) free(f_struct_out.func_name);
    //         if (f_struct_out.col_name) free(f_struct_out.col_name);
    //         // Free allocated memory
    //     }
    //     if (f_struct_in.func_name) free(f_struct_in.func_name);
    //     if (f_struct_in.col_name) free(f_struct_in.col_name);
    //     // }
    // }

    return 0;
}