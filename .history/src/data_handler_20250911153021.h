#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include <stdio.h>

#define MAX_LINE_LENGTH 256

int read_csv_line(FILE *fp, char *fields[], int max_fields, char delimiter);
int append_line(const char *filename, const char *line);
int overwrite_file(const char *filename, const char *content);

#endif // DATA_HANDLER_H
