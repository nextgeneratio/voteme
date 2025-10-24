// For strdup compatibility
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include "data_errors.h"
#include "csv_io.h"

// Safe strdup implementation if not available
#ifndef _GNU_SOURCE
char *strdup(const char *s)
{
    size_t len = strlen(s) + 1;
    char *result = malloc(len);
    if (result)
    {
        memcpy(result, s, len);
    }
    return result;
}
#endif

#define MAX_LINE_LENGTH 256
#define MAX_FIELDS 20
#define MAX_FILE_SIZE (MAX_LINE_LENGTH * 10000)

// error codes and get_last_error/set_error_message are provided by data_errors.h/.c

// Enhanced input validation
static int validate_string_input(const char *str, const char *param_name, size_t max_length)
{
    if (!str)
    {
        set_error_message("Error: %s cannot be NULL", param_name);
        return 0;
    }

    if (strlen(str) == 0)
    {
        set_error_message("Error: %s cannot be empty", param_name);
        return 0;
    }

    if (strlen(str) >= max_length)
    {
        set_error_message("Error: %s exceeds maximum length of %zu characters", param_name, max_length);
        return 0;
    }

    // Check for invalid characters that could break CSV format
    if (strchr(str, '\n') || strchr(str, '\r'))
    {
        set_error_message("Error: %s contains invalid newline characters", param_name);
        return 0;
    }

    return 1;
}

// file access validation is handled inside csv_io where needed

// safe_strdup handled inline when needed

// Enhanced CSV line reading with buffer overflow protection
// read_csv_line moved to csv_io.c

// Enhanced append line with comprehensive error checking
// append_line moved to csv_io.c

// Enhanced overwrite file with safety checks
// overwrite_file moved to csv_io.c

/* ==== Enhanced Generic CRUD Functions ==== */

// Enhanced create record with comprehensive validation
int create_record(const char *filename, const char *record)
{
    if (!validate_string_input(filename, "filename", MAX_LINE_LENGTH) ||
        !validate_string_input(record, "record", MAX_LINE_LENGTH))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    // Validate CSV format of the record
    int comma_count = 0;
    for (const char *p = record; *p; p++)
    {
        if (*p == ',')
            comma_count++;
    }

    if (comma_count == 0)
    {
        set_error_message("Error: Record must contain at least one comma separator");
        return DATA_ERROR_MALFORMED_DATA;
    }

    return append_line(filename, record);
}

// Helper: buffered append to content with size checks (avoids O(n^2) strcat)
static int append_to_buffer(char *content, size_t *len, const char *text)
{
    size_t add = strlen(text);
    if (*len + add >= MAX_FILE_SIZE)
    {
        set_error_message("Error: File size would exceed maximum limit");
        return 0;
    }
    memcpy(content + *len, text, add);
    *len += add;
    content[*len] = '\0';
    return 1;
}

