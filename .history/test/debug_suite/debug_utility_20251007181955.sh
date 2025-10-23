#!/bin/bash

# VoteMe Data Handler Debug Utility
# This script provides quick debugging commands for the data handler

echo "🔧 VoteMe Data Handler Debug Utility"
echo "======================================"

# Function to display menu
show_menu() {
    echo ""
    echo "Select an option:"
    echo "1. Run all tests"
    echo "2. Run core tests only"
    echo "3. Run entity tests only" 
    echo "4. Run performance tests"
    echo "5. Check data file structure"
    echo "6. Verify compilation"
    echo "7. Clean test files"
    echo "8. Memory leak check (requires valgrind)"
    echo "9. Quick function test"
    echo "0. Exit"
    echo ""
}

# Function to check data files
check_data_files() {
    echo "📁 Checking data file structure..."
    
    DATA_DIR="../../data"
    if [ ! -d "$DATA_DIR" ]; then
        echo "❌ Data directory not found: $DATA_DIR"
        return 1
    fi
    
    files=("approved_voters.txt" "approved_candidates.txt" "party_name.txt" "district.txt" "parliament_candidates.txt" "voter_count.txt")
    
    for file in "${files[@]}"; do
        filepath="$DATA_DIR/$file"
        if [ -f "$filepath" ]; then
            echo "✅ $file exists"
            echo "   Header: $(head -n 1 "$filepath")"
            echo "   Records: $(($(wc -l < "$filepath") - 1))"
        else
            echo "❌ $file missing"
        fi
    done
}

# Function to verify compilation
check_compilation() {
    echo "🔨 Verifying compilation..."
    
    if gcc -c ../../src/data_handler.c -I../../src 2>/dev/null; then
        echo "✅ data_handler.c compiles without errors"
        rm -f data_handler.o
    else
        echo "❌ Compilation errors found:"
        gcc -c ../../src/data_handler.c -I../../src
    fi
}

# Function to clean test files
clean_tests() {
    echo "🧹 Cleaning test files..."
    rm -f test_core test_entities test_performance
    rm -f ../../data/test_* ../../data/perf_*
    echo "✅ Test files cleaned"
}

# Function for memory leak check
memory_check() {
    if ! command -v valgrind &> /dev/null; then
        echo "❌ Valgrind not installed. Install with: sudo apt-get install valgrind"
        return 1
    fi
    
    echo "🔍 Running memory leak check..."
    gcc -g -o test_memory test_data_handler.c ../../src/data_handler.c -I../../src
    valgrind --leak-check=full --show-leak-kinds=all ./test_memory
    rm -f test_memory
}

# Function for quick function test
quick_test() {
    echo "⚡ Quick function test..."
    
    cat > quick_test.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include "../../src/data_handler.h"

int main() {
    printf("Testing basic CRUD operations...\n");
    
    // Test file creation
    FILE *fp = fopen("quick_test_data.txt", "w");
    if (fp) {
        fprintf(fp, "id,name,value\n");
        fprintf(fp, "1,Test,Value\n");
        fclose(fp);
    }
    
    // Test read
    char *keys[] = {"0:1"};
    char *result = read_record("quick_test_data.txt", keys, 1);
    if (result) {
        printf("✅ Read test passed: %s", result);
        free(result);
    } else {
        printf("❌ Read test failed\n");
    }
    
    // Test create
    if (create_record("quick_test_data.txt", "2,Test2,Value2")) {
        printf("✅ Create test passed\n");
    } else {
        printf("❌ Create test failed\n");
    }
    
    // Cleanup
    remove("quick_test_data.txt");
    
    return 0;
}
EOF
    
    gcc -o quick_test quick_test.c ../../src/data_handler.c -I../../src
    ./quick_test
    rm -f quick_test quick_test.c
}

# Main loop
while true; do
    show_menu
    read -p "Enter your choice: " choice
    
    case $choice in
        1)
            echo "🧪 Running all tests..."
            ./run_tests.sh
            ;;
        2)
            echo "🧪 Running core tests..."
            gcc -o test_core test_data_handler.c ../../src/data_handler.c -I../../src && ./test_core
            ;;
        3)
            echo "🧪 Running entity tests..."
            gcc -o test_entities test_entity_functions.c ../../src/data_handler.c -I../../src && ./test_entities
            ;;
        4)
            echo "🧪 Running performance tests..."
            gcc -o test_performance test_performance.c ../../src/data_handler.c -I../../src && ./test_performance
            ;;
        5)
            check_data_files
            ;;
        6)
            check_compilation
            ;;
        7)
            clean_tests
            ;;
        8)
            memory_check
            ;;
        9)
            quick_test
            ;;
        0)
            echo "👋 Goodbye!"
            break
            ;;
        *)
            echo "❌ Invalid option. Please try again."
            ;;
    esac
    
    echo ""
    read -p "Press Enter to continue..."
done