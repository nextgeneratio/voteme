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

/* ==== Generic CRUD Functions for CSV-based Files ==== */

// Generic function to create a record in any file
int create_record(const char *filename, const char *record) {
	return append_line(filename, record);
}

// Generic function to read a record by primary key(s)
// Returns allocated memory that must be freed by the caller
char* read_record(const char *filename, char *primary_keys[], int num_keys) {
	FILE *fp = fopen(filename, "r");
	if (!fp) return NULL;
	
	char line[MAX_LINE_LENGTH];
	char *result = NULL;
	char *fields[20]; // Assuming max 20 fields per line
	int found = 0;
	
	// Read the header first to determine field positions
	if (!fgets(line, sizeof(line), fp)) {
		fclose(fp);
		return NULL;
	}
	
	// Process each line
	while (fgets(line, sizeof(line), fp)) {
		char line_copy[MAX_LINE_LENGTH];
		strcpy(line_copy, line);
		
		// Split the line into fields
		char *token = strtok(line, ",");
		int i = 0;
		while (token && i < 20) {
			// Trim whitespace
			while (*token == ' ') token++;
			char *end = token + strlen(token) - 1;
			while (end > token && (*end == ' ' || *end == '\n')) *end-- = '\0';
			
			fields[i++] = token;
			token = strtok(NULL, ",");
		}
		
		// Check if this line matches the primary key(s)
		found = 1;
		for (int j = 0; j < num_keys && found; j++) {
			// The format of primary_keys should be "field_index:value"
			char key_copy[MAX_LINE_LENGTH];
			strcpy(key_copy, primary_keys[j]);
			
			char *index_str = strtok(key_copy, ":");
			char *value = strtok(NULL, ":");
			
			if (!index_str || !value) {
				found = 0;
				break;
			}
			
			int index = atoi(index_str);
			if (index >= i || strcmp(fields[index], value) != 0) {
				found = 0;
			}
		}
		
		if (found) {
			result = strdup(line_copy);
			break;
		}
	}
	
	fclose(fp);
	return result;
}

// Generic function to update a record by primary key(s)
// field_to_update is in the format "field_index:new_value"
int update_record(const char *filename, char *primary_keys[], int num_keys, const char *field_to_update) {
	FILE *fp = fopen(filename, "r");
	if (!fp) return 0;
	
	char content[MAX_LINE_LENGTH * 1000]; // Assuming file won't exceed this size
	content[0] = '\0';
	
	char line[MAX_LINE_LENGTH];
	int record_updated = 0;
	
	// First read the header and add to the content
	if (fgets(line, sizeof(line), fp)) {
		strcat(content, line);
	} else {
		fclose(fp);
		return 0;
	}
	
	// Parse the field to update
	char field_copy[MAX_LINE_LENGTH];
	strcpy(field_copy, field_to_update);
	char *index_str = strtok(field_copy, ":");
	char *new_value = strtok(NULL, ":");
	if (!index_str || !new_value) {
		fclose(fp);
		return 0;
	}
	int update_index = atoi(index_str);
	
	// Process each line
	while (fgets(line, sizeof(line), fp)) {
		char line_copy[MAX_LINE_LENGTH];
		strcpy(line_copy, line);
		
		// Check if this is the record to update
		char *fields[20];
		char *token = strtok(line, ",");
		int i = 0;
		while (token && i < 20) {
			// Trim whitespace
			while (*token == ' ') token++;
			char *end = token + strlen(token) - 1;
			while (end > token && (*end == ' ' || *end == '\n')) *end-- = '\0';
			
			fields[i++] = token;
			token = strtok(NULL, ",");
		}
		
		int found = 1;
		for (int j = 0; j < num_keys && found; j++) {
			// The format of primary_keys should be "field_index:value"
			char key_copy[MAX_LINE_LENGTH];
			strcpy(key_copy, primary_keys[j]);
			
			char *idx_str = strtok(key_copy, ":");
			char *value = strtok(NULL, ":");
			
			if (!idx_str || !value) {
				found = 0;
				break;
			}
			
			int index = atoi(idx_str);
			if (index >= i || strcmp(fields[index], value) != 0) {
				found = 0;
			}
		}
		
		if (found) {
			// This is the record to update
			if (update_index < i) {
				// Reconstruct the line with updated field
				char updated_line[MAX_LINE_LENGTH] = "";
				for (int j = 0; j < i; j++) {
					if (j > 0) strcat(updated_line, ", ");
					if (j == update_index) {
						strcat(updated_line, new_value);
					} else {
						strcat(updated_line, fields[j]);
					}
				}
				strcat(updated_line, "\n");
				strcat(content, updated_line);
				record_updated = 1;
			} else {
				// Invalid field index, keep original line
				strcat(content, line_copy);
			}
		} else {
			// Not the record to update, keep original line
			strcat(content, line_copy);
		}
	}
	
	fclose(fp);
	
	// Write updated content back to file if record was found and updated
	if (record_updated) {
		return overwrite_file(filename, content);
	}
	
	return 0;
}

// Generic function to delete a record by primary key(s)
int delete_record(const char *filename, char *primary_keys[], int num_keys) {
	FILE *fp = fopen(filename, "r");
	if (!fp) return 0;
	
	char content[MAX_LINE_LENGTH * 1000]; // Assuming file won't exceed this size
	content[0] = '\0';
	
	char line[MAX_LINE_LENGTH];
	int record_deleted = 0;
	
	// First read the header and add to the content
	if (fgets(line, sizeof(line), fp)) {
		strcat(content, line);
	} else {
		fclose(fp);
		return 0;
	}
	
	// Process each line
	while (fgets(line, sizeof(line), fp)) {
		char line_copy[MAX_LINE_LENGTH];
		strcpy(line_copy, line);
		
		// Check if this is the record to delete
		char *fields[20];
		char *token = strtok(line, ",");
		int i = 0;
		while (token && i < 20) {
			// Trim whitespace
			while (*token == ' ') token++;
			char *end = token + strlen(token) - 1;
			while (end > token && (*end == ' ' || *end == '\n')) *end-- = '\0';
			
			fields[i++] = token;
			token = strtok(NULL, ",");
		}
		
		int found = 1;
		for (int j = 0; j < num_keys && found; j++) {
			// The format of primary_keys should be "field_index:value"
			char key_copy[MAX_LINE_LENGTH];
			strcpy(key_copy, primary_keys[j]);
			
			char *idx_str = strtok(key_copy, ":");
			char *value = strtok(NULL, ":");
			
			if (!idx_str || !value) {
				found = 0;
				break;
			}
			
			int index = atoi(idx_str);
			if (index >= i || strcmp(fields[index], value) != 0) {
				found = 0;
			}
		}
		
		if (found) {
			// Skip this line (delete it)
			record_deleted = 1;
		} else {
			// Not the record to delete, keep original line
			strcat(content, line_copy);
		}
	}
	
	fclose(fp);
	
	// Write updated content back to file if record was found and deleted
	if (record_deleted) {
		return overwrite_file(filename, content);
	}
	
	return 0;
}
