#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/data_handler_enhanced.h"

static void ensure_data_dir()
{
    system("mkdir -p data");
}

int main(void)
{
    ensure_data_dir();

    // Initialize core data files with headers if missing
    FILE *fp;
    if ((fp = fopen("data/approved_voters.txt", "r")) == NULL)
    {
        fp = fopen("data/approved_voters.txt", "w");
        if (!fp)
        {
            perror("approved_voters.txt");
            return 1;
        }
        fprintf(fp, "voting_number,name,nic,district_id\n");
        fclose(fp);
    }
    if ((fp = fopen("data/approved_candidates.txt", "r")) == NULL)
    {
        fp = fopen("data/approved_candidates.txt", "w");
        if (!fp)
        {
            perror("approved_candidates.txt");
            return 1;
        }
        fprintf(fp, "candidate_number,name,party_id,district_id,nic\n");
        fclose(fp);
    }

    // Create a voter and candidate, then read them back
    int rc;
    rc = create_voter("V100", "Test Voter", "NIC100", "D01");
    if (rc != DATA_SUCCESS)
    {
        printf("FAIL: create_voter rc=%d err=%s\n", rc, get_last_error());
        return 1;
    }

    rc = create_candidate("C100", "Test Candidate", "P100", "D01", "NIC200");
    if (rc != DATA_SUCCESS)
    {
        printf("FAIL: create_candidate rc=%d err=%s\n", rc, get_last_error());
        return 1;
    }

    char *v = read_voter("V100");
    if (!v)
    {
        printf("FAIL: read_voter err=%s\n", get_last_error());
        return 1;
    }
    free(v);

    char *c = read_candidate("C100");
    if (!c)
    {
        printf("FAIL: read_candidate err=%s\n", get_last_error());
        return 1;
    }
    free(c);

    printf("PASS: data handler smoke test.\n");
    return 0;
}
