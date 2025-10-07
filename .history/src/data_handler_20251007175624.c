#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

// Reads a line from file and splits by delimiter
int read_csv_line(FILE *fp, char *fields[], int max_fields, char delimiter)
{
	char line[MAX_LINE_LENGTH];
	if (!fgets(line, sizeof(line), fp))
		return 0;
	int i = 0;
	char *token = strtok(line, &delimiter);
	while (token && i < max_fields)
	{
		fields[i++] = strdup(token);
		token = strtok(NULL, &delimiter);
	}
	return i;
}

// Appends a line to a file
int append_line(const char *filename, const char *line)
{
	FILE *fp = fopen(filename, "a");
	if (!fp)
		return 0;
	fprintf(fp, "%s\n", line);
	fclose(fp);
	return 1;
}

// Overwrites file with new content
int overwrite_file(const char *filename, const char *content)
{
	FILE *fp = fopen(filename, "w");
	if (!fp)
		return 0;
	fprintf(fp, "%s", content);
	fclose(fp);
	return 1;
}

/* ==== Generic CRUD Functions for CSV-based Files ==== */

// Generic function to create a record in any file
int create_record(const char *filename, const char *record)
{
	return append_line(filename, record);
}

// Generic function to read a record by primary key(s)
// Returns allocated memory that must be freed by the caller
char *read_record(const char *filename, char *primary_keys[], int num_keys)
{
	FILE *fp = fopen(filename, "r");
	if (!fp)
		return NULL;

	char line[MAX_LINE_LENGTH];
	char *result = NULL;
	char *fields[20]; // Assuming max 20 fields per line
	int found = 0;

	// Read the header first to determine field positions
	if (!fgets(line, sizeof(line), fp))
	{
		fclose(fp);
		return NULL;
	}

	// Process each line
	while (fgets(line, sizeof(line), fp))
	{
		char line_copy[MAX_LINE_LENGTH];
		strcpy(line_copy, line);

		// Split the line into fields
		char *token = strtok(line, ",");
		int i = 0;
		while (token && i < 20)
		{
			// Trim whitespace
			while (*token == ' ')
				token++;
			char *end = token + strlen(token) - 1;
			while (end > token && (*end == ' ' || *end == '\n'))
				*end-- = '\0';

			fields[i++] = token;
			token = strtok(NULL, ",");
		}

		// Check if this line matches the primary key(s)
		found = 1;
		for (int j = 0; j < num_keys && found; j++)
		{
			// The format of primary_keys should be "field_index:value"
			char key_copy[MAX_LINE_LENGTH];
			strcpy(key_copy, primary_keys[j]);

			char *index_str = strtok(key_copy, ":");
			char *value = strtok(NULL, ":");

			if (!index_str || !value)
			{
				found = 0;
				break;
			}

			int index = atoi(index_str);
			if (index >= i || strcmp(fields[index], value) != 0)
			{
				found = 0;
			}
		}

		if (found)
		{
			result = strdup(line_copy);
			break;
		}
	}

	fclose(fp);
	return result;
}

