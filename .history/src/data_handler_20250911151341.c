#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

// Reads a line from file and splits by delimiter
int read_csv_line(FILE *fp, char *fields[], int max_fields, char delimiter) {
	char line[MAX_LINE_LENGTH];
	if (!fgets(line, sizeof(line), fp)) return 0;
	int i = 0;
	char *token = strtok(line, &delimiter);
	while (token && i < max_fields) {
		fields[i++] = strdup(token);
		token = strtok(NULL, &delimiter);
	}
	return i;
}

// Appends a line to a file
int append_line(const char *filename, const char *line) {
	FILE *fp = fopen(filename, "a");
	if (!fp) return 0;
	fprintf(fp, "%s\n", line);
	fclose(fp);
	return 1;
}

// Overwrites file with new content
int overwrite_file(const char *filename, const char *content) {
	FILE *fp = fopen(filename, "w");
	if (!fp) return 0;
	fprintf(fp, "%s", content);
	fclose(fp);
	return 1;
}
