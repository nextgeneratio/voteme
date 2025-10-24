#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "csv_io.h"
#include "data_handler_enhanced.h"
#include "data_errors.h"
#include "voting-interface.h"

#define INPUT_BUF 256

static void trim_newline(char *s)
{
    if (!s)
        return;
    size_t n = strlen(s);
    if (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r'))
        s[--n] = '\0';
    if (n > 0 && (s[n - 1] == '\r'))
        s[--n] = '\0';
}

// Trim leading and trailing ASCII whitespace
static void trim_spaces(char *s)
{
    if (!s)
        return;
    char *start = s;
    while (*start && isspace((unsigned char)*start))
        start++;
    char *end = s + strlen(s);
    while (end > start && isspace((unsigned char)end[-1]))
        end--;
    size_t len = (size_t)(end - start);
    if (start != s)
        memmove(s, start, len);
    s[len] = '\0';
}

// Local safe strdup (portable)
static char *sdup(const char *s)
{
    size_t len = strlen(s) + 1;
    char *p = malloc(len);
    if (p)
        memcpy(p, s, len);
    return p;
}

// Normalize party id by uppercasing and removing leading zeros after 'P'
static void normalize_party_id(const char *in, char *out, size_t outsz)
{
    if (!in || !out || outsz == 0)
        return;
    char tmp[MAX_LINE_LENGTH];
    strncpy(tmp, in, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';
    trim_spaces(tmp);
    for (char *p = tmp; *p; ++p)
        *p = (char)toupper((unsigned char)*p);
    if (tmp[0] == 'P')
    {
        const char *digits = tmp + 1;
        while (*digits == '0')
            digits++;
        out[0] = 'P';
        size_t n = 1;
        if (*digits == '\0')
        {
            // all zeros or empty -> P0
            if (n < outsz - 1)
            {
                out[n++] = '0';
            }
            out[n] = '\0';
            return;
        }
        while (*digits && n < outsz - 1)
            out[n++] = *digits++;
        out[n] = '\0';
    }
    else
    {
        strncpy(out, tmp, outsz - 1);
        out[outsz - 1] = '\0';
    }
}

static bool eq_party_id(const char *a, const char *b)
{
    char na[MAX_LINE_LENGTH], nb[MAX_LINE_LENGTH];
    normalize_party_id(a, na, sizeof(na));
    normalize_party_id(b, nb, sizeof(nb));
    return strcmp(na, nb) == 0;
}

static bool ensure_votes_file_exists(const char *path)
{
    FILE *f = fopen(path, "r");
    if (f)
    {
        fclose(f);
        return true;
    }
    // Create with header if not present
    return overwrite_file(path, "voter_id,candidate_id\n") == DATA_SUCCESS;
}

// Read party_id -> party_name into arrays for quick lookup
static int load_party_names(const char *parties_path, char *ids[], char *names[], int max)
{
    FILE *f = fopen(parties_path, "r");
    if (!f)
        return 0;
    int n = 0;
    while (1)
    {
        char *fields[MAX_FIELDS] = {0};
        int nf = read_csv_line(f, fields, MAX_FIELDS, ',');
        if (nf <= 0)
            break;
        if (nf >= 2 && n < max)
        {
            trim_spaces(fields[0]);
            trim_spaces(fields[1]);
            ids[n] = sdup(fields[0]);
            names[n] = sdup(fields[1]);
            if (ids[n] && names[n])
                n++;
            else
            {
                free(ids[n]);
                free(names[n]);
            }
        }
        for (int i = 0; i < nf; ++i)
            free(fields[i]);
    }
    fclose(f);
    return n;
}

// Build unique list of party IDs that have at least one candidate
static int collect_candidate_backed_parties(const char *candidates_path, char *out_ids[], int max_ids)
{
    FILE *f = fopen(candidates_path, "r");
    if (!f)
        return 0;
    int count = 0;
    while (1)
    {
        char *fields[MAX_FIELDS] = {0};
        int nf = read_csv_line(f, fields, MAX_FIELDS, ',');
        if (nf <= 0)
            break;
        if (nf >= 3)
        {
            trim_spaces(fields[2]);
            const char *pid = fields[2];
            int seen = 0;
            for (int i = 0; i < count; ++i)
                if (eq_party_id(out_ids[i], pid))
                {
                    seen = 1;
                    break;
                }
            if (!seen && count < max_ids)
            {
                out_ids[count] = sdup(pid);
                if (out_ids[count])
                    count++;
            }
        }
        for (int i = 0; i < nf; ++i)
            free(fields[i]);
    }
    fclose(f);
    return count;
}

static const char *lookup_party_name(const char *id, char *pids[], char *pnames[], int n)
{
    for (int i = 0; i < n; ++i)
        if (pids[i] && eq_party_id(pids[i], id))
            return pnames[i];
    return NULL;
}

static int list_candidates_for_party(const char *candidates_path, const char *party_id, char *out_ids[], int max_ids)
{
    FILE *f = fopen(candidates_path, "r");
    if (!f)
    {
        fprintf(stderr, "Error: cannot open %s (%s)\n", candidates_path, get_last_error());
        return -1;
    }
    int count = 0;
    printf("\nCandidates in selected party (%s):\n", party_id);
    while (1)
    {
        char *fields[MAX_FIELDS] = {0};
        int nf = read_csv_line(f, fields, MAX_FIELDS, ',');
        if (nf <= 0)
            break;
        // Expect: candidate_id, candidate_name, party_id, district_id, ...
        if (nf >= 3)
        {
            trim_spaces(fields[2]);
            if (eq_party_id(fields[2], party_id))
            {
                trim_spaces(fields[0]);
                trim_spaces(fields[1]);
                printf("  %s - %s\n", fields[0], fields[1]);
                if (count < max_ids)
                {
                    if (eq_party_id(fields[2], party_id))
                    if (out_ids[count])
                        count++;
                }
            }
        }
        for (int i = 0; i < nf; ++i)
            free(fields[i]);
    }
    fclose(f);
    if (count == 0)
    {
        printf("No candidates found for party %s.\n", party_id);
    }
    return count;
}

// (count_candidates_for_party removed; superseded by collect_candidate_backed_parties)

static bool id_in_list(const char *id, char *ids[], int n)
{
    for (int i = 0; i < n; ++i)
    {
        if (ids[i] && strcmp(id, ids[i]) == 0)
            return true;
    }
    return false;
}

int vote_for_candidate_interactive(void)
{
    const char *parties_path = "data/party_name.txt";
    const char *candidates_path = "data/approved_candidates.txt";
    const char *votes_path = "data/votes.txt"; // persistent votes file used by voting.c

    char buf[INPUT_BUF];

    // 1) Prompt voter id and validate
    printf("Enter your Voter ID: ");
    if (!fgets(buf, sizeof(buf), stdin))
        return DATA_ERROR_MALFORMED_DATA;
    trim_newline(buf);
    if (buf[0] == '\0')
    {
        printf("Voter ID cannot be empty.\n");
        return DATA_ERROR_INVALID_INPUT;
    }

    // Validate voter exists
    char *voter_rec = read_voter(buf);
    if (!voter_rec)
    {
        printf("Voter ID '%s' not found or not approved.\n", buf);
        return DATA_ERROR_RECORD_NOT_FOUND;
    }
    // Save a safe copy and free record
    char voter_id_copy[MAX_LINE_LENGTH];
    strncpy(voter_id_copy, buf, sizeof(voter_id_copy) - 1);
    voter_id_copy[sizeof(voter_id_copy) - 1] = '\0';
    free(voter_rec);

    // 2) Show parties from party_name.txt (with names), and prompt until valid and has candidates
    char *party_ids[256] = {0}, *party_names[256] = {0};
    int known_parties = load_party_names(parties_path, party_ids, party_names, 256);
    if (known_parties <= 0)
        return DATA_ERROR_RECORD_NOT_FOUND;

    while (1)
    {
        printf("\nAvailable parties:\n");
        printf("  party_id - party_name\n");
        for (int i = 0; i < known_parties; ++i)
        {
            if (party_ids[i] && party_names[i])
                printf("  %s - %s\n", party_ids[i], party_names[i]);
        }

        printf("\nSelect a Party by entering Party ID (or 'q' to cancel): ");
        if (!fgets(buf, sizeof(buf), stdin))
        {
            for (int i = 0; i < known_parties; ++i)
            {
                free(party_ids[i]);
                free(party_names[i]);
            }
            return DATA_ERROR_MALFORMED_DATA;
        }
        trim_newline(buf);
        if (strcmp(buf, "q") == 0 || strcmp(buf, "Q") == 0)
        {
            for (int i = 0; i < known_parties; ++i)
            {
                free(party_ids[i]);
                free(party_names[i]);
            }
            printf("Cancelled.\n");
            return DATA_ERROR_INVALID_INPUT;
        }
        if (buf[0] == '\0')
        {
            printf("Party ID cannot be empty.\n");
            continue;
        }

        // Find the selected party in the known list
        int sel_index = -1;
        for (int i = 0; i < known_parties; ++i)
        {
            if (party_ids[i] && eq_party_id(buf, party_ids[i]))
            {
                sel_index = i;
                break;
            }
        }
        if (sel_index < 0)
        {
            printf("Party '%s' is not in the party list. Please choose from the list.\n", buf);
            continue;
        }

        // Ensure there are candidates for the selected party; otherwise re-prompt
        int has = 0;
        {
            FILE *cf = fopen(candidates_path, "r");
            if (cf)
            {
                while (1)
                {
                    char *fields[MAX_FIELDS] = {0};
                    int nf = read_csv_line(cf, fields, MAX_FIELDS, ',');
                    if (nf <= 0)
                        break;
                    if (nf >= 3)
                    {
                        trim_spaces(fields[2]);
                        if (eq_party_id(fields[2], party_ids[sel_index]))
                        {
                            has = 1;
                        }
                    }
                    for (int j = 0; j < nf; ++j)
                        free(fields[j]);
                    if (has)
                        break;
                }
                fclose(cf);
            }
        }
        if (!has)
        {
            printf("No candidates found for party %s - %s. Please choose another party.\n",
                   party_ids[sel_index], party_names[sel_index] ? party_names[sel_index] : "");
            continue;
        }

        // Stash the selected canonical party id into buf and proceed
        strncpy(buf, party_ids[sel_index], sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';
        break;
    }

    // Validate party exists. Prefer party list, but accept if candidates exist for this ID.
    char party_id[MAX_LINE_LENGTH];
    strncpy(party_id, buf, sizeof(party_id) - 1);
    party_id[sizeof(party_id) - 1] = '\0';

    // 3) Show candidates filtered by selected party
    char *candidate_ids[128] = {0};
    int cand_count = list_candidates_for_party(candidates_path, party_id, candidate_ids, 128);
    if (cand_count < 0)
        return DATA_ERROR_MALFORMED_DATA;
    if (cand_count == 0)
        return DATA_ERROR_RECORD_NOT_FOUND;

    while (1)
    {
        printf("\nSelect a Candidate by entering Candidate ID (or 'q' to cancel): ");
        if (!fgets(buf, sizeof(buf), stdin))
            return DATA_ERROR_MALFORMED_DATA;
        trim_newline(buf);
        if (strcmp(buf, "q") == 0 || strcmp(buf, "Q") == 0)
        {
            for (int i = 0; i < cand_count; ++i)
                free(candidate_ids[i]);
            printf("Cancelled.\n");
            return DATA_ERROR_INVALID_INPUT;
        }
        if (buf[0] == '\0')
        {
            printf("Candidate ID cannot be empty.\n");
            continue;
        }
        if (!id_in_list(buf, candidate_ids, cand_count))
        {
            printf("Candidate ID '%s' is not in the selected party.\n", buf);
            continue;
        }
        break;
    }
    char candidate_id[MAX_LINE_LENGTH];
    strncpy(candidate_id, buf, sizeof(candidate_id) - 1);
    candidate_id[sizeof(candidate_id) - 1] = '\0';

    // 4) Persist the vote
    if (!ensure_votes_file_exists(votes_path))
    {
        fprintf(stderr, "Failed to prepare votes file '%s' (%s)\n", votes_path, get_last_error());
        // clean up ids
        for (int i = 0; i < cand_count; ++i)
            free(candidate_ids[i]);
        return DATA_ERROR_MALFORMED_DATA;
    }

    char record[MAX_LINE_LENGTH];
    snprintf(record, sizeof(record), "%s,%s", voter_id_copy, candidate_id);
    int err = append_line(votes_path, record);
    if (err != DATA_SUCCESS)
    {
        fprintf(stderr, "Failed to record vote (code %d)\n", err);
        // cleanup allocated ids
        for (int i = 0; i < cand_count; ++i)
            free(candidate_ids[i]);
        return err;
    }

    printf("\nYour vote has been recorded. Thank you.\n");
    for (int i = 0; i < cand_count; ++i)
        free(candidate_ids[i]);
    return DATA_SUCCESS;
}