// Generic function to update a record by primary key(s)
// field_to_update is in the format "field_index:new_value"
int update_record(const char *filename, char *primary_keys[], int num_keys, const char *field_to_update)
{
	FILE *fp = fopen(filename, "r");
	if (!fp)
		return 0;

	char content[MAX_LINE_LENGTH * 1000]; // Assuming file won't exceed this size
	content[0] = '\0';

	char line[MAX_LINE_LENGTH];
	int record_updated = 0;

	// First read the header and add to the content
	if (fgets(line, sizeof(line), fp))
	{
		strcat(content, line);
	}
	else
	{
		fclose(fp);
		return 0;
	}

	// Parse the field to update
	char field_copy[MAX_LINE_LENGTH];
	strcpy(field_copy, field_to_update);
	char *index_str = strtok(field_copy, ":");
	char *new_value = strtok(NULL, ":");
	if (!index_str || !new_value)
	{
		fclose(fp);
		return 0;
	}
	int update_index = atoi(index_str);

	// Process each line
	while (fgets(line, sizeof(line), fp))
	{
		char line_copy[MAX_LINE_LENGTH];
		strcpy(line_copy, line);

		// Check if this is the record to update
		char *fields[20];
		char *token = strtok(line, ",");
		int i = 0;
		while (token && i < 20)
		{
			// Trim whitespace
			while (*token == ' ')
				token++;
			char *end = token + strlen(token) - 1;
			while (end > token && (*end == ' ' || *end == '\n'))
				*end-- = '\0';

			fields[i++] = token;
			token = strtok(NULL, ",");
		}

		int found = 1;
		for (int j = 0; j < num_keys && found; j++)
		{
			// The format of primary_keys should be "field_index:value"
			char key_copy[MAX_LINE_LENGTH];
			strcpy(key_copy, primary_keys[j]);

			char *idx_str = strtok(key_copy, ":");
			char *value = strtok(NULL, ":");

			if (!idx_str || !value)
			{
				found = 0;
				break;
			}

			int index = atoi(idx_str);
			if (index >= i || strcmp(fields[index], value) != 0)
			{
				found = 0;
			}
		}

		if (found)
		{
			// This is the record to update
			if (update_index < i)
			{
				// Reconstruct the line with updated field
				char updated_line[MAX_LINE_LENGTH] = "";
				for (int j = 0; j < i; j++)
				{
					if (j > 0)
						strcat(updated_line, ", ");
					if (j == update_index)
					{
						strcat(updated_line, new_value);
					}
					else
					{
						strcat(updated_line, fields[j]);
					}
				}
				strcat(updated_line, "\n");
				strcat(content, updated_line);
				record_updated = 1;
			}
			else
			{
				// Invalid field index, keep original line
				strcat(content, line_copy);
			}
		}
		else
		{
			// Not the record to update, keep original line
			strcat(content, line_copy);
		}
	}

	fclose(fp);

	// Write updated content back to file if record was found and updated
	if (record_updated)
	{
		return overwrite_file(filename, content);
	}

	return 0;
}

// Generic function to delete a record by primary key(s)
int delete_record(const char *filename, char *primary_keys[], int num_keys)
{
	FILE *fp = fopen(filename, "r");
	if (!fp)
		return 0;

	char content[MAX_LINE_LENGTH * 1000]; // Assuming file won't exceed this size
	content[0] = '\0';

	char line[MAX_LINE_LENGTH];
	int record_deleted = 0;

	// First read the header and add to the content
	if (fgets(line, sizeof(line), fp))
	{
		strcat(content, line);
	}
	else
	{
		fclose(fp);
		return 0;
	}

	// Process each line
	while (fgets(line, sizeof(line), fp))
	{
		char line_copy[MAX_LINE_LENGTH];
		strcpy(line_copy, line);

		// Check if this is the record to delete
		char *fields[20];
		char *token = strtok(line, ",");
		int i = 0;
		while (token && i < 20)
		{
			// Trim whitespace
			while (*token == ' ')
				token++;
			char *end = token + strlen(token) - 1;
			while (end > token && (*end == ' ' || *end == '\n'))
				*end-- = '\0';

			fields[i++] = token;
			token = strtok(NULL, ",");
		}

		int found = 1;
		for (int j = 0; j < num_keys && found; j++)
		{
			// The format of primary_keys should be "field_index:value"
			char key_copy[MAX_LINE_LENGTH];
			strcpy(key_copy, primary_keys[j]);

			char *idx_str = strtok(key_copy, ":");
			char *value = strtok(NULL, ":");

			if (!idx_str || !value)
			{
				found = 0;
				break;
			}

			int index = atoi(idx_str);
			if (index >= i || strcmp(fields[index], value) != 0)
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
			// Not the record to delete, keep original line
			strcat(content, line_copy);
		}
	}

	fclose(fp);

	// Write updated content back to file if record was found and deleted
	if (record_deleted)
	{
		return overwrite_file(filename, content);
	}

	return 0;
}

/* ==== Entity-Specific CRUD Functions ==== */

