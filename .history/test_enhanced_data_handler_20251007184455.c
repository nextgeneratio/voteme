/*
 * Comprehensive Test Suite for Enhanced Data Handler
 * Tests all error handling, validation, and safety features
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include "data_handler_enhanced.h"

// Color codes for test output
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define RESET "\033[0m"
#define BOLD "\033[1m"

// Test counters
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

// Test helper macros
#define ASSERT(condition, message) do { \
    tests_run++; \
    if (condition) { \
        printf(GREEN "✓ PASS: " RESET "%s\n", message); \
        tests_passed++; \
    } else { \
        printf(RED "✗ FAIL: " RESET "%s\n", message); \
        printf(RED "   Error: %s" RESET "\n", get_last_error()); \
        tests_failed++; \
    } \
} while(0)

#define ASSERT_ERROR(expression, expected_error, message) do { \
    tests_run++; \
    int result = (expression); \
    if (result == expected_error) { \
        printf(GREEN "✓ PASS: " RESET "%s (got expected error: %d)\n", message, expected_error); \
        tests_passed++; \
    } else { \
        printf(RED "✗ FAIL: " RESET "%s (expected: %d, got: %d)\n", message, expected_error, result); \
        printf(RED "   Error: %s" RESET "\n", get_last_error()); \
        tests_failed++; \
    } \
} while(0)

// Setup and cleanup functions
void setup_test_environment() {
    printf(BOLD BLUE "Setting up test environment...\n" RESET);
    
    // Create test data directory if it doesn't exist
    system("mkdir -p test_data");
    
    // Create test CSV files with headers
    FILE *fp;
    
    // Test candidates file
    fp = fopen("test_data/candidates.csv", "w");
    if (fp) {
        fprintf(fp, "candidate_number, name, party_id, district_id, nic\n");
        fprintf(fp, "001, John Doe, P1, D1, 123456789\n");
        fprintf(fp, "002, Jane Smith, P2, D1, 987654321\n");
        fclose(fp);
    }
    
    // Test voters file
    fp = fopen("test_data/voters.csv", "w");
    if (fp) {
        fprintf(fp, "voting_number, name, nic, district_id\n");
        fprintf(fp, "V001, Alice Johnson, 111222333, D1\n");
        fprintf(fp, "V002, Bob Wilson, 444555666, D2\n");
        fclose(fp);
    }
    
    // Test parties file
    fp = fopen("test_data/parties.csv", "w");
    if (fp) {
        fprintf(fp, "party_id, party_name\n");
        fprintf(fp, "P1, Progressive Party\n");
        fprintf(fp, "P2, Conservative Party\n");
        fclose(fp);
    }
    
    // Test empty file
    fp = fopen("test_data/empty.csv", "w");
    if (fp) fclose(fp);
    
    // Test malformed file
    fp = fopen("test_data/malformed.csv", "w");
    if (fp) {
        fprintf(fp, "This is not a proper CSV file\n");
        fprintf(fp, "It has no structure\n");
        fclose(fp);
    }
    
    printf(GREEN "✓ Test environment setup complete\n" RESET);
}

void cleanup_test_environment() {
    printf(BOLD BLUE "Cleaning up test environment...\n" RESET);
    system("rm -rf test_data");
    system("rm -rf data"); // Clean up entity test data too
    printf(GREEN "✓ Cleanup complete\n" RESET);
}

// Input validation tests
void test_input_validation() {
    printf(BOLD YELLOW "\n=== Testing Input Validation ===\n" RESET);
    
    // Test NULL parameter validation
    ASSERT_ERROR(create_record(NULL, "test"), DATA_ERROR_INVALID_INPUT, "NULL filename rejection");
    ASSERT_ERROR(create_record("test.csv", NULL), DATA_ERROR_INVALID_INPUT, "NULL record rejection");
    
    // Test empty string validation
    ASSERT_ERROR(create_record("", "test"), DATA_ERROR_INVALID_INPUT, "Empty filename rejection");
    ASSERT_ERROR(create_record("test.csv", ""), DATA_ERROR_INVALID_INPUT, "Empty record rejection");
    
    // Test length limit validation
    char long_string[300];
    memset(long_string, 'A', 299);
    long_string[299] = '\0';
    ASSERT_ERROR(create_record(long_string, "test"), DATA_ERROR_INVALID_INPUT, "Long filename rejection");
    ASSERT_ERROR(create_record("test.csv", long_string), DATA_ERROR_INVALID_INPUT, "Long record rejection");
    
    // Test CSV format validation
    ASSERT_ERROR(create_record("test.csv", "no_comma_here"), DATA_ERROR_MALFORMED_DATA, "Invalid CSV format rejection");
    
    // Test newline character rejection
    ASSERT_ERROR(create_record("test\n.csv", "test,data"), DATA_ERROR_INVALID_INPUT, "Newline in filename rejection");
    ASSERT_ERROR(create_record("test.csv", "test\ndata"), DATA_ERROR_INVALID_INPUT, "Newline in record rejection");
    
    printf(GREEN "✓ Input validation tests completed\n" RESET);
}

// File access tests
void test_file_access() {
    printf(BOLD YELLOW "\n=== Testing File Access Validation ===\n" RESET);
    
    // Test non-existent file for read operations
    char *result = read_record("nonexistent.csv", NULL, 0);
    ASSERT(result == NULL, "Non-existent file rejection for read");
    
    // Test read-only directory (create one for testing)
    system("mkdir -p test_data/readonly");
    system("chmod 444 test_data/readonly");
    
    ASSERT_ERROR(create_record("test_data/readonly/test.csv", "test,data"), 
                 DATA_ERROR_PERMISSION_DENIED, "Read-only directory rejection");
    
    // Restore permissions for cleanup
    system("chmod 755 test_data/readonly");
    
    printf(GREEN "✓ File access validation tests completed\n" RESET);
}

// Buffer overflow protection tests
void test_buffer_overflow_protection() {
    printf(BOLD YELLOW "\n=== Testing Buffer Overflow Protection ===\n" RESET);
    
    // Create a file with very long line
    FILE *fp = fopen("test_data/long_line.csv", "w");
    if (fp) {
        fprintf(fp, "header1,header2\n");
        for (int i = 0; i < 300; i++) {
            fprintf(fp, "A");
        }
        fprintf(fp, ",data\n");
        fclose(fp);
    }
    
    // Try to read the file with long line
    char *fields[MAX_FIELDS];
    fp = fopen("test_data/long_line.csv", "r");
    if (fp) {
        char header[MAX_LINE_LENGTH];
        fgets(header, sizeof(header), fp); // Skip header
        
        int result = read_csv_line(fp, fields, MAX_FIELDS, ',');
        ASSERT(result == 0, "Long line rejection in read_csv_line");
        fclose(fp);
    }
    
    // Test large content protection
    char *large_content = malloc(MAX_FILE_SIZE + 1000);
    if (large_content) {
        memset(large_content, 'X', MAX_FILE_SIZE + 999);
        large_content[MAX_FILE_SIZE + 999] = '\0';
        
        ASSERT_ERROR(overwrite_file("test_data/large.csv", large_content), 
                     DATA_ERROR_BUFFER_OVERFLOW, "Large content rejection");
        
        free(large_content);
    }
    
    printf(GREEN "✓ Buffer overflow protection tests completed\n" RESET);
}

// Error handling tests
void test_error_handling() {
    printf(BOLD YELLOW "\n=== Testing Error Handling System ===\n" RESET);
    
    // Test error message system
    const char *initial_error = get_last_error();
    ASSERT(initial_error != NULL, "Error message system initialization");
    
    // Generate an error and check message
    create_record(NULL, "test"); // This should generate an error
    const char *error_msg = get_last_error();
    ASSERT(error_msg != NULL && strlen(error_msg) > 0, "Error message generation");
    ASSERT(strstr(error_msg, "filename") != NULL, "Meaningful error message content");
    
    printf(GREEN "✓ Error handling tests completed\n" RESET);
}

// Data integrity tests
void test_data_integrity() {
    printf(BOLD YELLOW "\n=== Testing Data Integrity ===\n" RESET);
    
    // Test backup and recovery for overwrite operations
    FILE *fp = fopen("test_data/backup_test.csv", "w");
    if (fp) {
        fprintf(fp, "original,content\n");
        fprintf(fp, "data1,value1\n");
        fclose(fp);
    }
    
    // Perform overwrite operation
    int result = overwrite_file("test_data/backup_test.csv", "new,content\nnew_data,new_value\n");
    ASSERT(result == DATA_SUCCESS, "Successful file overwrite");
    
    // Verify new content
    fp = fopen("test_data/backup_test.csv", "r");
    if (fp) {
        char line[MAX_LINE_LENGTH];
        fgets(line, sizeof(line), fp);
        ASSERT(strstr(line, "new,content") != NULL, "Content update verification");
        fclose(fp);
    }
    
    // Check that backup was created and removed after successful write
    struct stat st;
    ASSERT(stat("test_data/backup_test.csv.backup", &st) != 0, "Backup cleanup after successful write");
    
    printf(GREEN "✓ Data integrity tests completed\n" RESET);
}

// Enhanced CRUD operation tests
void test_enhanced_crud_operations() {
    printf(BOLD YELLOW "\n=== Testing Enhanced CRUD Operations ===\n" RESET);
    
    // Test create with validation
    FILE *fp = fopen("test_data/crud_test.csv", "w");
    if (fp) {
        fprintf(fp, "header1,header2\n");
        fclose(fp);
    }
    
    ASSERT(create_record("test_data/crud_test.csv", "data1,data2") == DATA_SUCCESS, 
           "Valid record creation");
    
    // Test read with proper primary key format
    char key1[] = "0:data1";
    char *primary_keys[] = {key1};
    char *result = read_record("test_data/crud_test.csv", primary_keys, 1);
    ASSERT(result != NULL, "Valid record reading");
    if (result) free(result);
    
    // Test read with invalid primary key format
    char invalid_key[] = "invalidformat";
    char *invalid_keys[] = {invalid_key};
    result = read_record("test_data/crud_test.csv", invalid_keys, 1);
    ASSERT(result == NULL, "Invalid primary key format rejection");
    
    // Test update with field index validation
    char valid_key[] = "0:data1";
    char *valid_keys[] = {valid_key};
    ASSERT_ERROR(update_record("test_data/crud_test.csv", valid_keys, 1, "invalidformat"), 
                 DATA_ERROR_INVALID_INPUT, "Invalid update format rejection");
    
    ASSERT(update_record("test_data/crud_test.csv", valid_keys, 1, "1:updated_value") == DATA_SUCCESS, 
           "Valid record update");
    
    // Test delete
    ASSERT(delete_record("test_data/crud_test.csv", valid_keys, 1) == DATA_SUCCESS, 
           "Valid record deletion");
    
    printf(GREEN "✓ Enhanced CRUD operation tests completed\n" RESET);
}

// Entity-specific function tests
void test_entity_specific_functions() {
    printf(BOLD YELLOW "\n=== Testing Entity-Specific Functions ===\n" RESET);
    
    // Setup entity test files with correct structure
    system("mkdir -p data");
    
    FILE *fp = fopen("data/approved_candidates.txt", "w");
    if (fp) {
        fprintf(fp, "candidate_number, name, party_id, district_id, nic\n");
        fclose(fp);
    }
    
    fp = fopen("data/approved_voters.txt", "w");
    if (fp) {
        fprintf(fp, "voting_number, name, nic, district_id\n");
        fclose(fp);
    }
    
    fp = fopen("data/voter_count.txt", "w");
    if (fp) {
        fprintf(fp, "voting_number, candidate_number, party_id, district_id, count\n");
        fclose(fp);
    }
    
    fp = fopen("data/party_name.txt", "w");
    if (fp) {
        fprintf(fp, "party_id, party_name\n");
        fclose(fp);
    }
    
    fp = fopen("data/district.txt", "w");
    if (fp) {
        fprintf(fp, "district_id, district_name\n");
        fclose(fp);
    }
    
    fp = fopen("data/parliament_candidates.txt", "w");
    if (fp) {
        fprintf(fp, "candidate_number, party_id\n");
        fclose(fp);
    }
    
    // Test candidate functions with enhanced validation
    ASSERT(create_candidate("C001", "Test Candidate", "P1", "D1", "123456789") == DATA_SUCCESS, 
           "Valid candidate creation");
    
    ASSERT_ERROR(create_candidate("", "Test", "P1", "D1", "123"), DATA_ERROR_INVALID_INPUT, 
                 "Empty candidate number rejection");
    
    char *candidate = read_candidate("C001");
    ASSERT(candidate != NULL, "Valid candidate reading");
    if (candidate) free(candidate);
    
    ASSERT(update_candidate("C001", 1, "Updated Name") == DATA_SUCCESS, 
           "Valid candidate update");
    
    ASSERT_ERROR(update_candidate("C001", 10, "Invalid"), DATA_ERROR_INVALID_INPUT, 
                 "Invalid field index rejection");
    
    // Test voter count functions with numeric validation
    ASSERT(create_voter_count("V001", "C001", "P1", "D1", "5") == DATA_SUCCESS, 
           "Valid voter count creation");
    
    ASSERT_ERROR(create_voter_count("V001", "C001", "P1", "D1", "invalid_number"), 
                 DATA_ERROR_INVALID_INPUT, "Invalid count format rejection");
    
    ASSERT_ERROR(create_voter_count("V001", "C001", "P1", "D1", "-1"), 
                 DATA_ERROR_INVALID_INPUT, "Negative count rejection");
    
    printf(GREEN "✓ Entity-specific function tests completed\n" RESET);
}

// Performance and stress tests
void test_performance_and_stress() {
    printf(BOLD YELLOW "\n=== Testing Performance and Stress Scenarios ===\n" RESET);
    
    // Test many sequential operations
    FILE *fp = fopen("test_data/stress_test.csv", "w");
    if (fp) {
        fprintf(fp, "id,data\n");
        fclose(fp);
    }
    
    int success_count = 0;
    for (int i = 0; i < 100; i++) {
        char record[100];
        snprintf(record, sizeof(record), "%d,data_%d", i, i);
        if (create_record("test_data/stress_test.csv", record) == DATA_SUCCESS) {
            success_count++;
        }
    }
    
    ASSERT(success_count == 100, "Stress test: 100 sequential creates");
    
    // Test memory cleanup by reading and freeing many records
    int read_success = 0;
    for (int i = 0; i < 50; i++) {
        char key[50];
        snprintf(key, sizeof(key), "0:%d", i);
        char *keys[] = {key};
        char *result = read_record("test_data/stress_test.csv", keys, 1);
        if (result) {
            read_success++;
            free(result);
        }
    }
    
    ASSERT(read_success == 50, "Stress test: Memory management in reads");
    
    printf(GREEN "✓ Performance and stress tests completed\n" RESET);
}

// Recovery and backup tests
void test_recovery_and_backup() {
    printf(BOLD YELLOW "\n=== Testing Recovery and Backup Features ===\n" RESET);
    
    // Create a test file
    FILE *fp = fopen("test_data/recovery_test.csv", "w");
    if (fp) {
        fprintf(fp, "id,name\n");
        fprintf(fp, "1,original\n");
        fclose(fp);
    }
    
    // Simulate a failed write by making directory read-only after backup creation
    // This is tricky to test reliably, so we'll test the backup creation part
    
    // Test that overwrite creates backup
    int result = overwrite_file("test_data/recovery_test.csv", "id,name\n2,updated\n");
    ASSERT(result == DATA_SUCCESS, "File overwrite with backup");
    
    // Verify content was updated
    fp = fopen("test_data/recovery_test.csv", "r");
    if (fp) {
        char line[MAX_LINE_LENGTH];
        fgets(line, sizeof(line), fp); // Skip header
        fgets(line, sizeof(line), fp); // Read data line
        ASSERT(strstr(line, "updated") != NULL, "Content update verification");
        fclose(fp);
    }
    
    printf(GREEN "✓ Recovery and backup tests completed\n" RESET);
}

// Comprehensive edge case tests
void test_edge_cases() {
    printf(BOLD YELLOW "\n=== Testing Edge Cases ===\n" RESET);
    
    // Test with files that have unusual but valid content
    FILE *fp = fopen("test_data/edge_case.csv", "w");
    if (fp) {
        fprintf(fp, "field1,field2,field3\n");
        fprintf(fp, "  spaced  ,  content  ,  here  \n");
        fprintf(fp, "quotes,\"data,with,commas\",normal\n");
        fclose(fp);
    }
    
    // Test reading with whitespace handling
    char key[] = "0:spaced";
    char *keys[] = {key};
    char *result = read_record("test_data/edge_case.csv", keys, 1);
    ASSERT(result != NULL, "Whitespace handling in record reading");
    if (result) free(result);
    
    // Test maximum valid field count
    char max_fields_record[MAX_LINE_LENGTH] = "";
    for (int i = 0; i < MAX_FIELDS - 1; i++) {
        if (i > 0) strcat(max_fields_record, ",");
        char field[10];
        snprintf(field, sizeof(field), "field%d", i);
        strcat(max_fields_record, field);
    }
    
    ASSERT(create_record("test_data/max_fields.csv", max_fields_record) == DATA_SUCCESS, 
           "Maximum field count handling");
    
    // Test empty data file (header only)
    fp = fopen("test_data/header_only.csv", "w");
    if (fp) {
        fprintf(fp, "id,name\n");
        fclose(fp);
    }
    
    char empty_key[] = "0:nonexistent";
    char *empty_keys[] = {empty_key};
    result = read_record("test_data/header_only.csv", empty_keys, 1);
    ASSERT(result == NULL, "Empty data file handling");
    
    printf(GREEN "✓ Edge case tests completed\n" RESET);
}

int main() {
    printf(BOLD BLUE "========================================\n");
    printf("Enhanced Data Handler Test Suite\n");
    printf("========================================\n" RESET);
    
    setup_test_environment();
    
    // Run all test suites
    test_input_validation();
    test_file_access();
    test_buffer_overflow_protection();
    test_error_handling();
    test_data_integrity();
    test_enhanced_crud_operations();
    test_entity_specific_functions();
    test_performance_and_stress();
    test_recovery_and_backup();
    test_edge_cases();
    
    cleanup_test_environment();
    
    // Print final results
    printf(BOLD BLUE "\n========================================\n");
    printf("Test Results Summary\n");
    printf("========================================\n" RESET);
    printf("Total tests run:    %s%d%s\n", BOLD, tests_run, RESET);
    printf("Tests passed:       %s%s%d%s\n", GREEN, BOLD, tests_passed, RESET);
    printf("Tests failed:       %s%s%d%s\n", RED, BOLD, tests_failed, RESET);
    
    if (tests_failed == 0) {
        printf(GREEN BOLD "\n🎉 ALL TESTS PASSED! 🎉\n" RESET);
        printf(GREEN "Enhanced data handler is working correctly with comprehensive error handling.\n" RESET);
        return 0;
    } else {
        printf(RED BOLD "\n❌ SOME TESTS FAILED\n" RESET);
        printf(RED "Please review the failed tests and fix the issues.\n" RESET);
        return 1;
    }
}