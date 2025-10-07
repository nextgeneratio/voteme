#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../../src/data_handler.h"

// Test result tracking
int tests_passed = 0;
int tests_failed = 0;

// Color codes for output
#define GREEN "\033[32m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

// Test utility functions
void print_test_header(const char *test_name)
{
    printf("\n" BLUE "=== Testing %s ===" RESET "\n", test_name);
}

void assert_test(int condition, const char *test_description)
{
    if (condition)
    {
        printf(GREEN "✓ PASS: %s" RESET "\n", test_description);
        tests_passed++;
    }
    else
    {
        printf(RED "✗ FAIL: %s" RESET "\n", test_description);
        tests_failed++;
    }
}

void print_summary()
{
    printf("\n" YELLOW "=== Test Summary ===" RESET "\n");
    printf("Tests Passed: " GREEN "%d" RESET "\n", tests_passed);
    printf("Tests Failed: " RED "%d" RESET "\n", tests_failed);
    printf("Total Tests: %d\n", tests_passed + tests_failed);

    if (tests_failed == 0)
    {
        printf(GREEN "All tests passed!" RESET "\n");
    }
    else
    {
        printf(RED "Some tests failed. Please check the output above." RESET "\n");
    }
}

// Test data setup
void setup_test_data()
{
    printf(YELLOW "Setting up test data..." RESET "\n");

    // Create test voters file
    FILE *fp = fopen("../../data/test_voters.txt", "w");
    if (fp)
    {
        fprintf(fp, "voting_number,name,NIC,district_id\n");
        fprintf(fp, "V001,Alice Smith,199512345678,D001\n");
        fprintf(fp, "V002,Bob Johnson,198876543210,D002\n");
        fclose(fp);
    }

    // Create test candidates file
    fp = fopen("../../data/test_candidates.txt", "w");
    if (fp)
    {
        fprintf(fp, "candidate_number,name,party_id,district_id,nic\n");
        fprintf(fp, "C001,John Doe,P001,D001,198012345678\n");
        fprintf(fp, "C002,Jane Smith,P002,D002,197587654321\n");
        fclose(fp);
    }

    // Create test parties file
    fp = fopen("../../data/test_parties.txt", "w");
    if (fp)
    {
        fprintf(fp, "party_id,party_name\n");
        fprintf(fp, "P001,Democratic Party\n");
        fprintf(fp, "P002,Republican Party\n");
        fclose(fp);
    }

    // Create test districts file
    fp = fopen("../../data/test_districts.txt", "w");
    if (fp)
    {
        fprintf(fp, "district_id,district_name\n");
        fprintf(fp, "D001,Central District\n");
        fprintf(fp, "D002,Eastern District\n");
        fclose(fp);
    }

    printf(GREEN "Test data setup complete." RESET "\n");
}

void cleanup_test_data()
{
    printf(YELLOW "Cleaning up test data..." RESET "\n");
    remove("../../data/test_voters.txt");
    remove("../../data/test_candidates.txt");
    remove("../../data/test_parties.txt");
    remove("../../data/test_districts.txt");
    printf(GREEN "Cleanup complete." RESET "\n");
}

// Test functions for Generic CRUD operations
void test_generic_crud()
{
    print_test_header("Generic CRUD Operations");

    // Test create_record
    int result = create_record("../../data/test_voters.txt", "V003,Charlie Brown,199012345678,D001");
    assert_test(result == 1, "create_record should return 1 on success");

    // Test read_record
    char *primary_keys[] = {"0:V001"};
    char *record = read_record("../../data/test_voters.txt", primary_keys, 1);
    assert_test(record != NULL, "read_record should return non-NULL for existing record");
    assert_test(strstr(record, "Alice Smith") != NULL, "read_record should return correct data");
    if (record)
        free(record);

    // Test read non-existing record
    char *non_existing_keys[] = {"0:V999"};
    record = read_record("../../data/test_voters.txt", non_existing_keys, 1);
    assert_test(record == NULL, "read_record should return NULL for non-existing record");

    // Test update_record
    result = update_record("../../data/test_voters.txt", primary_keys, 1, "1:Alice Johnson");
    assert_test(result == 1, "update_record should return 1 on success");

    // Verify update
    record = read_record("../../data/test_voters.txt", primary_keys, 1);
    assert_test(record != NULL && strstr(record, "Alice Johnson") != NULL, "update_record should modify the correct field");
    if (record)
        free(record);

    // Test delete_record
    char *delete_keys[] = {"0:V002"};
    result = delete_record("../../data/test_voters.txt", delete_keys, 1);
    assert_test(result == 1, "delete_record should return 1 on success");

    // Verify deletion
    record = read_record("../../data/test_voters.txt", delete_keys, 1);
    assert_test(record == NULL, "delete_record should remove the record completely");
}

