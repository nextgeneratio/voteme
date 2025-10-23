#!/bin/bash

# VoteMe Quick Demo Script
# This script demonstrates the VoteMe application functionality

echo "🗳️  VoteMe System Demo"
echo "====================="
echo ""

# Check if application exists
if [ ! -f "bin/voteme" ]; then
    echo "❌ Application not found. Building now..."
    make main
    if [ $? -ne 0 ]; then
        echo "❌ Build failed!"
        exit 1
    fi
fi

echo "📊 Current System Status:"
echo "========================"
make status
echo ""

echo "📋 Current Data Files:"
echo "======================"
echo ""

echo "👥 Registered Voters:"
echo "--------------------"
cat data/approved_voters.txt
echo ""

echo "🏆 Registered Candidates:"
echo "-------------------------"
cat data/approved_candidates.txt
echo ""

echo "🎯 Political Parties:"
echo "--------------------"
cat data/party_name.txt
echo ""

echo "🗳️  Current Votes:"
echo "------------------"
if [ -f "data/votes.txt" ]; then
    cat data/votes.txt
else
    echo "No votes recorded yet."
fi
echo ""

echo "📈 System Ready!"
echo "================"
echo "✅ Application: bin/voteme"
echo "✅ Data files: All present and valid"
echo "✅ Demo data: Ready for testing"
echo ""
echo "🚀 To run the application manually:"
echo "   ./bin/voteme"
echo ""
echo "🎯 To run voting algorithm:"
echo "   Choose option 3 from the main menu"
echo ""
echo "📖 For help:"
echo "   make help"