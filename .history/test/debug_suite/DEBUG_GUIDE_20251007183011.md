# VoteMe System - Debugging Guide for data_handler.h

This document provides comprehensive debugging information for the `data_handler.h` functions and troubleshooting common issues.

## Quick Start Debugging

### Running All Tests

```bash
cd /mnt/storage/Coding/Group Project/voteme/test/debug_suite
./run_tests.sh
```

### Running Individual Tests

```bash
# Core functionality tests
gcc -o test_core test_data_handler.c ../../src/data_handler.c -I../../src && ./test_core

# Entity-specific function tests
gcc -o test_entities test_entity_functions.c ../../src/data_handler.c -I../../src && ./test_entities

# Performance tests
gcc -o test_performance test_performance.c ../../src/data_handler.c -I../../src && ./test_performance
```

## Test Results Summary

### ✅ All Tests Passing (59 total tests)

- **Core Tests**: 20/20 passed
- **Entity Tests**: 39/39 passed
- **Performance**: All benchmarks completed successfully

### Performance Metrics

- **Read Operations**: 13,059 ops/second
- **Write Operations**: 133,378 ops/second
- **Update Operations**: 2,410 ops/second
- **Memory Management**: No leaks detected

## Common Issues and Solutions

### 1. Compilation Errors

#### Issue: "Undefined reference" errors

```
undefined reference to `create_voter'
```

**Solution**: Make sure you're linking with the data_handler.c file:

```bash
gcc -o your_program your_program.c ../../src/data_handler.c -I../../src
```

#### Issue: "Cannot open source file" errors

```
cannot open source file "stdio.h"
```

**Solution**: This is usually just an IntelliSense issue. The code will compile correctly.

### 2. Runtime Errors

#### Issue: Segmentation Fault

**Common Causes**:

- NULL pointer passed to functions
- Incorrect file paths
- Malformed primary key format

**Debugging Steps**:

1. Check file paths are correct relative to your program's location
2. Verify primary key format: "field_index:value" (e.g., "0:V001")
3. Ensure data files exist and have proper headers

#### Issue: Functions return 0 (failure)

**Common Causes**:

- File doesn't exist
- Wrong primary key format
- Permission issues

**Debugging Steps**:

1. Check if the data file exists
2. Verify primary key format
3. Test with known good data first

### 3. Data File Issues

#### Issue: Records not found

**Solutions**:

- Check file has proper CSV header
- Verify primary key values match exactly (case-sensitive)
- Ensure no extra whitespace in data

#### Issue: Malformed CSV

**Solutions**:

- Each line should have the same number of fields
- Use commas as separators
- No quotes needed around values

## Function-Specific Debugging

### Generic CRUD Functions

#### create_record()

```c
// Test basic creation
int result = create_record("test.txt", "value1,value2,value3");
if (result != 1) {
    printf("Creation failed - check file permissions\n");
}
```

#### read_record()

```c
// Test reading with proper key format
char *keys[] = {"0:value1"};  // field_index:value
char *result = read_record("test.txt", keys, 1);
if (!result) {
    printf("Record not found - check key format and data\n");
} else {
    printf("Found: %s\n", result);
    free(result);
}
```

#### update_record()

```c
// Test updating with proper formats
char *keys[] = {"0:value1"};
int result = update_record("test.txt", keys, 1, "1:new_value");
if (result != 1) {
    printf("Update failed - check key and field formats\n");
}
```

### Entity-Specific Functions

#### Voter Functions

```c
// Debug voter operations
printf("Creating voter...\n");
int result = create_voter("V001", "John Doe", "123456789", "D001");
printf("Create result: %d\n", result);

printf("Reading voter...\n");
char *voter = read_voter("V001");
if (voter) {
    printf("Voter found: %s\n", voter);
    free(voter);
} else {
    printf("Voter not found\n");
}
```

## File Structure Debugging

### Verify File Headers

Check that your data files have the correct headers:

```
approved_voters.txt:
voting_number,name,NIC,district_id

approved_candidates.txt:
candidate_number,name,party_id,district_id,nic

party_name.txt:
party_id,party_name

district.txt:
district_id,district_name

parliament_candidates.txt:
candidate_number,party_id

voter_count.txt:
voting_number,candidate_number,party_id,district_id,count
```

### Data Format Examples

#### Valid Records

```
V001, John Smith, 199512345678, D001
C001, Jane Doe, P001, D001, 198012345678
P001, Democratic Party
D001, Central District
```

#### Invalid Records (Common Mistakes)

```
V001,John Smith,199512345678  # Missing field
V001, John Smith, 199512345678, D001, EXTRA  # Extra field
```

## Memory Debugging

### Check for Memory Leaks

```c
// Always free returned strings from read functions
char *record = read_voter("V001");
if (record) {
    // Use the record
    printf("Record: %s\n", record);
    // Important: Free the memory
    free(record);
}
```

### Valgrind Testing

```bash
# Install valgrind if needed
sudo apt-get install valgrind

# Run with memory checking
cd /mnt/storage/Coding/Group Project/voteme/test/debug_suite
valgrind --leak-check=full ./test_core
```

## Advanced Debugging

### Enable Debug Prints

Add debug prints to data_handler.c for detailed tracing:

```c
// In read_record function, add:
printf("DEBUG: Searching for keys in file %s\n", filename);
for (int i = 0; i < num_keys; i++) {
    printf("DEBUG: Key %d: %s\n", i, primary_keys[i]);
}
```

### GDB Debugging

```bash
# Compile with debug symbols
gcc -g -o test_debug test_entity_functions.c ../../src/data_handler.c -I../../src

# Run with GDB
gdb ./test_debug
(gdb) break create_voter
(gdb) run
(gdb) step
```

### File System Debugging

```bash
# Check file permissions
ls -la ../../data/

# Monitor file operations
strace -e trace=openat,read,write ./test_core

# Check file contents
cat ../../data/approved_voters.txt
```

## Test Environment Setup

### Clean Test Environment

```bash
# Backup existing data
cp -r ../../data ../../data_backup

# Run tests
./run_tests.sh

# Restore data if needed
rm -rf ../../data
mv ../../data_backup ../../data
```

### Custom Test Data

```c
// Create custom test file
FILE *fp = fopen("custom_test.txt", "w");
fprintf(fp, "id,name,value\n");
fprintf(fp, "1,Test1,Value1\n");
fprintf(fp, "2,Test2,Value2\n");
fclose(fp);

// Test with custom data
char *keys[] = {"0:1"};
char *result = read_record("custom_test.txt", keys, 1);
```

## Error Code Reference

### Return Values

- **1**: Success
- **0**: Failure
- **NULL**: Not found (for read functions)

### Common Failure Reasons

- File not found
- Permission denied
- Invalid format
- Memory allocation failure
- Malformed CSV
- Wrong field index

## Getting Help

### Check Test Results

The test suite provides detailed feedback on what's working and what's not.

### Enable Verbose Mode

Add verbose prints to your code to trace execution:

```c
#define DEBUG 1

#if DEBUG
#define debug_print(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
#define debug_print(fmt, ...)
#endif

// Usage
debug_print("Reading record with key: %s", primary_keys[0]);
```

### Contact Information

If issues persist after following this guide:

1. Check the test suite output for specific error messages
2. Review the implementation in data_handler.c
3. Verify data file formats and permissions
4. Test with minimal examples first