// Enhanced read record with improved error handling
char *read_record(const char *filename, char *primary_keys[], int num_keys)
{
    if (!validate_string_input(filename, "filename", MAX_LINE_LENGTH) ||
        !primary_keys || num_keys <= 0)
    {
        set_error_message("Error: Invalid parameters for read_record");
        return NULL;
    }

    // Validate all primary keys
    for (int i = 0; i < num_keys; i++)
    {
        if (!validate_string_input(primary_keys[i], "primary_key", MAX_LINE_LENGTH))
        {
            return NULL;
        }

        // Validate primary key format
        if (!strchr(primary_keys[i], ':'))
        {
            set_error_message("Error: Primary key '%s' must be in format 'field_index:value'", primary_keys[i]);
            return NULL;
        }
    }

    if (!validate_file_access(filename, "r"))
    {
        return NULL;
    }

    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        set_error_message("Error: Cannot open file '%s' for reading: %s", filename, strerror(errno));
        return NULL;
    }

    char line[MAX_LINE_LENGTH];
    char *result = NULL;
    char *fields[MAX_FIELDS];
    int line_number = 0;

    // Read the header first
    if (!fgets(line, sizeof(line), fp))
    {
        set_error_message("Error: File '%s' is empty or cannot read header", filename);
        fclose(fp);
        return NULL;
    }
    line_number++;

    // Process each data line
    while (fgets(line, sizeof(line), fp))
    {
        line_number++;

        // Check for buffer overflow
        if (strlen(line) >= MAX_LINE_LENGTH - 1 && line[MAX_LINE_LENGTH - 2] != '\n')
        {
            set_error_message("Warning: Line %d exceeds maximum length, skipping", line_number);
            continue;
        }

        char line_copy[MAX_LINE_LENGTH];
        strncpy(line_copy, line, sizeof(line_copy) - 1);
        line_copy[sizeof(line_copy) - 1] = '\0';

        // Split the line into fields (allocated)
        int field_count = split_csv_fields(line, fields, MAX_FIELDS, ',');
        if (field_count <= 0)
            continue;

        // Check if this line matches all primary keys
        int found = 1;
        for (int j = 0; j < num_keys && found; j++)
        {
            char key_copy[MAX_LINE_LENGTH];
            strncpy(key_copy, primary_keys[j], sizeof(key_copy) - 1);
            key_copy[sizeof(key_copy) - 1] = '\0';

            char *index_str = strtok(key_copy, ":");
            char *value = strtok(NULL, ":");

            if (!index_str || !value)
            {
                set_error_message("Error: Invalid primary key format in '%s'", primary_keys[j]);
                fclose(fp);
                return NULL;
            }

            int index = atoi(index_str);
            if (index < 0 || index >= field_count)
            {
                set_error_message("Error: Field index %d out of range (0-%d) at line %d", index, field_count - 1, line_number);
                found = 0;
                break;
            }

            if (strcmp(fields[index], value) != 0)
            {
                found = 0;
            }
        }

        if (found)
        {
            // Duplicate the line safely
            char *dup = strdup(line_copy);
            if (!dup)
            {
                set_error_message("Error: Memory allocation failed for record duplication");
                fclose(fp);
                // free allocated fields
                for (int f = 0; f < field_count; f++)
                    free(fields[f]);
                return NULL;
            }
            result = dup;
            // free allocated fields before break
            for (int f = 0; f < field_count; f++)
                free(fields[f]);
            break;
        }
        // free allocated fields for this line
        for (int f = 0; f < field_count; f++)
            free(fields[f]);
    }

    fclose(fp);

    if (!result)
    {
        set_error_message("Record not found with specified primary key(s)");
    }

    return result;
}