/* === Candidate Functions === */
int create_candidate(const char *candidate_number, const char *name, const char *party_id, 
                     const char *district_id, const char *nic) {
    char record[MAX_LINE_LENGTH];
    snprintf(record, sizeof(record), "%s, %s, %s, %s, %s", 
             candidate_number, name, party_id, district_id, nic);
    return create_record("../data/approved_candidates.txt", record);
}

char *read_candidate(const char *candidate_number) {
    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", candidate_number);
    char *primary_keys[] = {key};
    return read_record("../data/approved_candidates.txt", primary_keys, 1);
}

int update_candidate(const char *candidate_number, int field_index, const char *new_value) {
    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", candidate_number);
    char *primary_keys[] = {key};
    
    char field_update[MAX_LINE_LENGTH];
    snprintf(field_update, sizeof(field_update), "%d:%s", field_index, new_value);
    
    return update_record("../data/approved_candidates.txt", primary_keys, 1, field_update);
}

int delete_candidate(const char *candidate_number) {
    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", candidate_number);
    char *primary_keys[] = {key};
    return delete_record("../data/approved_candidates.txt", primary_keys, 1);
}

/* === Voter Functions === */
int create_voter(const char *voting_number, const char *name, const char *nic, const char *district_id) {
    char record[MAX_LINE_LENGTH];
    snprintf(record, sizeof(record), "%s, %s, %s, %s", 
             voting_number, name, nic, district_id);
    return create_record("../data/approved_voters.txt", record);
}

char *read_voter(const char *voting_number) {
    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", voting_number);
    char *primary_keys[] = {key};
    return read_record("../data/approved_voters.txt", primary_keys, 1);
}

int update_voter(const char *voting_number, int field_index, const char *new_value) {
    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", voting_number);
    char *primary_keys[] = {key};
    
    char field_update[MAX_LINE_LENGTH];
    snprintf(field_update, sizeof(field_update), "%d:%s", field_index, new_value);
    
    return update_record("../data/approved_voters.txt", primary_keys, 1, field_update);
}

int delete_voter(const char *voting_number) {
    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", voting_number);
    char *primary_keys[] = {key};
    return delete_record("../data/approved_voters.txt", primary_keys, 1);
}

/* === Party Functions === */
int create_party(const char *party_id, const char *party_name) {
    char record[MAX_LINE_LENGTH];
    snprintf(record, sizeof(record), "%s, %s", party_id, party_name);
    return create_record("../data/party_name.txt", record);
}

char *read_party(const char *party_id) {
    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", party_id);
    char *primary_keys[] = {key};
    return read_record("../data/party_name.txt", primary_keys, 1);
}

int update_party(const char *party_id, const char *new_party_name) {
    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", party_id);
    char *primary_keys[] = {key};
    
    char field_update[MAX_LINE_LENGTH];
    snprintf(field_update, sizeof(field_update), "1:%s", new_party_name);
    
    return update_record("../data/party_name.txt", primary_keys, 1, field_update);
}

int delete_party(const char *party_id) {
    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", party_id);
    char *primary_keys[] = {key};
    return delete_record("../data/party_name.txt", primary_keys, 1);
}

/* === District Functions === */
int create_district(const char *district_id, const char *district_name) {
    char record[MAX_LINE_LENGTH];
    snprintf(record, sizeof(record), "%s, %s", district_id, district_name);
    return create_record("../data/district.txt", record);
}

char *read_district(const char *district_id) {
    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", district_id);
    char *primary_keys[] = {key};
    return read_record("../data/district.txt", primary_keys, 1);
}

int update_district(const char *district_id, const char *new_district_name) {
    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", district_id);
    char *primary_keys[] = {key};
    
    char field_update[MAX_LINE_LENGTH];
    snprintf(field_update, sizeof(field_update), "1:%s", new_district_name);
    
    return update_record("../data/district.txt", primary_keys, 1, field_update);
}

int delete_district(const char *district_id) {
    char key[MAX_LINE_LENGTH];
    snprintf(key, sizeof(key), "0:%s", district_id);
    char *primary_keys[] = {key};
    return delete_record("../data/district.txt", primary_keys, 1);
}

