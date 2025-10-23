#!/bin/bash

# VoteMe Data Handler Test Runner
# This script compiles and runs all tests for data_handler.h

echo "========================================="
echo "  VoteMe Data Handler Testing Suite"
echo "========================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Change to the test directory
cd "$(dirname "$0")"
TEST_DIR=$(pwd)
echo -e "${BLUE}Test directory: ${TEST_DIR}${NC}"

# Function to compile and run a test
run_test() {
    local test_name=$1
    local source_file=$2
    local executable=$3
    
    echo -e "\n${YELLOW}=== Compiling ${test_name} ===${NC}"
    
    # Compile the test
    if gcc -o "${executable}" "${source_file}" ../../src/data_handler.c -I../../src -Wall -Wextra; then
        echo -e "${GREEN}✓ Compilation successful${NC}"
        
        echo -e "\n${YELLOW}=== Running ${test_name} ===${NC}"
        
        # Run the test
        if ./"${executable}"; then
            echo -e "\n${GREEN}✓ ${test_name} completed successfully${NC}"
            return 0
        else
            echo -e "\n${RED}✗ ${test_name} failed${NC}"
            return 1
        fi
    else
        echo -e "${RED}✗ Compilation failed for ${test_name}${NC}"
        return 1
    fi
}

# Initialize counters
total_tests=0
passed_tests=0

# Run core data handler tests
echo -e "\n${BLUE}Running Core Data Handler Tests...${NC}"
if run_test "Core Data Handler Tests" "test_data_handler.c" "test_core"; then
    ((passed_tests++))
fi
((total_tests++))

# Run entity function tests
echo -e "\n${BLUE}Running Entity Function Tests...${NC}"
if run_test "Entity Function Tests" "test_entity_functions.c" "test_entities"; then
    ((passed_tests++))
fi
((total_tests++))

# Clean up executables
echo -e "\n${YELLOW}Cleaning up...${NC}"
rm -f test_core test_entities

# Print final summary
echo -e "\n========================================="
echo -e "           FINAL TEST SUMMARY"
echo -e "========================================="
echo -e "Total Test Suites: ${total_tests}"
echo -e "Passed: ${GREEN}${passed_tests}${NC}"
echo -e "Failed: ${RED}$((total_tests - passed_tests))${NC}"

if [ $passed_tests -eq $total_tests ]; then
    echo -e "\n${GREEN}🎉 All test suites passed! 🎉${NC}"
    exit 0
else
    echo -e "\n${RED}❌ Some test suites failed. Please check the output above.${NC}"
    exit 1
fi