// Enhanced update record with comprehensive validation and error handling
int update_record(const char *filename, char *primary_keys[], int num_keys, const char *field_to_update)
{
    // Input validation
    if (!validate_string_input(filename, "filename", MAX_LINE_LENGTH) ||
        !primary_keys || num_keys <= 0 ||
        !validate_string_input(field_to_update, "field_to_update", MAX_LINE_LENGTH))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    // Validate field_to_update format
    if (!strchr(field_to_update, ':'))
    {
        set_error_message("Error: field_to_update must be in format 'field_index:new_value'");
        return DATA_ERROR_INVALID_INPUT;
    }

    if (!validate_file_access(filename, "r"))
    {
        return DATA_ERROR_FILE_NOT_FOUND;
    }

    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        set_error_message("Error: Cannot open file '%s' for reading: %s", filename, strerror(errno));
        return DATA_ERROR_FILE_NOT_FOUND;
    }

    char *content = malloc(MAX_FILE_SIZE);
    if (!content)
    {
        set_error_message("Error: Memory allocation failed for file content buffer");
        fclose(fp);
        return DATA_ERROR_MEMORY_ALLOCATION;
    }
    content[0] = '\0';
    size_t content_len = 0;

    char line[MAX_LINE_LENGTH];
    int record_updated = 0;
    int line_number = 0;

    // Read and copy header
    if (!fgets(line, sizeof(line), fp))
    {
        set_error_message("Error: Cannot read header from file '%s'", filename);
        free(content);
        fclose(fp);
        return DATA_ERROR_MALFORMED_DATA;
    }

    if (!append_to_buffer(content, &content_len, line))
    {
        free(content);
        fclose(fp);
        return DATA_ERROR_BUFFER_OVERFLOW;
    }
    line_number++;

    // Parse field_to_update
    char field_copy[MAX_LINE_LENGTH];
    strncpy(field_copy, field_to_update, sizeof(field_copy) - 1);
    field_copy[sizeof(field_copy) - 1] = '\0';

    char *index_str = strtok(field_copy, ":");
    char *new_value = strtok(NULL, ":");
    if (!index_str || !new_value)
    {
        set_error_message("Error: Invalid field_to_update format");
        free(content);
        fclose(fp);
        return DATA_ERROR_INVALID_INPUT;
    }

    int update_index = atoi(index_str);
    if (update_index < 0)
    {
        set_error_message("Error: Field index must be non-negative");
        free(content);
        fclose(fp);
        return DATA_ERROR_INVALID_INPUT;
    }

    // Process each data line
    while (fgets(line, sizeof(line), fp))
    {
        line_number++;

        char line_copy[MAX_LINE_LENGTH];
        strncpy(line_copy, line, sizeof(line_copy) - 1);
        line_copy[sizeof(line_copy) - 1] = '\0';

        // Parse fields using shared helper
        char *fields[MAX_FIELDS];
        int field_count = split_csv_fields(line, fields, MAX_FIELDS, ',');
        if (field_count <= 0)
        {
            // keep original line if cannot parse
            if (!append_to_buffer(content, &content_len, line_copy))
            {
                free(content);
                fclose(fp);
                return DATA_ERROR_BUFFER_OVERFLOW;
            }
            continue;
        }

        // Check if this matches our primary keys
        int found = 1;
        for (int j = 0; j < num_keys && found; j++)
        {
            char key_copy[MAX_LINE_LENGTH];
            strncpy(key_copy, primary_keys[j], sizeof(key_copy) - 1);
            key_copy[sizeof(key_copy) - 1] = '\0';

            char *idx_str = strtok(key_copy, ":");
            char *value = strtok(NULL, ":");

            if (!idx_str || !value)
            {
                found = 0;
                break;
            }

            int index = atoi(idx_str);
            if (index >= field_count || strcmp(fields[index], value) != 0)
            {
                found = 0;
            }
        }

        if (found)
        {
            // Validate update index
            if (update_index >= field_count)
            {
                set_error_message("Error: Update field index %d out of range (0-%d)", update_index, field_count - 1);
                free(content);
                fclose(fp);
                for (int f = 0; f < field_count; f++)
                    free(fields[f]);
                return DATA_ERROR_INVALID_INPUT;
            }

            // Construct updated line
            char updated_line[MAX_LINE_LENGTH] = "";
            for (int i = 0; i < field_count; i++)
            {
                if (i > 0)
                    strcat(updated_line, ", ");
                if (i == update_index)
                {
                    strcat(updated_line, new_value);
                }
                else
                {
                    strcat(updated_line, fields[i]);
                }
            }
            strcat(updated_line, "\n");

            if (!append_to_buffer(content, &content_len, updated_line))
            {
                free(content);
                fclose(fp);
                for (int f = 0; f < field_count; f++)
                    free(fields[f]);
                return DATA_ERROR_BUFFER_OVERFLOW;
            }
            record_updated = 1;
        }
        else
        {
            if (!append_to_buffer(content, &content_len, line_copy))
            {
                free(content);
                fclose(fp);
                for (int f = 0; f < field_count; f++)
                    free(fields[f]);
                return DATA_ERROR_BUFFER_OVERFLOW;
            }
        }
        for (int f = 0; f < field_count; f++)
            free(fields[f]);
    }

    fclose(fp);

    if (!record_updated)
    {
        set_error_message("Record not found for update");
        free(content);
        return DATA_ERROR_RECORD_NOT_FOUND;
    }

    // Write updated content back
    int result = overwrite_file(filename, content);
    free(content);

    return result;
}

