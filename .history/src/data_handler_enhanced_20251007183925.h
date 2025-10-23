#ifndef DATA_HANDLER_ENHANCED_H
#define DATA_HANDLER_ENHANCED_H

#include <stdio.h>
#include <stdarg.h>

/* ==== Error Handling System ==== */

// Error codes for comprehensive error reporting
typedef enum {
    DATA_SUCCESS = 1,
    DATA_ERROR_FILE_NOT_FOUND = -1,
    DATA_ERROR_PERMISSION_DENIED = -2,
    DATA_ERROR_INVALID_INPUT = -3,
    DATA_ERROR_MEMORY_ALLOCATION = -4,
    DATA_ERROR_BUFFER_OVERFLOW = -5,
    DATA_ERROR_MALFORMED_DATA = -6,
    DATA_ERROR_RECORD_NOT_FOUND = -7,
    DATA_ERROR_DISK_FULL = -8
} data_error_t;

// Error reporting functions
const char* get_last_error(void);

/* ==== Constants and Limits ==== */
#define MAX_LINE_LENGTH 256
#define MAX_FIELDS 20
#define MAX_FILE_SIZE (MAX_LINE_LENGTH * 10000)

/* ==== File Operation Functions ==== */

/**
 * Enhanced CSV line reading with buffer overflow protection
 * @param fp File pointer to read from
 * @param fields Array to store parsed fields (will be dynamically allocated)
 * @param max_fields Maximum number of fields to parse
 * @param delimiter Character to use as field separator
 * @return Number of fields parsed, 0 on EOF or error
 * 
 * Safety Features:
 * - Buffer overflow protection
 * - Memory allocation tracking
 * - Automatic cleanup on errors
 */
int read_csv_line(FILE *fp, char *fields[], int max_fields, char delimiter);

/**
 * Enhanced file append with comprehensive error checking
 * @param filename Path to file to append to
 * @param line Content to append (without newline)
 * @return DATA_SUCCESS on success, negative error code on failure
 * 
 * Safety Features:
 * - Input validation
 * - Permission checking
 * - Disk space verification
 * - Atomic operations where possible
 */
int append_line(const char *filename, const char *line);

/**
 * Enhanced file overwrite with backup and recovery
 * @param filename Path to file to overwrite
 * @param content New content for the file
 * @return DATA_SUCCESS on success, negative error code on failure
 * 
 * Safety Features:
 * - Automatic backup creation
 * - Rollback on failure
 * - Size limit enforcement
 * - Permission validation
 */
int overwrite_file(const char *filename, const char *content);

/* ==== Enhanced Generic CRUD Functions ==== */

/**
 * Create a new record with comprehensive validation
 * @param filename CSV file to add record to
 * @param record Comma-separated record data
 * @return DATA_SUCCESS on success, negative error code on failure
 * 
 * Validations:
 * - Input parameter checking
 * - CSV format validation
 * - File access permissions
 * - Data integrity checks
 */
int create_record(const char *filename, const char *record);

/**
 * Read a record by primary key(s) with enhanced error handling
 * @param filename CSV file to read from
 * @param primary_keys Array of "field_index:value" strings
 * @param num_keys Number of primary keys
 * @return Dynamically allocated record string (must be freed), NULL on error
 * 
 * Features:
 * - Multi-field primary key support
 * - Input validation
 * - Memory leak prevention
 * - Detailed error reporting
 */
char *read_record(const char *filename, char *primary_keys[], int num_keys);

/**
 * Update a record with comprehensive validation and safety checks
 * @param filename CSV file to update
 * @param primary_keys Array of "field_index:value" strings to identify record
 * @param num_keys Number of primary keys
 * @param field_to_update "field_index:new_value" string
 * @return DATA_SUCCESS on success, negative error code on failure
 * 
 * Safety Features:
 * - Backup and rollback capability
 * - Field index validation
 * - Data integrity preservation
 * - Atomic update operations
 */
int update_record(const char *filename, char *primary_keys[], int num_keys, const char *field_to_update);

/**
 * Delete a record with comprehensive validation
 * @param filename CSV file to delete from
 * @param primary_keys Array of "field_index:value" strings to identify record
 * @param num_keys Number of primary keys
 * @return DATA_SUCCESS on success, negative error code on failure
 * 
 * Safety Features:
 * - Record existence verification
 * - Backup before deletion
 * - Data integrity preservation
 */
int delete_record(const char *filename, char *primary_keys[], int num_keys);

/* ==== Enhanced Entity-Specific CRUD Functions ==== */

/* ---- Candidate Management Functions ---- */

/**
 * Create a new candidate record with validation
 * @param candidate_number Unique candidate identifier
 * @param name Candidate's full name
 * @param party_id Political party identifier
 * @param district_id Electoral district identifier
 * @param nic National Identity Card number
 * @return DATA_SUCCESS on success, negative error code on failure
 * 
 * Validations:
 * - All parameters non-null and non-empty
 * - Length limits enforcement
 * - Format validation for special fields
 */
