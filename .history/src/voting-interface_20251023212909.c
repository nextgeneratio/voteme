#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "csv_io.h"
#include "data_handler_enhanced.h"
#include "data_errors.h"
#include "voting-interface.h"

#define INPUT_BUF 256

static void trim_newline(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    if (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r')) s[--n] = '\0';
    if (n > 0 && (s[n-1] == '\r')) s[--n] = '\0';
}

static bool ensure_votes_file_exists(const char *path) {
    FILE *f = fopen(path, "r");
    if (f) { fclose(f); return true; }
    // Create with header if not present
    return overwrite_file(path, "voter_id,candidate_id\n") == DATA_SUCCESS;
}

static bool list_parties_and_exists(const char *path, const char *party_id) {
    // Print available parties; if party_id != NULL, also check its existence
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Error: cannot open %s (%s)\n", path, get_last_error());
        return false;
    }
    bool found = false;
    printf("\nAvailable parties:\n");
    while (1) {
        char *fields[MAX_FIELDS] = {0};
        int nf = read_csv_line(f, fields, MAX_FIELDS, ',');
        if (nf <= 0) break;
        if (nf >= 2) {
            // Expect: party_id,party_name
            printf("  %s - %s\n", fields[0], fields[1]);
            if (party_id && strcmp(fields[0], party_id) == 0) {
                found = true;
            }
        }
        for (int i = 0; i < nf; ++i) free(fields[i]);
    }
    fclose(f);
    return party_id ? found : true;
}

static int list_candidates_for_party(const char *candidates_path, const char *party_id, char *out_ids[], int max_ids) {
    FILE *f = fopen(candidates_path, "r");
    if (!f) {
        fprintf(stderr, "Error: cannot open %s (%s)\n", candidates_path, get_last_error());
        return -1;
    }
    int count = 0;
    printf("\nCandidates in selected party (%s):\n", party_id);
    while (1) {
        char *fields[MAX_FIELDS] = {0};
        int nf = read_csv_line(f, fields, MAX_FIELDS, ',');
        if (nf <= 0) break;
        // Expect: candidate_id, candidate_name, party_id, district_id, ...
        if (nf >= 3 && strcmp(fields[2], party_id) == 0) {
            printf("  %s - %s\n", fields[0], fields[1]);
            if (count < max_ids) {
                out_ids[count] = strdup(fields[0]);
                if (out_ids[count]) count++;
            }
        }
        for (int i = 0; i < nf; ++i) free(fields[i]);
    }
    fclose(f);
    if (count == 0) {
        printf("No candidates found for party %s.\n", party_id);
    }
    return count;
}

static bool id_in_list(const char *id, char *ids[], int n) {
    for (int i = 0; i < n; ++i) {
        if (ids[i] && strcmp(id, ids[i]) == 0) return true;
    }
    return false;
}

int vote_for_candidate_interactive(void) {
    const char *voters_path = "data/approved_voters.txt";
    const char *parties_path = "data/party_name.txt";
    const char *candidates_path = "data/approved_candidates.txt";
    const char *votes_path = "data/votes.txt"; // persistent votes file used by voting.c

    char buf[INPUT_BUF];

    // 1) Prompt voter id and validate
    printf("Enter your Voter ID: ");
    if (!fgets(buf, sizeof(buf), stdin)) return DATA_ERROR_MALFORMED_DATA;
    trim_newline(buf);
    if (buf[0] == '\0') { printf("Voter ID cannot be empty.\n"); return DATA_ERROR_INVALID_INPUT; }

    // Validate voter exists
    char *voter_rec = read_voter(buf);
    if (!voter_rec) {
        printf("Voter ID '%s' not found or not approved.\n", buf);
    return DATA_ERROR_RECORD_NOT_FOUND;
    }
    // Save a safe copy and free record
    char voter_id_copy[MAX_LINE_LENGTH];
    strncpy(voter_id_copy, buf, sizeof(voter_id_copy) - 1);
    voter_id_copy[sizeof(voter_id_copy) - 1] = '\0';
    free(voter_rec);

    // 2) Show party list and prompt for party id
    if (!list_parties_and_exists(parties_path, NULL)) {
        return DATA_ERROR_FILE_NOT_FOUND;
    }

    printf("\nSelect a Party by entering Party ID: ");
    if (!fgets(buf, sizeof(buf), stdin)) return DATA_ERROR_MALFORMED_DATA;
    trim_newline(buf);
    if (buf[0] == '\0') { printf("Party ID cannot be empty.\n"); return DATA_ERROR_INVALID_INPUT; }

    // Validate party exists using CRUD helper as well (defensive)
    char *party_rec = read_party(buf);
    if (!party_rec) {
        printf("Party ID '%s' not found.\n", buf);
    return DATA_ERROR_RECORD_NOT_FOUND;
    }
    // Also confirm via listing function
    if (!list_parties_and_exists(parties_path, buf)) {
        printf("Party ID '%s' not found.\n", buf);
        free(party_rec);
    return DATA_ERROR_RECORD_NOT_FOUND;
    }
    char party_id[MAX_LINE_LENGTH];
    strncpy(party_id, buf, sizeof(party_id) - 1);
    party_id[sizeof(party_id) - 1] = '\0';
    free(party_rec);

    // 3) Show candidates filtered by selected party
    char *candidate_ids[128] = {0};
    int cand_count = list_candidates_for_party(candidates_path, party_id, candidate_ids, 128);
    if (cand_count < 0) return DATA_ERROR_MALFORMED_DATA;
    if (cand_count == 0) return DATA_ERROR_RECORD_NOT_FOUND;

    printf("\nSelect a Candidate by entering Candidate ID: ");
    if (!fgets(buf, sizeof(buf), stdin)) return DATA_ERROR_MALFORMED_DATA;
    trim_newline(buf);
    if (buf[0] == '\0') { printf("Candidate ID cannot be empty.\n"); return DATA_ERROR_INVALID_INPUT; }
    if (!id_in_list(buf, candidate_ids, cand_count)) {
        printf("Candidate ID '%s' is not in the selected party.\n", buf);
    return DATA_ERROR_RECORD_NOT_FOUND;
    }
    char candidate_id[MAX_LINE_LENGTH];
    strncpy(candidate_id, buf, sizeof(candidate_id) - 1);
    candidate_id[sizeof(candidate_id) - 1] = '\0';

    // 4) Persist the vote
    if (!ensure_votes_file_exists(votes_path)) {
        fprintf(stderr, "Failed to prepare votes file '%s' (%s)\n", votes_path, get_last_error());
        // clean up ids
        for (int i = 0; i < cand_count; ++i) free(candidate_ids[i]);
        return DATA_ERROR_MALFORMED_DATA;
    }

    char record[MAX_LINE_LENGTH];
    snprintf(record, sizeof(record), "%s,%s\n", voter_id_copy, candidate_id);
    int err = append_line(votes_path, record);
    if (err != DATA_SUCCESS) {
        fprintf(stderr, "Failed to record vote (code %d)\n", err);
        // cleanup allocated ids
        for (int i = 0; i < cand_count; ++i) free(candidate_ids[i]);
        return err;
    }

    printf("\nYour vote has been recorded. Thank you.\n");
    for (int i = 0; i < cand_count; ++i) free(candidate_ids[i]);
    return DATA_SUCCESS;
}