// Enhanced delete record with comprehensive validation
int delete_record(const char *filename, char *primary_keys[], int num_keys)
{
    // Input validation
    if (!validate_string_input(filename, "filename", MAX_LINE_LENGTH) ||
        !primary_keys || num_keys <= 0)
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    // Validate all primary keys
    for (int i = 0; i < num_keys; i++)
    {
        if (!validate_string_input(primary_keys[i], "primary_key", MAX_LINE_LENGTH))
        {
            return DATA_ERROR_INVALID_INPUT;
        }

        if (!strchr(primary_keys[i], ':'))
        {
            set_error_message("Error: Primary key '%s' must be in format 'field_index:value'", primary_keys[i]);
            return DATA_ERROR_INVALID_INPUT;
        }
    }

    if (!validate_file_access(filename, "r"))
    {
        return DATA_ERROR_FILE_NOT_FOUND;
    }

    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        set_error_message("Error: Cannot open file '%s' for reading: %s", filename, strerror(errno));
        return DATA_ERROR_FILE_NOT_FOUND;
    }

    char *content = malloc(MAX_FILE_SIZE);
    if (!content)
    {
        set_error_message("Error: Memory allocation failed for file content buffer");
        fclose(fp);
        return DATA_ERROR_MEMORY_ALLOCATION;
    }
    content[0] = '\0';
    size_t content_len = 0;

    char line[MAX_LINE_LENGTH];
    int record_deleted = 0;
    int line_number = 0;

    // Read and copy header
    if (!fgets(line, sizeof(line), fp))
    {
        set_error_message("Error: Cannot read header from file '%s'", filename);
        free(content);
        fclose(fp);
        return DATA_ERROR_MALFORMED_DATA;
    }

    strcat(content, line);
    line_number++;

    // Process each data line
    while (fgets(line, sizeof(line), fp))
    {
        line_number++;

        char line_copy[MAX_LINE_LENGTH];
        strncpy(line_copy, line, sizeof(line_copy) - 1);
        line_copy[sizeof(line_copy) - 1] = '\0';

        char *fields[MAX_FIELDS];
        int field_count = split_csv_fields(line, fields, MAX_FIELDS, ',');
        if (field_count <= 0)
        {
            if (!append_to_buffer(content, &content_len, line_copy))
            {
                free(content);
                fclose(fp);
                return DATA_ERROR_BUFFER_OVERFLOW;
            }
            continue;
        }

        // Check if this matches our primary keys
        int found = 1;
        for (int j = 0; j < num_keys && found; j++)
        {
            char key_copy[MAX_LINE_LENGTH];
            strncpy(key_copy, primary_keys[j], sizeof(key_copy) - 1);
            key_copy[sizeof(key_copy) - 1] = '\0';

            char *idx_str = strtok(key_copy, ":");
            char *value = strtok(NULL, ":");

            if (!idx_str || !value)
            {
                found = 0;
                break;
            }

            int index = atoi(idx_str);
            if (index >= field_count || strcmp(fields[index], value) != 0)
            {
                found = 0;
            }
        }

        if (found)
        {
            // Skip this line (delete it)
            record_deleted = 1;
        }
        else
        {
            // Keep this line
            if (!append_to_buffer(content, &content_len, line_copy))
            {
                free(content);
                fclose(fp);
                for (int f = 0; f < field_count; f++)
                    free(fields[f]);
                return DATA_ERROR_BUFFER_OVERFLOW;
            }
        }
        for (int f = 0; f < field_count; f++)
            free(fields[f]);
    }

    fclose(fp);

    if (!record_deleted)
    {
        set_error_message("Record not found for deletion");
        free(content);
        return DATA_ERROR_RECORD_NOT_FOUND;
    }

    // Write updated content back
    int result = overwrite_file(filename, content);
    free(content);

    return result;
}

/* ==== Enhanced Entity-Specific CRUD Functions ==== */

// Enhanced candidate functions with validation
int create_candidate(const char *candidate_number, const char *name, const char *party_id,
                     const char *district_id, const char *nic)
{
    if (!validate_string_input(candidate_number, "candidate_number", 50) ||
        !validate_string_input(name, "name", 100) ||
        !validate_string_input(party_id, "party_id", 20) ||
        !validate_string_input(district_id, "district_id", 20) ||
        !validate_string_input(nic, "nic", 20))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    char record[MAX_LINE_LENGTH];
    int result = snprintf(record, sizeof(record), "%s, %s, %s, %s, %s",
                          candidate_number, name, party_id, district_id, nic);

    if (result >= (int)sizeof(record))
    {
        set_error_message("Error: Candidate record exceeds maximum length");
        return DATA_ERROR_BUFFER_OVERFLOW;
    }

    return create_record("data/approved_candidates.txt", record);
}