int create_candidate(const char *candidate_number, const char *name, const char *party_id, 
                     const char *district_id, const char *nic);

/**
 * Read candidate information by candidate number
 * @param candidate_number Unique candidate identifier
 * @return Dynamically allocated record string (must be freed), NULL on error
 */
char *read_candidate(const char *candidate_number);

/**
 * Update a specific field of a candidate record
 * @param candidate_number Unique candidate identifier
 * @param field_index Field to update (0=number, 1=name, 2=party, 3=district, 4=nic)
 * @param new_value New value for the field
 * @return DATA_SUCCESS on success, negative error code on failure
 * 
 * Field Index Reference:
 * 0 = candidate_number
 * 1 = name
 * 2 = party_id
 * 3 = district_id
 * 4 = nic
 */
int update_candidate(const char *candidate_number, int field_index, const char *new_value);

/**
 * Delete a candidate record
 * @param candidate_number Unique candidate identifier
 * @return DATA_SUCCESS on success, negative error code on failure
 */
int delete_candidate(const char *candidate_number);

/* ---- Voter Management Functions ---- */

/**
 * Create a new voter record with validation
 * @param voting_number Unique voter identifier
 * @param name Voter's full name
 * @param nic National Identity Card number
 * @param district_id Electoral district identifier
 * @return DATA_SUCCESS on success, negative error code on failure
 */
int create_voter(const char *voting_number, const char *name, const char *nic, const char *district_id);

/**
 * Read voter information by voting number
 * @param voting_number Unique voter identifier
 * @return Dynamically allocated record string (must be freed), NULL on error
 */
char *read_voter(const char *voting_number);

/**
 * Update a specific field of a voter record
 * @param voting_number Unique voter identifier
 * @param field_index Field to update (0=number, 1=name, 2=nic, 3=district)
 * @param new_value New value for the field
 * @return DATA_SUCCESS on success, negative error code on failure
 * 
 * Field Index Reference:
 * 0 = voting_number
 * 1 = name
 * 2 = nic
 * 3 = district_id
 */
int update_voter(const char *voting_number, int field_index, const char *new_value);

/**
 * Delete a voter record
 * @param voting_number Unique voter identifier
 * @return DATA_SUCCESS on success, negative error code on failure
 */
int delete_voter(const char *voting_number);

/* ---- Party Management Functions ---- */

/**
 * Create a new political party record
 * @param party_id Unique party identifier
 * @param party_name Full party name
 * @return DATA_SUCCESS on success, negative error code on failure
 */
int create_party(const char *party_id, const char *party_name);

/**
 * Read party information by party ID
 * @param party_id Unique party identifier
 * @return Dynamically allocated record string (must be freed), NULL on error
 */
char *read_party(const char *party_id);

/**
 * Update party name
 * @param party_id Unique party identifier
 * @param new_party_name New name for the party
 * @return DATA_SUCCESS on success, negative error code on failure
 */
int update_party(const char *party_id, const char *new_party_name);

/**
 * Delete a party record
 * @param party_id Unique party identifier
 * @return DATA_SUCCESS on success, negative error code on failure
 */
int delete_party(const char *party_id);

/* ---- District Management Functions ---- */

/**
 * Create a new electoral district record
 * @param district_id Unique district identifier
 * @param district_name Full district name
 * @return DATA_SUCCESS on success, negative error code on failure
 */
int create_district(const char *district_id, const char *district_name);

/**
 * Read district information by district ID
 * @param district_id Unique district identifier
 * @return Dynamically allocated record string (must be freed), NULL on error
 */
char *read_district(const char *district_id);

/**
 * Update district name
 * @param district_id Unique district identifier
 * @param new_district_name New name for the district
 * @return DATA_SUCCESS on success, negative error code on failure
 */
int update_district(const char *district_id, const char *new_district_name);

/**
 * Delete a district record
 * @param district_id Unique district identifier
 * @return DATA_SUCCESS on success, negative error code on failure
 */
int delete_district(const char *district_id);

/* ---- Parliament Candidate Functions ---- */

/**
 * Create a parliament candidate entry (candidate-party association)
 * @param candidate_number Unique candidate identifier
 * @param party_id Political party identifier
 * @return DATA_SUCCESS on success, negative error code on failure
 */
int create_parliament_candidate(const char *candidate_number, const char *party_id);

/**
 * Read parliament candidate by candidate number and party ID
 * @param candidate_number Unique candidate identifier
 * @param party_id Political party identifier
 * @return Dynamically allocated record string (must be freed), NULL on error
 */
char *read_parliament_candidate(const char *candidate_number, const char *party_id);

/**
 * Delete a parliament candidate entry
 * @param candidate_number Unique candidate identifier
 * @param party_id Political party identifier
 * @return DATA_SUCCESS on success, negative error code on failure
 */
