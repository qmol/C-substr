# C-substr: Cross-DBMS Substring Function Parser and Translator

A C library that parses (SAS-like) substring function calls and translates them between different Database Management System (DBMS) syntaxes. This project provides a unified interface for handling substring operations across different DBMSs.

## Overview

Different DBMS systems have varying syntax rules for substring functions:
- **Function names**: `SUBSTR`, `SUBSTRING`, `SBSTR`, `SSTR`
- **Index base**: 0-based vs 1-based indexing
- **Negative positions**: Some systems allow negative start positions, others don't

This library abstracts these differences and provides translation capabilities between different DBMS substring syntaxes.

## Asumptions
- Input is a string containes a SAS-like SUBSTR function in the syntax of "SUBSTR(string, position <, length>)".
- A second input variable is the ID of target DBMS that the "SUBSTR" function call is to be translated.

## Features

- Parse substring function calls from input strings
- Validate syntax according to DBMS-specific rules
- Translate between different DBMS substring syntaxes
- Support for both column names and string literals
- Comprehensive error handling and status reporting
- Memory-safe string operations

## Project Structure

```
C-substr/
├── main.c              # Test program with examples
├── substr_wrapper.h    # Main header file
├── substr_wrapper.c    # Core implementation
├── func_status.h       # Status codes and error definitions
├── Makefile           # Build configuration
└── README.md          # This file
```

## Building the Project

### Prerequisites

- GCC compiler with C99 support
- Make utility
- Optional: Valgrind (for memory checking)
- Optional: cppcheck (for static analysis)

### Build Commands

```bash
# Build the project
make

# Build and run
make run

# Debug build
make debug

# Release build (optimized)
make release

# Clean build artifacts
make clean

# Rebuild from scratch
make rebuild
```

### Advanced Build Options

```bash
# Memory leak checking with Valgrind
make memcheck

# Static code analysis
make analyze

# Format code with clang-format
make format

# Show all available targets
make help
```

## Usage

### Basic Example

```c
#include "substr_wrapper.h"

int main() {
    // Define DBMS syntax rules
    substr_func_syntax oracle_syntax = {"substr", 1, 0};  // Allows negative, 1-based
    substr_func_syntax sqlite_syntax = {"SUBSTR", 0, 0};  // No negative, 0-based
    
    char input[] = "SUBSTR(column_name, -1, 5)";
    char output[256];
    size_t bytes_written;
    
    // Translate to Oracle syntax
    FunctionStatus result = translate_substr_func(
        input, 
        &oracle_syntax, 
        output, 
        sizeof(output), 
        &bytes_written
    );
    
    if (result == RET_SUCCESS) {
        printf("Oracle: %s\n", output);  // Output: substr(column_name, -1, 5)
    }
    
    return 0;
}
```

### Input Format

The parser accepts substring function calls in the following formats:

```sql
-- Column reference
SUBSTR(column_name, start_pos, length)
SUBSTR(column_name, start_pos)

-- String literal
SUBSTR("string literal", start_pos, length)
SUBSTR("string literal", start_pos)
```

### Error Handling

The library uses comprehensive error codes defined in `func_status.h`:

```c
typedef enum {
    RET_SUCCESS = 0,                    // Operation successful
    SUBSTR_STARTPOS_NEGATIVE = -1,      // Negative start position not allowed
    FUNC_CALL_PARENS_MISMATCH = -10,    // Mismatched parentheses
    FUNC_CALL_DQUOTE_MISMATCH = -11,    // Mismatched quotes
    FUNC_CALL_WRONG_COL_NAME = -12,     // Invalid column name
    FUNC_CALL_WRONG_START_POS = -13,    // Invalid start position
    FUNC_CALL_WRONG_LENGTH = -14,       // Invalid length parameter
    NULL_INPUT_POINTER = -20,           // Null pointer passed
    MEMORY_ALLOCATION_ERR = -21,        // Memory allocation failed
    TOO_SHORT_OUTPUT_BUFFER = -22,      // Output buffer too small
} FunctionStatus;
```

## API Reference

### Core Functions

#### `translate_substr_func()`
Main wrapper function that parses input and generates translated output.

```c
FunctionStatus translate_substr_func(
    const char *input_str,              // Input substring function call
    const substr_func_syntax *f_syntax, // Target DBMS syntax rules
    char *out_substr_string,            // Output buffer
    size_t out_str_len,                 // Output buffer size
    size_t *out_str_wrt                 // Bytes written (output)
);
```

#### `parse_substr_call()`
Parses a substring function call into structured components.

```c
FunctionStatus parse_substr_call(
    const char *input_str,      // Input string to parse
    substr_func *f_struct_out   // Parsed function structure (output)
);
```

#### `gen_substr_func()`
Generates a translated substring function based on DBMS syntax rules.

```c
FunctionStatus gen_substr_func(
    const substr_func_syntax *f_syntax,  // Target syntax rules
    const substr_func *f_struct_in,      // Input function structure
    substr_func *f_struct_out            // Output function structure
);
```

#### `gen_substr_cmd()`
Converts a function structure back to a string representation.

```c
long int gen_substr_cmd(
    const substr_func *f_struct_in,  // Input function structure
    char *substr_string,             // Output string buffer
    size_t str_len                   // Buffer size
);
```

### Data Structures

#### `substr_func`
Represents a parsed substring function:

```c
typedef struct {
    char *func_name;    // Function name (e.g., "substr", "substring")
    char *col_name;     // Column name or string literal
    long int start_pos; // Start position
    long int length;    // Length (0 if not specified)
} substr_func;
```

#### `substr_func_syntax`
Defines DBMS-specific syntax rules:

```c
typedef struct {
    char *func_name;    // Function name for this DBMS
    int neg_start;      // 1 if negative start positions allowed, 0 otherwise
    int shift_start;    // Index base adjustment (0 for 1-based, -1 for 0-based)
} substr_func_syntax;
```

## Testing

The project includes comprehensive tests in `main.c` that verify:

1. **Negative start position handling** - Tests DBMS-specific rules for negative indices
2. **Long string processing** - Validates handling of complex string literals
3. **Buffer overflow protection** - Ensures safe operation with limited output buffers

Run tests with:
```bash
make run
```

Expected output shows test results for each DBMS system with pass/fail status.

## Memory Management

The library handles dynamic memory allocation internally:
- All allocated strings are properly freed
- Functions clean up on error conditions
- No memory leaks in normal operation paths

For memory leak detection:
```bash
make memcheck
```

## Compatibility

- **C Standard**: C99 or later
- **Platforms**: Linux, macOS, Windows (with appropriate compiler)
- **Dependencies**: Standard C library, math library (`-lm`)