char *read_candidate(const char *candidate_number)
{
    if (!validate_string_input(candidate_number, "candidate_number", 50))
    {
        return NULL;
    }

    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", candidate_number);
    char *primary_keys[] = {key};
    return read_record("data/approved_candidates.txt", primary_keys, 1);
}

int update_candidate(const char *candidate_number, int field_index, const char *new_value)
{
    if (!validate_string_input(candidate_number, "candidate_number", 50) ||
        !validate_string_input(new_value, "new_value", 100))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    if (field_index < 0 || field_index > 4)
    {
        set_error_message("Error: Field index must be between 0 and 4 for candidates");
        return DATA_ERROR_INVALID_INPUT;
    }

    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", candidate_number);
    char *primary_keys[] = {key};

    char field_update[MAX_LINE_LENGTH];
    snprintf(field_update, sizeof(field_update), "%d:%s", field_index, new_value);

    return update_record("data/approved_candidates.txt", primary_keys, 1, field_update);
}

int delete_candidate(const char *candidate_number)
{
    if (!validate_string_input(candidate_number, "candidate_number", 50))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", candidate_number);
    char *primary_keys[] = {key};
    return delete_record("data/approved_candidates.txt", primary_keys, 1);
}

// Enhanced voter functions with validation
int create_voter(const char *voting_number, const char *name, const char *nic, const char *district_id)
{
    if (!validate_string_input(voting_number, "voting_number", 50) ||
        !validate_string_input(name, "name", 100) ||
        !validate_string_input(nic, "nic", 20) ||
        !validate_string_input(district_id, "district_id", 20))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    char record[MAX_LINE_LENGTH];
    int result = snprintf(record, sizeof(record), "%s, %s, %s, %s",
                          voting_number, name, nic, district_id);

    if (result >= (int)sizeof(record))
    {
        set_error_message("Error: Voter record exceeds maximum length");
        return DATA_ERROR_BUFFER_OVERFLOW;
    }

    return create_record("data/approved_voters.txt", record);
}

char *read_voter(const char *voting_number)
{
    if (!validate_string_input(voting_number, "voting_number", 50))
    {
        return NULL;
    }

    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", voting_number);
    char *primary_keys[] = {key};
    return read_record("data/approved_voters.txt", primary_keys, 1);
}

int update_voter(const char *voting_number, int field_index, const char *new_value)
{
    if (!validate_string_input(voting_number, "voting_number", 50) ||
        !validate_string_input(new_value, "new_value", 100))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    if (field_index < 0 || field_index > 3)
    {
        set_error_message("Error: Field index must be between 0 and 3 for voters");
        return DATA_ERROR_INVALID_INPUT;
    }

    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", voting_number);
    char *primary_keys[] = {key};

    char field_update[MAX_LINE_LENGTH];
    snprintf(field_update, sizeof(field_update), "%d:%s", field_index, new_value);

    return update_record("data/approved_voters.txt", primary_keys, 1, field_update);
}

int delete_voter(const char *voting_number)
{
    if (!validate_string_input(voting_number, "voting_number", 50))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", voting_number);
    char *primary_keys[] = {key};
    return delete_record("data/approved_voters.txt", primary_keys, 1);
}

// Enhanced party functions
int create_party(const char *party_id, const char *party_name)
{
    if (!validate_string_input(party_id, "party_id", 20) ||
        !validate_string_input(party_name, "party_name", 100))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    char record[MAX_LINE_LENGTH];
    int result = snprintf(record, sizeof(record), "%s, %s", party_id, party_name);

    if (result >= (int)sizeof(record))
    {
        set_error_message("Error: Party record exceeds maximum length");
        return DATA_ERROR_BUFFER_OVERFLOW;
    }

    return create_record("data/party_name.txt", record);
}

