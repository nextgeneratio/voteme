#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../src/models.h"
#include "../src/entity_codec.h"
#include "../src/entity_service.h"
#include "../src/data_handler_enhanced.h"

static void ensure_data_dir()
{
    system("mkdir -p data");
}

static void ensure_headers()
{
    FILE *fp;
    if ((fp = fopen("data/approved_voters.txt", "r")) == NULL)
    {
        fp = fopen("data/approved_voters.txt", "w");
        fprintf(fp, "voting_number,name,nic,district_id\n");
        fclose(fp);
    }
    if ((fp = fopen("data/approved_candidates.txt", "r")) == NULL)
    {
        fp = fopen("data/approved_candidates.txt", "w");
        fprintf(fp, "candidate_number,name,party_id,district_id,nic\n");
        fclose(fp);
    }
}

static int candidate_equal(const Candidate *a, const Candidate *b)
{
    return strcmp(a->candidate_number, b->candidate_number) == 0 &&
           strcmp(a->name, b->name) == 0 &&
           strcmp(a->party_id, b->party_id) == 0 &&
           strcmp(a->district_id, b->district_id) == 0 &&
           strcmp(a->nic, b->nic) == 0;
}

static int voter_equal(const Voter *a, const Voter *b)
{
    return strcmp(a->voting_number, b->voting_number) == 0 &&
           strcmp(a->name, b->name) == 0 &&
           strcmp(a->nic, b->nic) == 0 &&
           strcmp(a->district_id, b->district_id) == 0;
}

int main(void)
{
    ensure_data_dir();
    ensure_headers();

    // Round-trip codec for Candidate
    Candidate c = {0};
    strcpy(c.candidate_number, "C777");
    strcpy(c.name, "Codec Candidate");
    strcpy(c.party_id, "P777");
    strcpy(c.district_id, "D07");
    strcpy(c.nic, "NIC777");

    char buf[256];
    if (!format_candidate_line(&c, buf, sizeof buf))
    {
        printf("FAIL: format_candidate_line\n");
        return 1;
    }

    Candidate c2 = {0};
    if (!parse_candidate_line(buf, &c2))
    {
        printf("FAIL: parse_candidate_line\n");
        return 1;
    }
    if (!candidate_equal(&c, &c2))
    {
        printf("FAIL: candidate round-trip mismatch\n");
        return 1;
    }

    // Service create/read for Candidate
    if (create_candidate_struct(&c) != DATA_SUCCESS)
    {
        printf("FAIL: create_candidate_struct err=%s\n", get_last_error());
        return 1;
    }
    Candidate c3 = {0};
    if (read_candidate_struct("C777", &c3) != DATA_SUCCESS)
    {
        printf("FAIL: read_candidate_struct err=%s\n", get_last_error());
        return 1;
    }
    if (!candidate_equal(&c, &c3))
    {
        printf("FAIL: read_candidate_struct mismatch\n");
        return 1;
    }

    // Round-trip codec for Voter
    Voter v = {0};
    strcpy(v.voting_number, "V777");
    strcpy(v.name, "Codec Voter");
    strcpy(v.nic, "NICV777");
    strcpy(v.district_id, "D07");

    if (!format_voter_line(&v, buf, sizeof buf))
    {
        printf("FAIL: format_voter_line\n");
        return 1;
    }

    Voter v2 = {0};
    if (!parse_voter_line(buf, &v2))
    {
        printf("FAIL: parse_voter_line\n");
        return 1;
    }
    if (!voter_equal(&v, &v2))
    {
        printf("FAIL: voter round-trip mismatch\n");
        return 1;
    }

    if (create_voter_struct(&v) != DATA_SUCCESS)
    {
        printf("FAIL: create_voter_struct err=%s\n", get_last_error());
        return 1;
    }
    Voter v3 = {0};
    if (read_voter_struct("V777", &v3) != DATA_SUCCESS)
    {
        printf("FAIL: read_voter_struct err=%s\n", get_last_error());
        return 1;
    }
    if (!voter_equal(&v, &v3))
    {
        printf("FAIL: read_voter_struct mismatch\n");
        return 1;
    }

    printf("PASS: models/codec/service tests.\n");
    return 0;
}