/* === Parliament Candidate Functions === */
int create_parliament_candidate(const char *candidate_number, const char *party_id) {
    char record[MAX_LINE_LENGTH];
    snprintf(record, sizeof(record), "%s, %s", candidate_number, party_id);
    return create_record("../data/parliament_candidates.txt", record);
}

char *read_parliament_candidate(const char *candidate_number, const char *party_id) {
    char key1[MAX_LINE_LENGTH], key2[MAX_LINE_LENGTH];
    snprintf(key1, sizeof(key1), "0:%s", candidate_number);
    snprintf(key2, sizeof(key2), "1:%s", party_id);
    char *primary_keys[] = {key1, key2};
    return read_record("../data/parliament_candidates.txt", primary_keys, 2);
}

int delete_parliament_candidate(const char *candidate_number, const char *party_id) {
    char key1[MAX_LINE_LENGTH], key2[MAX_LINE_LENGTH];
    snprintf(key1, sizeof(key1), "0:%s", candidate_number);
    snprintf(key2, sizeof(key2), "1:%s", party_id);
    char *primary_keys[] = {key1, key2};
    return delete_record("../data/parliament_candidates.txt", primary_keys, 2);
}

/* === Voter Count Functions === */
int create_voter_count(const char *voting_number, const char *candidate_number,
                       const char *party_id, const char *district_id, const char *count) {
    char record[MAX_LINE_LENGTH];
    snprintf(record, sizeof(record), "%s, %s, %s, %s, %s", 
             voting_number, candidate_number, party_id, district_id, count);
    return create_record("../data/voter_count.txt", record);
}

char *read_voter_count(const char *voting_number, const char *candidate_number,
                       const char *party_id, const char *district_id) {
    char key1[MAX_LINE_LENGTH], key2[MAX_LINE_LENGTH], key3[MAX_LINE_LENGTH], key4[MAX_LINE_LENGTH];
    snprintf(key1, sizeof(key1), "0:%s", voting_number);
    snprintf(key2, sizeof(key2), "1:%s", candidate_number);
    snprintf(key3, sizeof(key3), "2:%s", party_id);
    snprintf(key4, sizeof(key4), "3:%s", district_id);
    char *primary_keys[] = {key1, key2, key3, key4};
    return read_record("../data/voter_count.txt", primary_keys, 4);
}

int update_voter_count(const char *voting_number, const char *candidate_number,
                       const char *party_id, const char *district_id, const char *new_count) {
    char key1[MAX_LINE_LENGTH], key2[MAX_LINE_LENGTH], key3[MAX_LINE_LENGTH], key4[MAX_LINE_LENGTH];
    snprintf(key1, sizeof(key1), "0:%s", voting_number);
    snprintf(key2, sizeof(key2), "1:%s", candidate_number);
    snprintf(key3, sizeof(key3), "2:%s", party_id);
    snprintf(key4, sizeof(key4), "3:%s", district_id);
    char *primary_keys[] = {key1, key2, key3, key4};
    
    char field_update[MAX_LINE_LENGTH];
    snprintf(field_update, sizeof(field_update), "4:%s", new_count);
    
    return update_record("../data/voter_count.txt", primary_keys, 4, field_update);
}

int delete_voter_count(const char *voting_number, const char *candidate_number,
                       const char *party_id, const char *district_id) {
    char key1[MAX_LINE_LENGTH], key2[MAX_LINE_LENGTH], key3[MAX_LINE_LENGTH], key4[MAX_LINE_LENGTH];
    snprintf(key1, sizeof(key1), "0:%s", voting_number);
    snprintf(key2, sizeof(key2), "1:%s", candidate_number);
    snprintf(key3, sizeof(key3), "2:%s", party_id);
    snprintf(key4, sizeof(key4), "3:%s", district_id);
    char *primary_keys[] = {key1, key2, key3, key4};
    return delete_record("../data/voter_count.txt", primary_keys, 4);
}