char *read_party(const char *party_id)
{
    if (!validate_string_input(party_id, "party_id", 20))
    {
        return NULL;
    }

    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", party_id);
    char *primary_keys[] = {key};
    return read_record("data/party_name.txt", primary_keys, 1);
}

int update_party(const char *party_id, const char *new_party_name)
{
    if (!validate_string_input(party_id, "party_id", 20) ||
        !validate_string_input(new_party_name, "new_party_name", 100))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", party_id);
    char *primary_keys[] = {key};

    char field_update[MAX_LINE_LENGTH];
    snprintf(field_update, sizeof(field_update), "1:%s", new_party_name);

    return update_record("data/party_name.txt", primary_keys, 1, field_update);
}

int delete_party(const char *party_id)
{
    if (!validate_string_input(party_id, "party_id", 20))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", party_id);
    char *primary_keys[] = {key};
    return delete_record("data/party_name.txt", primary_keys, 1);
}

// Enhanced district functions
int create_district(const char *district_id, const char *district_name)
{
    if (!validate_string_input(district_id, "district_id", 20) ||
        !validate_string_input(district_name, "district_name", 100))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    char record[MAX_LINE_LENGTH];
    int result = snprintf(record, sizeof(record), "%s, %s", district_id, district_name);

    if (result >= (int)sizeof(record))
    {
        set_error_message("Error: District record exceeds maximum length");
        return DATA_ERROR_BUFFER_OVERFLOW;
    }

    return create_record("data/district.txt", record);
}

char *read_district(const char *district_id)
{
    if (!validate_string_input(district_id, "district_id", 20))
    {
        return NULL;
    }

    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", district_id);
    char *primary_keys[] = {key};
    return read_record("data/district.txt", primary_keys, 1);
}

int update_district(const char *district_id, const char *new_district_name)
{
    if (!validate_string_input(district_id, "district_id", 20) ||
        !validate_string_input(new_district_name, "new_district_name", 100))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", district_id);
    char *primary_keys[] = {key};

    char field_update[MAX_LINE_LENGTH];
    snprintf(field_update, sizeof(field_update), "1:%s", new_district_name);

    return update_record("data/district.txt", primary_keys, 1, field_update);
}

int delete_district(const char *district_id)
{
    if (!validate_string_input(district_id, "district_id", 20))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", district_id);
    char *primary_keys[] = {key};
    return delete_record("data/district.txt", primary_keys, 1);
}

// Enhanced parliament candidate functions
int create_parliament_candidate(const char *candidate_number, const char *party_id)
{
    if (!validate_string_input(candidate_number, "candidate_number", 50) ||
        !validate_string_input(party_id, "party_id", 20))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    char record[MAX_LINE_LENGTH];
    int result = snprintf(record, sizeof(record), "%s, %s", candidate_number, party_id);

    if (result >= (int)sizeof(record))
    {
        set_error_message("Error: Parliament candidate record exceeds maximum length");
        return DATA_ERROR_BUFFER_OVERFLOW;
    }

    return create_record("data/parliament_candidates.txt", record);
}

char *read_parliament_candidate(const char *candidate_number, const char *party_id)
{
    if (!validate_string_input(candidate_number, "candidate_number", 50) ||
        !validate_string_input(party_id, "party_id", 20))
    {
        return NULL;
    }

    char key1[MAX_LINE_LENGTH], key2[MAX_LINE_LENGTH];
    snprintf(key1, sizeof(key1), "0:%s", candidate_number);
    snprintf(key2, sizeof(key2), "1:%s", party_id);
    char *primary_keys[] = {key1, key2};
    return read_record("data/parliament_candidates.txt", primary_keys, 2);
}

int delete_parliament_candidate(const char *candidate_number, const char *party_id)
{
    if (!validate_string_input(candidate_number, "candidate_number", 50) ||
        !validate_string_input(party_id, "party_id", 20))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    char key1[MAX_LINE_LENGTH], key2[MAX_LINE_LENGTH];
    snprintf(key1, sizeof(key1), "0:%s", candidate_number);
    snprintf(key2, sizeof(key2), "1:%s", party_id);
    char *primary_keys[] = {key1, key2};
    return delete_record("data/parliament_candidates.txt", primary_keys, 2);
}

