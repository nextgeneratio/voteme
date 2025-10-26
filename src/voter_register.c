// Refactored voter_register to use data_handler_enhanced APIs and enforce no-duplicates in approved_voters.txt

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "data_handler_enhanced.h"
#include "data_errors.h"
#include "voting-interface.h"

#define APPROVED_VOTERS_FILE "data/approved_voters.txt"
#define SYSTEM_CONFIG_FILE "data/system_config.txt"
#define DISTRICT_FILE "data/district.txt"

// Utility: trim trailing newline and surrounding spaces
static void trim(char *s)
{
    if (!s)
        return;
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r' || s[n - 1] == ' ' || s[n - 1] == '\t'))
        s[--n] = '\0';
    // trim leading spaces
    char *start = s;
    while (*start == ' ' || *start == '\t')
        start++;
    if (start != s)
        memmove(s, start, strlen(start) + 1);
}

static void prompt(const char *label, char *buf, size_t bufsz)
{
    printf("%s: ", label);
    fflush(stdout);
    if (!fgets(buf, bufsz, stdin))
    {
        buf[0] = '\0';
        return;
    }
    trim(buf);
}

// Display available districts from district.txt (ID and Name)
static void print_available_districts(void)
{
    FILE *fp = fopen(DISTRICT_FILE, "r");
    if (!fp)
    {
        printf("(District list not available: %s missing)\n", DISTRICT_FILE);
        return;
    }
    char line[512];
    int printed_header = 0;
    while (fgets(line, sizeof line, fp))
    {
        // skip header
        if (!printed_header)
        {
            printed_header = 1;
            if (strncmp(line, "district_id,", 12) == 0)
                continue;
        }
        // split into id,name
        char *p = line;
        while (*p == ' ' || *p == '\t')
            p++;
        char *comma = strchr(p, ',');
        if (!comma)
            continue;
        *comma = '\0';
        char *id = p;
        char *name = comma + 1;
        char *nl = strchr(name, '\n');
        if (nl)
            *nl = '\0';
        trim(id);
        trim(name);
        if (*id)
            printf(" - %s : %s\n", id, name[0] ? name : "(no name)");
    }
    fclose(fp);
}

// Validate if a district id exists in district.txt
static bool is_valid_district_id(const char *district_id)
{
    FILE *fp = fopen(DISTRICT_FILE, "r");
    if (!fp)
        return true; // if list missing, don't block entry
    char line[512];
    int checked_header = 0;
    while (fgets(line, sizeof line, fp))
    {
        if (!checked_header)
        {
            checked_header = 1;
            if (strncmp(line, "district_id,", 12) == 0)
                continue;
        }
        char *p = line;
        while (*p == ' ' || *p == '\t')
            p++;
        char *comma = strchr(p, ',');
        if (!comma)
            continue;
        *comma = '\0';
        trim(p);
        if (strcmp(p, district_id) == 0)
        {
            fclose(fp);
            return true;
        }
    }
    fclose(fp);
    return false;
}

// Prompt for district id with list preview and validation loop
static void prompt_for_district_id(char *out, size_t outsz)
{
    printf("Available Districts:\n");
    print_available_districts();
    while (1)
    {
        prompt("Enter District ID", out, outsz);
        if (out[0] == '\0')
        {
            printf("District ID cannot be empty.\n");
            continue;
        }
        if (!is_valid_district_id(out))
        {
            printf("Invalid District ID. Please choose one from the list above.\n");
            continue;
        }
        break;
    }
}

// Count data rows (excluding CSV header) in a file; returns -1 if file missing
static int count_data_rows(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return -1;
    int count = 0;
    char line[1024];
    // skip header if present
    if (fgets(line, sizeof line, fp))
    {
        while (fgets(line, sizeof line, fp))
            count++;
    }
    fclose(fp);
    return count;
}

// Read max_voters from system_config.txt; default to 10000 if not found
static int read_max_voters(void)
{
    FILE *fp = fopen(SYSTEM_CONFIG_FILE, "r");
    if (!fp)
        return 10000; // sensible default
    char line[256];
    int max_voters = 10000;
    while (fgets(line, sizeof line, fp))
    {
        if (strncmp(line, "max_voters=", 11) == 0)
        {
            max_voters = atoi(line + 11);
            break;
        }
    }
    fclose(fp);
    if (max_voters <= 0)
        max_voters = 10000;
    return max_voters;
}

