#!/usr/bin/env bash
set -euo pipefail
echo "Compiling voting-interface..."
gcc -std=c11 -O2 ../src/voting-interface.c -o voting-interface || exit 1
mkdir -p test_run
cd test_run
printf "12345\n67890\n" > approved_voters.txt
printf "PartyA|Alice\nPartyA|Bob\nPartyB|Carol\n" > candidates.txt
: > voted_voters.txt
: > votes.txt
echo "Running first vote (should record)..."
printf "12345\n1\n2\n" | ../voting-interface > run1.out 2>&1 || true
if ! grep -qE '^12345\\|' votes.txt; then echo "ERROR: vote not recorded in votes.txt"; exit 2; fi
if ! grep -q '^12345$' voted_voters.txt; then echo "ERROR: NIC not marked in voted_voters.txt"; exit 3; fi
echo "Running second attempt (should be blocked)..."
printf "12345\n" | ../voting-interface > run2.out 2>&1 || true
if ! grep -q "has already voted" run2.out; then echo "ERROR: second attempt was not blocked"; exit 4; fi
echo "All checks passed. Test artifacts are in: $(pwd)"
exit 0