// Enhanced voter count functions
int create_voter_count(const char *voting_number, const char *candidate_number,
                       const char *party_id, const char *district_id, const char *count)
{
    if (!validate_string_input(voting_number, "voting_number", 50) ||
        !validate_string_input(candidate_number, "candidate_number", 50) ||
        !validate_string_input(party_id, "party_id", 20) ||
        !validate_string_input(district_id, "district_id", 20) ||
        !validate_string_input(count, "count", 20))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    // Validate that count is a number
    char *endptr;
    long count_value = strtol(count, &endptr, 10);
    if (*endptr != '\0' || count_value < 0)
    {
        set_error_message("Error: Count must be a non-negative integer");
        return DATA_ERROR_INVALID_INPUT;
    }

    char record[MAX_LINE_LENGTH];
    int result = snprintf(record, sizeof(record), "%s, %s, %s, %s, %s",
                          voting_number, candidate_number, party_id, district_id, count);

    if (result >= (int)sizeof(record))
    {
        set_error_message("Error: Voter count record exceeds maximum length");
        return DATA_ERROR_BUFFER_OVERFLOW;
    }

    return create_record("data/voter_count.txt", record);
}

char *read_voter_count(const char *voting_number, const char *candidate_number,
                       const char *party_id, const char *district_id)
{
    if (!validate_string_input(voting_number, "voting_number", 50) ||
        !validate_string_input(candidate_number, "candidate_number", 50) ||
        !validate_string_input(party_id, "party_id", 20) ||
        !validate_string_input(district_id, "district_id", 20))
    {
        return NULL;
    }

    char key1[MAX_LINE_LENGTH], key2[MAX_LINE_LENGTH], key3[MAX_LINE_LENGTH], key4[MAX_LINE_LENGTH];
    snprintf(key1, sizeof(key1), "0:%s", voting_number);
    snprintf(key2, sizeof(key2), "1:%s", candidate_number);
    snprintf(key3, sizeof(key3), "2:%s", party_id);
    snprintf(key4, sizeof(key4), "3:%s", district_id);
    char *primary_keys[] = {key1, key2, key3, key4};
    return read_record("data/voter_count.txt", primary_keys, 4);
}

int update_voter_count(const char *voting_number, const char *candidate_number,
                       const char *party_id, const char *district_id, const char *new_count)
{
    if (!validate_string_input(voting_number, "voting_number", 50) ||
        !validate_string_input(candidate_number, "candidate_number", 50) ||
        !validate_string_input(party_id, "party_id", 20) ||
        !validate_string_input(district_id, "district_id", 20) ||
        !validate_string_input(new_count, "new_count", 20))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    // Validate that new_count is a number
    char *endptr;
    long count_value = strtol(new_count, &endptr, 10);
    if (*endptr != '\0' || count_value < 0)
    {
        set_error_message("Error: New count must be a non-negative integer");
        return DATA_ERROR_INVALID_INPUT;
    }

    char key1[MAX_LINE_LENGTH], key2[MAX_LINE_LENGTH], key3[MAX_LINE_LENGTH], key4[MAX_LINE_LENGTH];
    snprintf(key1, sizeof(key1), "0:%s", voting_number);
    snprintf(key2, sizeof(key2), "1:%s", candidate_number);
    snprintf(key3, sizeof(key3), "2:%s", party_id);
    snprintf(key4, sizeof(key4), "3:%s", district_id);
    char *primary_keys[] = {key1, key2, key3, key4};

    char field_update[MAX_LINE_LENGTH];
    snprintf(field_update, sizeof(field_update), "4:%s", new_count);

    return update_record("data/voter_count.txt", primary_keys, 4, field_update);
}

