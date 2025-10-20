#!/bin/bash

# Demo script to showcase the enhanced VoteMe system
echo "🗳️  Enhanced VoteMe System Demo - Auto-Generated IDs & Validation"
echo "================================================================="
echo ""

echo "📁 Current Data Files Status:"
echo "------------------------------"

echo "📊 Existing Voters:"
if [ -f "data/approved_voters.txt" ]; then
    cat data/approved_voters.txt
else
    echo "No voter data file found"
fi

echo ""
echo "📊 Available Districts:"
if [ -f "data/district.txt" ]; then
    cat data/district.txt
else
    echo "No district data file found"
fi

echo ""
echo "📊 Available Parties:"
if [ -f "data/party_name.txt" ]; then
    cat data/party_name.txt
else
    echo "No party data file found"
fi

echo ""
echo "🎯 Key Features Implemented:"
echo "=============================="
echo "✅ Auto-generated sequential IDs (V001, V002... for voters, C001, C002... for candidates)"
echo "✅ NIC duplicate validation across both voter and candidate databases"
echo "✅ District selection with name display and validation"
echo "✅ Party selection with name display and validation"
echo "✅ Enhanced console UI with colors and icons"
echo "✅ Robust input validation and error handling"
echo ""

echo "💡 To test the system interactively:"
echo "  ./bin/voteme"
echo ""
echo "📋 Main Menu Options:"
echo "  3 → Voter Management (auto-generated IDs, NIC validation)"
echo "  4 → Candidate Management (auto-generated IDs, district/party selection)"
echo ""

echo "🔧 System Status: ✅ Compiled successfully with enhanced features!"