void test_file_operations()
{
    print_test_header("File Operations");

    // Test append_line
    int result = append_line("../../data/test_temp.txt", "Test line 1");
    assert_test(result == 1, "append_line should return 1 on success");

    result = append_line("../../data/test_temp.txt", "Test line 2");
    assert_test(result == 1, "append_line should append to existing file");

    // Test overwrite_file
    result = overwrite_file("../../data/test_temp.txt", "New content\nLine 2\n");
    assert_test(result == 1, "overwrite_file should return 1 on success");

    // Verify content
    FILE *fp = fopen("../../data/test_temp.txt", "r");
    if (fp)
    {
        char line[256];
        fgets(line, sizeof(line), fp);
        assert_test(strstr(line, "New content") != NULL, "overwrite_file should replace file content");
        fclose(fp);
    }

    // Cleanup
    remove("../../data/test_temp.txt");
}

void test_error_handling()
{
    print_test_header("Error Handling");

    // Test with non-existent file
    char *primary_keys[] = {"0:V001"};
    char *record = read_record("../../data/non_existent_file.txt", primary_keys, 1);
    assert_test(record == NULL, "read_record should handle non-existent files gracefully");

    // Test with invalid primary key format
    int result = update_record("../../data/test_voters.txt", NULL, 0, "1:NewName");
    assert_test(result == 0, "update_record should handle NULL primary keys");

    // Test with invalid field format
    result = update_record("../../data/test_voters.txt", primary_keys, 1, "invalid_format");
    assert_test(result == 0, "update_record should handle invalid field format");

    // Test append to read-only location (should fail gracefully)
    result = append_line("/root/readonly_file.txt", "test");
    assert_test(result == 0, "append_line should handle permission errors gracefully");
}

void test_edge_cases()
{
    print_test_header("Edge Cases");

    // Test with empty file
    FILE *fp = fopen("../../data/test_empty.txt", "w");
    if (fp)
    {
        fprintf(fp, "header1,header2\n");
        fclose(fp);
    }

    char *primary_keys[] = {"0:nonexistent"};
    char *record = read_record("../../data/test_empty.txt", primary_keys, 1);
    assert_test(record == NULL, "read_record should handle empty files (header only)");

    // Test with malformed CSV
    fp = fopen("../../data/test_malformed.txt", "w");
    if (fp)
    {
        fprintf(fp, "header1,header2\n");
        fprintf(fp, "value1\n");              // Missing second field
        fprintf(fp, "value3,value4,extra\n"); // Extra field
        fclose(fp);
    }

    record = read_record("../../data/test_malformed.txt", primary_keys, 1);
    assert_test(record == NULL, "read_record should handle malformed CSV gracefully");

    // Cleanup
    remove("../../data/test_empty.txt");
    remove("../../data/test_malformed.txt");
}

void test_memory_management()
{
    print_test_header("Memory Management");

    // Test multiple reads to check for memory leaks
    char *primary_keys[] = {"0:V001"};
    for (int i = 0; i < 100; i++)
    {
        char *record = read_record("../../data/test_voters.txt", primary_keys, 1);
        if (record)
        {
            free(record);
        }
    }
    assert_test(1, "Multiple read operations completed without crashes");

    // Test reading large records (within limits)
    create_record("../../data/test_voters.txt", "V999,Very Long Name That Exceeds Normal Length But Should Still Work,123456789012,D999");
    char *large_keys[] = {"0:V999"};
    char *large_record = read_record("../../data/test_voters.txt", large_keys, 1);
    assert_test(large_record != NULL, "read_record should handle longer records");
    if (large_record)
        free(large_record);
}

int main()
{
    printf(BLUE "=== Data Handler Testing and Debugging Suite ===" RESET "\n");
    printf("This suite will test all functions declared in data_handler.h\n");

    // Setup
    setup_test_data();

    // Run all tests
    test_file_operations();
    test_generic_crud();
    test_error_handling();
    test_edge_cases();
    test_memory_management();

    // Cleanup and summary
    cleanup_test_data();
    print_summary();

    return (tests_failed == 0) ? 0 : 1;
}