int delete_parliament_candidate(const char *candidate_number, const char *party_id);

/* ---- Voter Count Management Functions ---- */

/**
 * Create a voter count record with validation
 * @param voting_number Unique voter identifier
 * @param candidate_number Unique candidate identifier
 * @param party_id Political party identifier
 * @param district_id Electoral district identifier
 * @param count Vote count (must be non-negative integer)
 * @return DATA_SUCCESS on success, negative error code on failure
 * 
 * Validations:
 * - Count must be a valid non-negative integer
 * - All identifiers must be valid
 */
int create_voter_count(const char *voting_number, const char *candidate_number,
                       const char *party_id, const char *district_id, const char *count);

/**
 * Read voter count by all key fields
 * @param voting_number Unique voter identifier
 * @param candidate_number Unique candidate identifier
 * @param party_id Political party identifier
 * @param district_id Electoral district identifier
 * @return Dynamically allocated record string (must be freed), NULL on error
 */
char *read_voter_count(const char *voting_number, const char *candidate_number,
                       const char *party_id, const char *district_id);

/**
 * Update vote count with validation
 * @param voting_number Unique voter identifier
 * @param candidate_number Unique candidate identifier
 * @param party_id Political party identifier
 * @param district_id Electoral district identifier
 * @param new_count New vote count (must be non-negative integer)
 * @return DATA_SUCCESS on success, negative error code on failure
 */
int update_voter_count(const char *voting_number, const char *candidate_number,
                       const char *party_id, const char *district_id, const char *new_count);

/**
 * Delete a voter count record
 * @param voting_number Unique voter identifier
 * @param candidate_number Unique candidate identifier
 * @param party_id Political party identifier
 * @param district_id Electoral district identifier
 * @return DATA_SUCCESS on success, negative error code on failure
 */
int delete_voter_count(const char *voting_number, const char *candidate_number,
                       const char *party_id, const char *district_id);

/* ==== Usage Guidelines ==== */

/*
SAFETY GUIDELINES FOR USING THESE FUNCTIONS:

1. ERROR HANDLING:
   - Always check return values
   - Use get_last_error() for detailed error messages
   - Handle all possible error codes appropriately

2. MEMORY MANAGEMENT:
   - Free all strings returned by read functions
   - Functions handle internal memory management automatically
   - No need to free error messages from get_last_error()

3. INPUT VALIDATION:
   - All functions validate inputs automatically
   - NULL parameters are detected and reported
   - String length limits are enforced
   - CSV format validation is performed

4. THREAD SAFETY:
   - Functions are NOT thread-safe
   - Use external synchronization for multi-threaded access
   - Error messages are stored in global state

5. FILE SYSTEM REQUIREMENTS:
   - Ensure sufficient disk space before operations
   - Verify read/write permissions
   - Handle network file system considerations

BREAKING SCENARIOS:

1. Will FAIL with DATA_ERROR_INVALID_INPUT:
   - NULL or empty string parameters
   - Strings exceeding maximum length limits
   - Invalid CSV format (missing separators)
   - Invalid primary key format (missing ':' separator)
   - Invalid field indices
   - Non-numeric values where numbers expected

2. Will FAIL with DATA_ERROR_FILE_NOT_FOUND:
   - File doesn't exist for read operations
   - Invalid file paths

3. Will FAIL with DATA_ERROR_PERMISSION_DENIED:
   - Insufficient file/directory permissions
   - Read-only file systems

4. Will FAIL with DATA_ERROR_MEMORY_ALLOCATION:
   - System out of memory
   - Memory allocation limits reached

5. Will FAIL with DATA_ERROR_BUFFER_OVERFLOW:
   - Records exceeding maximum line length
   - Files exceeding maximum size limits
   - Input strings too long

6. Will FAIL with DATA_ERROR_MALFORMED_DATA:
   - Corrupted CSV files
   - Invalid file format
   - Missing headers

7. Will FAIL with DATA_ERROR_RECORD_NOT_FOUND:
   - Update/delete operations on non-existent records
   - Invalid primary key combinations

8. Will FAIL with DATA_ERROR_DISK_FULL:
   - Insufficient disk space
   - I/O errors during write operations

BEST PRACTICES:

1. Always check return values:
   if (create_candidate("001", "John Doe", "P1", "D1", "123456789") != DATA_SUCCESS) {
       printf("Error: %s\n", get_last_error());
   }

2. Free allocated memory:
   char *result = read_candidate("001");
   if (result) {
       printf("Candidate: %s\n", result);
       free(result);
   }

3. Handle errors appropriately:
   int result = update_candidate("001", 1, "Jane Doe");
   switch (result) {
       case DATA_SUCCESS:
           printf("Update successful\n");
           break;
       case DATA_ERROR_RECORD_NOT_FOUND:
           printf("Candidate not found\n");
           break;
       default:
           printf("Error: %s\n", get_last_error());
   }
*/

#endif /* DATA_HANDLER_ENHANCED_H */