// Generate next voting_number in the format V#### (zero-padded, grows beyond 4 as needed)
static void generate_next_voting_number(char *out, size_t outsz)
{
    FILE *f = fopen(APPROVED_VOTERS_FILE, "r");
    int max_num = 0;
    if (f)
    {
        char line[1024];
        // read header if present
        if (fgets(line, sizeof line, f))
        {
            while (fgets(line, sizeof line, f))
            {
                // get first field
                char *p = line;
                while (*p == ' ' || *p == '\t')
                    p++;
                char *end = strchr(p, ',');
                if (!end)
                    continue;
                *end = '\0';
                trim(p);
                // Expect starts with 'V' followed by digits
                if (p[0] == 'V')
                {
                    char *numstr = p + 1;
                    // skip any leading zeros for atoi correctness (atoi handles fine)
                    int n = atoi(numstr);
                    if (n > max_num)
                        max_num = n;
                }
            }
        }
        fclose(f);
    }
    // next number is max+1; format with at least 4 digits, but don't truncate larger numbers
    int next_num = max_num + 1;
    // Use a temp buffer with enough room
    char tmp[32];
    // if next_num < 10000 we want zero padding to 4, else natural width
    if (next_num < 10000)
        snprintf(tmp, sizeof tmp, "V%04d", next_num);
    else
        snprintf(tmp, sizeof tmp, "V%d", next_num);
    // copy to out
    strncpy(out, tmp, outsz - 1);
    out[outsz - 1] = '\0';
}

// Check if a NIC already exists in approved_voters.txt
static bool nic_exists_in_file(const char *nic, const char *except_voting_number)
{
    FILE *f = fopen(APPROVED_VOTERS_FILE, "r");
    if (!f)
        return false; // treat as not existing if file missing
    char line[1024];
    while (fgets(line, sizeof line, f))
    {
        // skip header if present
        if (strncmp(line, "voting_number,", 14) == 0)
            continue;
        // Parse CSV: voting_number, name, nic, district_id
        char *p = line;
        char *fields[4] = {0};
        int idx = 0;
        // simple split by ',' (file format may include spaces after comma)
        for (; idx < 4; ++idx)
        {
            while (*p == ' ')
                p++;
            fields[idx] = p;
            char *c = strchr(p, ',');
            if (c)
            {
                *c = '\0';
                p = c + 1;
            }
            else
            {
                // last field up to EOL
                char *nl = strchr(p, '\n');
                if (nl)
                    *nl = '\0';
                idx++;
                break;
            }
        }
        if (idx >= 3 && fields[2])
        {
            // trim spaces
            for (int i = 0; i < 4 && fields[i]; ++i)
                trim(fields[i]);
            if (except_voting_number && fields[0] && strcmp(fields[0], except_voting_number) == 0)
                continue; // skip self when updating
            if (strcmp(fields[2], nic) == 0)
            {
                fclose(f);
                return true;
            }
        }
    }
    fclose(f);
    return false;
}

// Function prototypes (only expose what we use in this CLI)
static void createVoter(void);

int main(void)
{
    char input[16];
    for (;;)
    {
        printf("\n====================================\n");
        printf("       VOTER MANAGEMENT SYSTEM       \n");
        printf("====================================\n\n");
        printf("1. Register New Voter\n");
        printf("2. Vote for Candidate\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        if (!fgets(input, sizeof input, stdin))
            return 0;
        int choice = atoi(input);

        switch (choice)
        {
        case 1:
            createVoter();
            break;
        case 2:
            // Launch the interactive voting flow from voting-interface.c
            vote_for_candidate_interactive();
            break;
        case 3:
            printf("Goodbye!\n");
            return 0;
        default:
            printf("Invalid choice! Try again.\n");
            break;
        }
    }
}

// ---------------- Create Function ----------------
static void createVoter(void)
{
    char voting_number[64], name[128], nic[64], district_id[64];

    // Enforce system limit: max_voters
    int max_voters = read_max_voters();
    int current_count = count_data_rows(APPROVED_VOTERS_FILE);
    if (current_count >= 0 && current_count >= max_voters)
    {
        printf("Registration closed: maximum voters limit (%d) reached.\n", max_voters);
        return;
    }

    // Auto-generate next voting number
    generate_next_voting_number(voting_number, sizeof voting_number);
    printf("Assigned Voting Number: %s\n", voting_number);

    prompt("Enter Full Name", name, sizeof name);
    if (name[0] == '\0')
    {
        printf("Name cannot be empty.\n");
        return;
    }
    prompt("Enter NIC", nic, sizeof nic);
    if (nic[0] == '\0')
    {
        printf("NIC cannot be empty.\n");
        return;
    }
    // Ensure NIC is unique across approved_voters.txt
    if (nic_exists_in_file(nic, NULL))
    {
        printf("Error: NIC already exists.\n");
        return;
    }
    prompt_for_district_id(district_id, sizeof district_id);

    int rc = create_voter(voting_number, name, nic, district_id);
    if (rc == DATA_SUCCESS)
        printf("✅ Voter Registered Successfully.\n");
    else
        printf("❌ Failed to register voter.\n");
}

// ---------------- Read Function ----------------
#if 0
// These functions are intentionally disabled in this CLI since only Admin can
// view/update/delete voters. They are kept for reference.
static void readVoters(void) {}
static void updateVoter(void) {}
static void deleteVoter(void) {}
#endif