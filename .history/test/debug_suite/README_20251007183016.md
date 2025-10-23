# VoteMe Testing and Debugging Suite

This directory contains comprehensive testing and debugging tools for the `data_handler.h` functions.

## Test Files

### 1. test_data_handler.c

**Purpose**: Tests the core generic CRUD functions and file operations
**Tests Covered**:

- Generic CRUD operations (create_record, read_record, update_record, delete_record)
- File operations (append_line, overwrite_file)
- Error handling scenarios
- Edge cases (empty files, malformed CSV)
- Memory management

### 2. test_entity_functions.c

**Purpose**: Tests all entity-specific functions declared in data_handler.h
**Tests Covered**:

- Voter functions (create_voter, read_voter, update_voter, delete_voter)
- Candidate functions (create_candidate, read_candidate, update_candidate, delete_candidate)
- Party functions (create_party, read_party, update_party, delete_party)
- District functions (create_district, read_district, update_district, delete_district)
- Parliament candidate functions
- Voter count functions

## How to Run Tests

### Compile and Run Core Tests

```bash
cd /mnt/storage/Coding/Group Project/voteme/test/debug_suite
gcc -o test_core test_data_handler.c ../../src/data_handler.c -I../../src
./test_core
```

### Compile and Run Entity Tests

```bash
cd /mnt/storage/Coding/Group Project/voteme/test/debug_suite
gcc -o test_entities test_entity_functions.c ../../src/data_handler.c -I../../src
./test_entities
```

### Run All Tests

```bash
cd /mnt/storage/Coding/Group Project/voteme/test/debug_suite
gcc -o test_core test_data_handler.c ../../src/data_handler.c -I../../src
gcc -o test_entities test_entity_functions.c ../../src/data_handler.c -I../../src
./test_core && ./test_entities
```

## Test Output

The tests provide color-coded output:

- 🟢 **Green**: Test passed
- 🔴 **Red**: Test failed
- 🟡 **Yellow**: Information/setup messages
- 🔵 **Blue**: Section headers

## Test Categories

### 1. Functional Tests

- Verify that functions work as expected with valid inputs
- Test return values and output data

### 2. Error Handling Tests

- Test behavior with invalid inputs
- Test handling of non-existent files
- Test graceful failure scenarios

### 3. Edge Case Tests

- Empty files (header only)
- Malformed CSV data
- Large records
- Boundary conditions

### 4. Memory Management Tests

- Verify proper memory allocation/deallocation
- Test for memory leaks in read operations
- Multiple operation sequences

## Debugging Features

### Test Data Setup

- Automatically creates test data files
- Uses separate test files to avoid affecting real data
- Cleans up test files after completion

### Detailed Output

- Each test shows exactly what is being tested
- Failed tests provide context about what went wrong
- Summary statistics at the end

### Isolation

- Each test is independent
- Test data is reset between test suites
- No interference between different test runs

## Common Issues and Solutions

### Compilation Issues

1. **Cannot find header file**: Make sure you're in the correct directory and using the `-I../../src` flag
2. **Undefined references**: Ensure you're linking with `../../src/data_handler.c`

### Test Failures

1. **File permission errors**: Make sure the data directory is writable
2. **Function not implemented**: Some entity-specific functions may not be implemented yet
3. **Memory errors**: Check for proper malloc/free usage in data_handler.c

### Adding New Tests

To add new tests:

1. Follow the existing test pattern
2. Use `assert_test()` or `assert_entity_test()` for validation
3. Include setup and cleanup for any test data
4. Update this README with new test descriptions

## Performance Testing

The test suite also includes basic performance testing:

- Multiple read operations to test for memory leaks
- Large record handling
- File operation efficiency

## Integration Testing

These tests can be integrated into a CI/CD pipeline:

- Return codes indicate success (0) or failure (non-zero)
- Can be run automatically before commits
- Provide regression testing for code changes
