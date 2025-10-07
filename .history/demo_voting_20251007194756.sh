#!/bin/bash

# VoteMe Voting Algorithm Demonstration Script
echo "==============================================="
echo "🗳️  VoteMe Voting Algorithm Demonstration"
echo "==============================================="
echo ""

# Navigate to project directory
cd "/mnt/storage/Coding/Group Project/voteme"

# Show current system status
echo "📊 Current System Status:"
make -f Makefile_admin status
echo ""

# Check if votes file exists, if not create sample votes
if [ ! -f "data/votes.txt" ]; then
    echo "📝 Creating sample votes file for demonstration..."
    echo "voter_id,candidate_id" > data/votes.txt
    echo "V001,C001" >> data/votes.txt  # John votes for Alice
    echo "V002,C001" >> data/votes.txt  # Jane votes for Alice  
    echo "V003,C002" >> data/votes.txt  # Bob votes for Charlie
    echo "✅ Sample votes created!"
    echo ""
fi

# Show votes data
echo "🗳️  Current Votes:"
echo "==================="
cat data/votes.txt
echo ""

# Show candidates data
echo "👥 Current Candidates:"
echo "======================"
cat data/approved_candidates.txt
echo ""

# Create a simple C program to test the voting algorithm directly
cat > test_voting.c << 'EOF'
#include <stdio.h>
#include "src/data_handler_enhanced.h"
#include "src/voting.h"

int main() {
    printf("🗳️  Testing VoteMe Voting Algorithm\n");
    printf("====================================\n\n");
    
    // Set voting parameters
    int min_votes = 1;      // Minimum votes for parliament
    int max_parliament = 2;  // Maximum parliament members
    
    printf("⚙️  Voting Parameters:\n");
    printf("• Minimum votes for parliament: %d\n", min_votes);
    printf("• Maximum parliament members: %d\n", max_parliament);
    printf("\n");
    
    // Execute voting algorithm
    int result = execute_voting_algorithm(min_votes, max_parliament);
    
    if (result == DATA_SUCCESS) {
        printf("\n✅ Voting algorithm completed successfully!\n");
        printf("📄 Check 'data/voting_results.txt' for detailed results.\n");
    } else {
        printf("\n❌ Voting algorithm failed with error code: %d\n", result);
    }
    
    return 0;
}
EOF

# Compile the test program
echo "🔨 Compiling voting algorithm test..."
gcc -Wall -std=c99 -Isrc -o test_voting test_voting.c src/data_handler_enhanced.c src/voting.c

if [ $? -eq 0 ]; then
    echo "✅ Compilation successful!"
    echo ""
    
    # Run the voting algorithm
    echo "🚀 Executing Voting Algorithm:"
    echo "==============================="
    ./test_voting
    echo ""
    
    # Show results if they exist
    if [ -f "data/voting_results.txt" ]; then
        echo "📋 Voting Results Summary:"
        echo "=========================="
        head -20 data/voting_results.txt
        echo ""
        echo "💾 Full results saved in data/voting_results.txt"
    fi
    
    # Clean up
    rm -f test_voting test_voting.c
else
    echo "❌ Compilation failed!"
fi

echo ""
echo "🎉 Voting Algorithm Demonstration Complete!"
echo "==============================================="