// Simple test harness: compiles src/voting-interface.c, prepares test files, runs the program and verifies results.
// Writes a portable shell script (LF endings) then executes it.

#include <stdio.h>
#include <stdlib.h>

static int write_file(const char *path, const char *contents) {
    FILE *f = fopen(path, "wb");
    if (!f) return 0;
    fputs(contents, f);
    fclose(f);
    return 1;
}

int main(void) {
    const char *script =
        "#!/usr/bin/env bash\n"
        "set -euo pipefail\n"
        "echo \"Compiling voting-interface...\"\n"
        "gcc -std=c11 -O2 ../src/voting-interface.c -o voting-interface || exit 1\n"
        "mkdir -p test_run\n"
        "cd test_run\n"
        "printf \"12345\\n67890\\n\" > approved_voters.txt\n"
        "printf \"PartyA|Alice\\nPartyA|Bob\\nPartyB|Carol\\n\" > candidates.txt\n"
        ": > voted_voters.txt\n"
        ": > votes.txt\n"
        "echo \"Running first vote (should record)...\"\n"
        "printf \"12345\\n1\\n2\\n\" | ../voting-interface > run1.out 2>&1 || true\n"
        "if ! grep -qE '^12345\\\\|' votes.txt; then echo \"ERROR: vote not recorded in votes.txt\"; exit 2; fi\n"
        "if ! grep -q '^12345$' voted_voters.txt; then echo \"ERROR: NIC not marked in voted_voters.txt\"; exit 3; fi\n"
        "echo \"Running second attempt (should be blocked)...\"\n"
        "printf \"12345\\n\" | ../voting-interface > run2.out 2>&1 || true\n"
        "if ! grep -q \"has already voted\" run2.out; then echo \"ERROR: second attempt was not blocked\"; exit 4; fi\n"
        "echo \"All checks passed. Test artifacts are in: $(pwd)\"\n"
        "exit 0\n";

    if (!write_file("run_test.sh", script)) {
        fprintf(stderr, "Failed to write run_test.sh\n");
        return 1;
    }

    if (system("chmod +x run_test.sh") != 0) {
        fprintf(stderr, "Failed to chmod run_test.sh\n");
        return 2;
    }

    int rc = system("./run_test.sh");
    if (rc != 0) {
        fprintf(stderr, "Test script exited with code %d\n", rc);
        return rc;
    }

    puts("Test completed successfully.");
    return 0;
}