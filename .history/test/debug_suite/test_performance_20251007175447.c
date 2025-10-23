#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../../src/data_handler.h"

// Performance test configuration
#define PERFORMANCE_ITERATIONS 1000
#define LARGE_RECORD_SIZE 200

// Color codes
#define GREEN "\033[32m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

void print_performance_header(const char *test_name) {
    printf("\n" BLUE "=== Performance Test: %s ===" RESET "\n", test_name);
}

double time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
}

void setup_performance_test_data() {
    printf(YELLOW "Setting up performance test data..." RESET "\n");
    
    FILE *fp = fopen("../../data/perf_test.txt", "w");
    if (fp) {
        fprintf(fp, "id,name,data\n");
        for (int i = 0; i < 1000; i++) {
            fprintf(fp, "ID%04d,Name%04d,SomeData%04d\n", i, i, i);
        }
        fclose(fp);
    }
    
    printf(GREEN "Performance test data ready." RESET "\n");
}

void test_read_performance() {
    print_performance_header("Read Operations");
    
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    int successful_reads = 0;
    for (int i = 0; i < PERFORMANCE_ITERATIONS; i++) {
        char key[20];
        snprintf(key, sizeof(key), "0:ID%04d", i % 1000);
        char *primary_keys[] = {key};
        
        char *record = read_record("../../data/perf_test.txt", primary_keys, 1);
        if (record) {
            successful_reads++;
            free(record);
        }
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = time_diff(start, end);
    
    printf("Read %d records in %.3f seconds\n", PERFORMANCE_ITERATIONS, elapsed);
    printf("Average time per read: %.6f seconds\n", elapsed / PERFORMANCE_ITERATIONS);
    printf("Successful reads: %d/%d\n", successful_reads, PERFORMANCE_ITERATIONS);
    printf("Read rate: %.2f operations/second\n", PERFORMANCE_ITERATIONS / elapsed);
}

void test_write_performance() {
    print_performance_header("Write Operations");
    
    // Create a temporary file for write testing
    FILE *fp = fopen("../../data/write_perf_test.txt", "w");
    if (fp) {
        fprintf(fp, "id,name,data\n");
        fclose(fp);
    }
    
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    int successful_writes = 0;
    for (int i = 0; i < 100; i++) { // Fewer iterations for write test
        char record[256];
        snprintf(record, sizeof(record), "WID%04d,WriteName%04d,WriteData%04d", i, i, i);
        
        if (create_record("../../data/write_perf_test.txt", record)) {
            successful_writes++;
        }
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = time_diff(start, end);
    
    printf("Wrote %d records in %.3f seconds\n", 100, elapsed);
    printf("Average time per write: %.6f seconds\n", elapsed / 100);
    printf("Successful writes: %d/%d\n", successful_writes, 100);
    printf("Write rate: %.2f operations/second\n", 100 / elapsed);
    
    // Cleanup
    remove("../../data/write_perf_test.txt");
}

void test_update_performance() {
    print_performance_header("Update Operations");
    
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    int successful_updates = 0;
    for (int i = 0; i < 50; i++) { // Even fewer iterations for update test
        char key[20];
        snprintf(key, sizeof(key), "0:ID%04d", i);
        char *primary_keys[] = {key};
        
        char new_name[50];
        snprintf(new_name, sizeof(new_name), "1:UpdatedName%04d", i);
        
        if (update_record("../../data/perf_test.txt", primary_keys, 1, new_name)) {
            successful_updates++;
        }
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = time_diff(start, end);
    
    printf("Updated %d records in %.3f seconds\n", 50, elapsed);
    printf("Average time per update: %.6f seconds\n", elapsed / 50);
    printf("Successful updates: %d/%d\n", successful_updates, 50);
    printf("Update rate: %.2f operations/second\n", 50 / elapsed);
}

void test_memory_usage() {
    print_performance_header("Memory Usage");
    
    printf("Testing memory allocation/deallocation patterns...\n");
    
    // Test multiple allocations without immediate free
    char **records = malloc(100 * sizeof(char*));
    int allocated = 0;
    
    for (int i = 0; i < 100; i++) {
        char key[20];
        snprintf(key, sizeof(key), "0:ID%04d", i);
        char *primary_keys[] = {key};
        
        records[i] = read_record("../../data/perf_test.txt", primary_keys, 1);
        if (records[i]) {
            allocated++;
        }
    }
    
    printf("Allocated %d records in memory\n", allocated);
    
    // Free all allocated memory
    for (int i = 0; i < 100; i++) {
        if (records[i]) {
            free(records[i]);
        }
    }
    free(records);
    
    printf(GREEN "Memory allocation/deallocation test completed successfully" RESET "\n");
}

void test_large_records() {
    print_performance_header("Large Record Handling");
    
    // Create a file with large records
    FILE *fp = fopen("../../data/large_record_test.txt", "w");
    if (fp) {
        fprintf(fp, "id,large_data\n");
        
        char large_data[LARGE_RECORD_SIZE];
        memset(large_data, 'A', LARGE_RECORD_SIZE - 1);
        large_data[LARGE_RECORD_SIZE - 1] = '\0';
        
        fprintf(fp, "LARGE001,%s\n", large_data);
        fclose(fp);
    }
    
    // Test reading large record
    char *primary_keys[] = {"0:LARGE001"};
    char *record = read_record("../../data/large_record_test.txt", primary_keys, 1);
    
    if (record) {
        printf("Successfully read large record of size: %zu bytes\n", strlen(record));
        free(record);
        printf(GREEN "Large record handling test passed" RESET "\n");
    } else {
        printf(RED "Failed to read large record" RESET "\n");
    }
    
    // Cleanup
    remove("../../data/large_record_test.txt");
}

void test_concurrent_file_access() {
    print_performance_header("File Access Patterns");
    
    // Simulate multiple rapid file operations
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int i = 0; i < 50; i++) {
        // Read operation
        char key[20];
        snprintf(key, sizeof(key), "0:ID%04d", i % 10);
        char *primary_keys[] = {key};
        char *record = read_record("../../data/perf_test.txt", primary_keys, 1);
        if (record) free(record);
        
        // Write operation
        char new_record[100];
        snprintf(new_record, sizeof(new_record), "TEMP%04d,TempName%04d,TempData%04d", i, i, i);
        create_record("../../data/perf_test.txt", new_record);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = time_diff(start, end);
    
    printf("Completed mixed operations in %.3f seconds\n", elapsed);
    printf("Average time per operation: %.6f seconds\n", elapsed / 100);
}

void cleanup_performance_test_data() {
    printf(YELLOW "Cleaning up performance test data..." RESET "\n");
    remove("../../data/perf_test.txt");
    printf(GREEN "Cleanup complete." RESET "\n");
}

int main() {
    printf(BLUE "=== Data Handler Performance Testing Suite ===" RESET "\n");
    printf("This suite will test the performance characteristics of data_handler.h functions\n");
    
    setup_performance_test_data();
    
    test_read_performance();
    test_write_performance();
    test_update_performance();
    test_memory_usage();
    test_large_records();
    test_concurrent_file_access();
    
    cleanup_performance_test_data();
    
    printf("\n" GREEN "=== Performance testing completed ===" RESET "\n");
    
    return 0;
}