int delete_voter_count(const char *voting_number, const char *candidate_number,
                       const char *party_id, const char *district_id)
{
    if (!validate_string_input(voting_number, "voting_number", 50) ||
        !validate_string_input(candidate_number, "candidate_number", 50) ||
        !validate_string_input(party_id, "party_id", 20) ||
        !validate_string_input(district_id, "district_id", 20))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    char key1[MAX_LINE_LENGTH], key2[MAX_LINE_LENGTH], key3[MAX_LINE_LENGTH], key4[MAX_LINE_LENGTH];
    snprintf(key1, sizeof(key1), "0:%s", voting_number);
    snprintf(key2, sizeof(key2), "1:%s", candidate_number);
    snprintf(key3, sizeof(key3), "2:%s", party_id);
    snprintf(key4, sizeof(key4), "3:%s", district_id);
    char *primary_keys[] = {key1, key2, key3, key4};
    return delete_record("data/voter_count.txt", primary_keys, 4);
}

/* ---- Temp Voted List Functions (data/temp-voted-list.txt) ---- */
// Schema: voting_number,candidate_number,party_id

static int ensure_temp_voted_header(void)
{
    const char *path = "data/temp-voted-list.txt";
    const char *header = "voting_number,candidate_number,party_id\n";
    FILE *fp = fopen(path, "r");
    if (!fp)
    {
        // Create with header
        return overwrite_file(path, header);
    }
    char first[MAX_LINE_LENGTH];
    first[0] = '\0';
    if (fgets(first, sizeof(first), fp) == NULL)
    {
        // Empty file or read error -> ensure header exists
        fclose(fp);
        return overwrite_file(path, header);
    }
    fclose(fp);
    // If file is empty or missing header, rewrite header (non-destructive would be nicer, but spec requires header present)
    if (first[0] == '\0')
    {
        return overwrite_file(path, header);
    }
    return DATA_SUCCESS;
}

char *read_temp_voted(const char *voting_number)
{
    if (!validate_string_input(voting_number, "voting_number", 50))
    {
        return NULL;
    }

    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", voting_number);
    char *primary_keys[] = {key};
    return read_record("data/temp-voted-list.txt", primary_keys, 1);
}

int update_temp_voted(const char *voting_number, const char *candidate_number, const char *party_id)
{
    if (!validate_string_input(voting_number, "voting_number", 50) ||
        !validate_string_input(candidate_number, "candidate_number", 50) ||
        !validate_string_input(party_id, "party_id", 20))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    // Ensure record exists
    char *existing = read_temp_voted(voting_number);
    if (!existing)
    {
        // Propagate record not found for clarity
        return DATA_ERROR_RECORD_NOT_FOUND;
    }
    free(existing);

    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", voting_number);
    char *primary_keys[] = {key};

    // Update candidate_number (field index 1)
    char field_update1[MAX_LINE_LENGTH];
    snprintf(field_update1, sizeof(field_update1), "1:%s", candidate_number);
    int rc = update_record("data/temp-voted-list.txt", primary_keys, 1, field_update1);
    if (rc != DATA_SUCCESS)
        return rc;

    // Update party_id (field index 2)
    char field_update2[MAX_LINE_LENGTH];
    snprintf(field_update2, sizeof(field_update2), "2:%s", party_id);
    rc = update_record("data/temp-voted-list.txt", primary_keys, 1, field_update2);
    return rc;
}

int clear_temp_voted(void)
{
    // Reset file to header only
    const char *header = "voting_number,candidate_number,party_id\n";
    return overwrite_file("data/temp-voted-list.txt", header);
}

int create_temp_voted(const char *voting_number, const char *candidate_number, const char *party_id)
{
    if (!validate_string_input(voting_number, "voting_number", 50) ||
        !validate_string_input(candidate_number, "candidate_number", 50) ||
        !validate_string_input(party_id, "party_id", 20))
    {
        return DATA_ERROR_INVALID_INPUT;
    }

    int rc = ensure_temp_voted_header();
    if (rc != DATA_SUCCESS)
        return rc;

    char record[MAX_LINE_LENGTH];
    int n = snprintf(record, sizeof(record), "%s, %s, %s", voting_number, candidate_number, party_id);
    if (n <= 0 || n >= (int)sizeof(record))
    {
        set_error_message("Error: temp voted record exceeds maximum length");
        return DATA_ERROR_BUFFER_OVERFLOW;
    }
    return append_line("data/temp-voted-list.txt", record);
}