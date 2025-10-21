#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/voting.h"
#include "../src/data_handler_enhanced.h"

static void ensure_data_dir()
{
    system("mkdir -p data");
}

static void write_file(const char *path, const char *content)
{
    FILE *fp = fopen(path, "w");
    if (!fp)
    {
        perror(path);
        exit(1);
    }
    fputs(content, fp);
    fclose(fp);
}

int main(void)
{
    ensure_data_dir();

    // Prepare minimal candidate and votes dataset
    write_file("data/approved_candidates.txt",
               "candidate_number,name,party_id,district_id,nic\n"
               "C001,Alice,P001,D01,111V\n"
               "C002,Bob,P002,D01,222V\n");

    write_file("data/votes.txt",
               "voter_id,candidate_id\n"
               "V001,C001\n"
               "V002,C001\n"
               "V003,C002\n");

    // Execute with low threshold so at least one qualifies
    int rc = execute_voting_algorithm(2, 5);
    if (rc != DATA_SUCCESS)
    {
        printf("FAIL: execute_voting_algorithm returned %d (err: %s)\n", rc, get_last_error());
        return 1;
    }

    // Verify results file exists and contains expected winner
    FILE *fp = fopen("data/voting_results.txt", "r");
    if (!fp)
    {
        perror("data/voting_results.txt");
        return 1;
    }

    int found = 0;
    char line[512];
    while (fgets(line, sizeof line, fp))
    {
        if (strstr(line, "C001") && strstr(line, "Alice"))
        {
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found)
    {
        printf("FAIL: Expected candidate C001 (Alice) not found in results.\n");
        return 1;
    }

    printf("PASS: voting algorithm basic scenario.\n");
    return 0;
}
