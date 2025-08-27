#ifndef __func_status_h__
#define __func_status_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
If a function returns an integer code, 
    0 always means success!
*/
typedef enum  {
    /* If function merely returns an execution status to indicate
        everything goes correctly. */
    RET_SUCCESS = 0,

    /* For validation type of function, return TRUE (1) or FALSE (0) */
    TRUE_STATUS  = 1,
    FALSE_STATUS = 0,

    SUBSTR_STARTPOS_NEGATIVE = -1,


    FUNC_CALL_PARENS_MISMATCH = -10,
    FUNC_CALL_DQUOTE_MISMATCH  = -11,
    FUNC_CALL_WRONG_COL_NAME = -12,
    FUNC_CALL_WRONG_START_POS = -13,
    FUNC_CALL_WRONG_LENGTH    = -14,

    NULL_INPUT_POINTER    = -20,
    MEMORY_ALLOCATION_ERR = -21,
    TOO_SHORT_OUTPUT_BUFFER = -22,

} FunctionStatus;

#endif // __func_status_h__
