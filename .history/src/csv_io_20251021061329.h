#ifndef CSV_IO_H
#define CSV_IO_H

#include <stdio.h>

#include "data_errors.h"
#include "data_handler_enhanced.h" // for MAX_LINE_LENGTH/MAX_FIELDS/MAX_FILE_SIZE constants

// Reads a CSV line and splits into fields (allocated via strdup). Returns number of fields or 0 on EOF/error.
int read_csv_line(FILE *fp, char *fields[], int max_fields, char delimiter);

// Append a single line to a file with validation and error reporting.
int append_line(const char *filename, const char *line);

// Overwrite entire file content with backup/rollback behavior.
int overwrite_file(const char *filename, const char *content);

// Expose file access validation for reuse by higher-level modules
int validate_file_access(const char *filename, const char *mode);

#endif // CSV_IO_H
