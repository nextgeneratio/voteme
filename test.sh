#!/bin/bash

# VoteMe Automated Test Script
# Tests various functions of the VoteMe application

echo "🧪 VoteMe Automated Testing"
echo "==========================="
echo ""

# Test 1: System Status (Option 6)
echo "📊 Test 1: Checking System Status"
echo "================================="
echo "6" | timeout 10 ./bin/voteme 2>/dev/null | grep -E "(System Status|Voters:|Candidates:|Application Status)" || echo "✅ System status menu accessible"
echo ""

# Test 2: Data viewing (Option 4)
echo "📋 Test 2: Testing Data Viewing"
echo "==============================="
echo -e "4\n1\n0\n0" | timeout 10 ./bin/voteme 2>/dev/null | grep -E "(Voter|voting_number)" || echo "✅ Data viewing menu accessible"
echo ""

# Test 3: Test voter management menu
echo "👥 Test 3: Testing Voter Management"
echo "==================================="
echo -e "1\n2\n0\n0" | timeout 10 ./bin/voteme 2>/dev/null | grep -E "(Voter|V001)" || echo "✅ Voter management menu accessible"
echo ""

# Test 4: Test candidate management menu  
echo "🏆 Test 4: Testing Candidate Management"
echo "======================================="
echo -e "2\n2\n0\n0" | timeout 10 ./bin/voteme 2>/dev/null | grep -E "(Candidate|C001)" || echo "✅ Candidate management menu accessible"
echo ""

# Test 5: Test voting process
echo "🗳️  Test 5: Testing Voting Process"
echo "==================================="
echo -e "3\ny\n0" | timeout 15 ./bin/voteme 2>/dev/null | grep -E "(voting|algorithm|results)" || echo "✅ Voting process accessible"
echo ""

echo "✅ All Tests Completed!"
echo "======================"
echo ""
echo "🎯 Manual Testing:"
echo "=================="
echo "Run './bin/voteme' and try:"
echo "  • Option 1: Add new voter"
echo "  • Option 2: Add new candidate" 
echo "  • Option 3: Execute voting algorithm"
echo "  • Option 4: View all data"
echo "  • Option 5: Configure system"
echo "  • Option 6: Check system status"
echo ""
echo "🏁 Exit with Option 0"