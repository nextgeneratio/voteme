// Refactored candidate_register to use data_handler_enhanced APIs, auto-generate IDs,
// enforce max_candidates limit, and list/validate parties and districts.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "data_handler_enhanced.h"
#include "data_errors.h"

#define APPROVED_CANDIDATES_FILE "data/approved_candidates.txt"
#define SYSTEM_CONFIG_FILE "data/system_config.txt"
#define PARTY_FILE "data/party_name.txt"
#define DISTRICT_FILE "data/district.txt"

// ANSI color codes
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"

// Utility: trim trailing newline and surrounding spaces
static void trim(char *s)
{
    if (!s)
        return;
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r' || s[n - 1] == ' ' || s[n - 1] == '\t'))
        s[--n] = '\0';
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

// File helpers
static int count_data_rows(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return -1;
    int count = 0;
    char line[1024];
    if (fgets(line, sizeof line, fp))
    {
        while (fgets(line, sizeof line, fp))
            count++;
    }
    fclose(fp);
    return count;
}

static int read_max_candidates(void)
{
    FILE *fp = fopen(SYSTEM_CONFIG_FILE, "r");
    if (!fp)
        return 500; // default consistent with admin.c
    char line[256];
    int max_candidates = 500;
    while (fgets(line, sizeof line, fp))
    {
        if (strncmp(line, "max_candidates=", 15) == 0)
        {
            max_candidates = atoi(line + 15);
            break;
        }
    }
    fclose(fp);
    if (max_candidates <= 0)
        max_candidates = 500;
    return max_candidates;
}

// Generate next candidate number in format C### (3-digit padding, then natural width)
static void generate_next_candidate_number(char *out, size_t outsz)
{
    FILE *f = fopen(APPROVED_CANDIDATES_FILE, "r");
    int max_num = 0;
    if (f)
    {
        char line[1024];
        if (fgets(line, sizeof line, f))
        {
            while (fgets(line, sizeof line, f))
            {
                char *p = line;
                while (*p == ' ' || *p == '\t')
                    p++;
                char *end = strchr(p, ',');
                if (!end)
                    continue;
                *end = '\0';
                trim(p);
                if (p[0] == 'C')
                {
                    int n = atoi(p + 1);
                    if (n > max_num)
                        max_num = n;
                }
            }
        }
        fclose(f);
    }
    int next = max_num + 1;
    char tmp[32];
    if (next < 1000)
        snprintf(tmp, sizeof tmp, "C%03d", next);
    else
        snprintf(tmp, sizeof tmp, "C%d", next);
    strncpy(out, tmp, outsz - 1);
    out[outsz - 1] = '\0';
}

// Uniqueness: NIC must be unique among candidates
static bool candidate_nic_exists(const char *nic, const char *except_candidate_number)
{
    FILE *f = fopen(APPROVED_CANDIDATES_FILE, "r");
    if (!f)
        return false;
    char line[1024];
    while (fgets(line, sizeof line, f))
    {
        if (strncmp(line, "candidate_number,", 18) == 0)
            continue;
        char *p = line;
        char *fields[5] = {0};
        int idx = 0;
        for (; idx < 5; ++idx)
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
                char *nl = strchr(p, '\n');
                if (nl)
                    *nl = '\0';
                idx++;
                break;
            }
        }
        for (int i = 0; i < 5 && fields[i]; ++i)
            trim(fields[i]);
        if (idx >= 5 && fields[4])
        {
            if (except_candidate_number && fields[0] && strcmp(fields[0], except_candidate_number) == 0)
                continue;
            if (strcmp(fields[4], nic) == 0)
            {
                fclose(f);
                return true;
            }
        }
    }
    fclose(f);
    return false;
}

// District helpers (list + validation)
static void print_available_districts(void)
{
    FILE *fp = fopen(DISTRICT_FILE, "r");
    if (!fp)
    {
        printf("(District list not available)\n");
        return;
    }
    char line[512];
    int header = 0;
    while (fgets(line, sizeof line, fp))
    {
        if (!header)
        {
            header = 1;
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

static bool is_valid_district_id(const char *district_id)
{
    FILE *fp = fopen(DISTRICT_FILE, "r");
    if (!fp)
        return true; // don't block if list missing
    char line[512];
    int header = 0;
    while (fgets(line, sizeof line, fp))
    {
        if (!header)
        {
            header = 1;
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
            printf("Invalid District ID. Choose from the list above.\n");
            continue;
        }
        break;
    }
}

// Party helpers (list + validation)
static void print_available_parties(void)
{
    FILE *fp = fopen(PARTY_FILE, "r");
    if (!fp)
    {
        printf("(Party list not available)\n");
        return;
    }
    char line[512];
    int header = 0;
    while (fgets(line, sizeof line, fp))
    {
        if (!header)
        {
            header = 1;
            if (strncmp(line, "party_id,", 9) == 0)
                continue;
        }
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

static bool is_valid_party_id(const char *party_id)
{
    FILE *fp = fopen(PARTY_FILE, "r");
    if (!fp)
        return true;
    char line[512];
    int header = 0;
    while (fgets(line, sizeof line, fp))
    {
        if (!header)
        {
            header = 1;
            if (strncmp(line, "party_id,", 9) == 0)
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
        if (strcmp(p, party_id) == 0)
        {
            fclose(fp);
            return true;
        }
    }
    fclose(fp);
    return false;
}

static void prompt_for_party_id(char *out, size_t outsz)
{
    printf("Available Parties:\n");
    print_available_parties();
    while (1)
    {
        prompt("Enter Party ID", out, outsz);
        if (out[0] == '\0')
        {
            printf("Party ID cannot be empty.\n");
            continue;
        }
        if (!is_valid_party_id(out))
        {
            printf("Invalid Party ID. Choose from the list above.\n");
            continue;
        }
        break;
    }
}

// CRUD prototypes
static void createCandidate(void);
static void readCandidates(void);
static void updateCandidate(void);
static void deleteCandidate(void);

int main(void)
{
    int choice;
    char input[16];
    while (1)
    {
        printf(YELLOW "\n=====================================\n");
        printf("     CANDIDATE MANAGEMENT SYSTEM     \n");
        printf("=====================================\n" RESET);
        printf("1. Register New Candidate\n");
        printf("2. View All Candidates\n");
        printf("3. Update Candidate\n");
        printf("4. Delete Candidate\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        if (!fgets(input, sizeof input, stdin))
            return 0;
        choice = atoi(input);

        switch (choice)
        {
        case 1:
            createCandidate();
            break;
        case 2:
            readCandidates();
            break;
        case 3:
            updateCandidate();
            break;
        case 4:
            deleteCandidate();
            break;
        case 5:
            printf(GREEN "\nGoodbye!\n" RESET);
            return 0;
        default:
            printf(RED "Invalid choice! Try again.\n" RESET);
            break;
        }
    }
}

// ---------------- Create ----------------
static void createCandidate(void)
{
    // Enforce system limit
    int max_candidates = read_max_candidates();
    int current = count_data_rows(APPROVED_CANDIDATES_FILE);
    if (current >= 0 && current >= max_candidates)
    {
        printf(RED "Registration closed: maximum candidates limit (%d) reached.\n" RESET, max_candidates);
        return;
    }

    char candidate_number[32], name[128], party_id[32], district_id[32], nic[64];
    generate_next_candidate_number(candidate_number, sizeof candidate_number);
    printf("Assigned Candidate Number: %s\n", candidate_number);

    prompt("Enter Full Name", name, sizeof name);
    if (!name[0])
    {
        printf("Name cannot be empty.\n");
        return;
    }

    prompt_for_party_id(party_id, sizeof party_id);
    prompt_for_district_id(district_id, sizeof district_id);

    prompt("Enter NIC", nic, sizeof nic);
    if (!nic[0])
    {
        printf("NIC cannot be empty.\n");
        return;
    }
    if (candidate_nic_exists(nic, NULL))
    {
        printf("Error: NIC already exists.\n");
        return;
    }

    int rc = create_candidate(candidate_number, name, party_id, district_id, nic);
    if (rc == DATA_SUCCESS)
        printf("✅ Candidate Registered Successfully.\n");
    else
        printf("❌ Failed to register candidate. %s\n", get_last_error());
}

// ---------------- Read (list all) ----------------
static void readCandidates(void)
{
    FILE *f = fopen(APPROVED_CANDIDATES_FILE, "r");
    if (!f)
    {
        printf("No candidate data found.\n");
        return;
    }
    printf("\n======= APPROVED CANDIDATES =======\n");
    printf("%-12s | %-20s | %-8s | %-10s | %-12s\n",
           "candidate", "name", "party", "district", "nic");
    printf("-------------+----------------------+----------+------------+--------------\n");
    char line[1024];
    while (fgets(line, sizeof line, f))
    {
        if (strncmp(line, "candidate_number,", 18) == 0)
            continue;
        char *p = line;
        char *fields[5] = {0};
        for (int i = 0; i < 5; ++i)
        {
            while (*p == ' ')
                p++;
            fields[i] = p;
            char *c = strchr(p, ',');
            if (c)
            {
                *c = '\0';
                p = c + 1;
            }
            else
            {
                char *nl = strchr(p, '\n');
                if (nl)
                    *nl = '\0';
                break;
            }
        }
        for (int i = 0; i < 5 && fields[i]; ++i)
            trim(fields[i]);
        if (fields[0] && *fields[0])
            printf("%-12s | %-20s | %-8s | %-10s | %-12s\n",
                   fields[0], fields[1] ? fields[1] : "",
                   fields[2] ? fields[2] : "",
                   fields[3] ? fields[3] : "",
                   fields[4] ? fields[4] : "");
    }
    fclose(f);
}

// ---------------- Update ----------------
static void updateCandidate(void)
{
    char candidate_number[32];
    prompt("Enter Candidate Number to update", candidate_number, sizeof candidate_number);
    if (!candidate_number[0])
    {
        printf("Candidate Number is required.\n");
        return;
    }

    char *current = read_candidate(candidate_number);
    if (!current)
    {
        printf("Candidate not found.\n");
        return;
    }
    printf("Current record: %s\n", current);
    free(current);

    printf("Choose field to update:\n");
    printf("1. Name\n2. Party ID\n3. District ID\n4. NIC\nEnter choice (1-4): ");
    int choice = 0;
    char buf[16];
    if (!fgets(buf, sizeof buf, stdin))
        return;
    choice = atoi(buf);
    if (choice < 1 || choice > 4)
    {
        printf("Invalid choice.\n");
        return;
    }

    char new_value[128];
    if (choice == 2)
        prompt_for_party_id(new_value, sizeof new_value);
    else if (choice == 3)
        prompt_for_district_id(new_value, sizeof new_value);
    else
        prompt("Enter new value", new_value, sizeof new_value);
    if (!new_value[0])
    {
        printf("Value cannot be empty.\n");
        return;
    }

    if (choice == 4 && candidate_nic_exists(new_value, candidate_number))
    {
        printf("Error: NIC already exists.\n");
        return;
    }

    int rc = update_candidate(candidate_number, choice /*1=name,2=party,3=district,4=nic*/, new_value);
    if (rc == DATA_SUCCESS)
        printf("✅ Candidate Updated Successfully.\n");
    else
        printf("❌ Failed to update candidate. %s\n", get_last_error());
}

// ---------------- Delete ----------------
static void deleteCandidate(void)
{
    char candidate_number[32];
    prompt("Enter Candidate Number to delete", candidate_number, sizeof candidate_number);
    if (!candidate_number[0])
    {
        printf("Candidate Number is required.\n");
        return;
    }
    int rc = delete_candidate(candidate_number);
    if (rc == DATA_SUCCESS)
        printf("✅ Candidate Deleted Successfully.\n");
    else
        printf("❌ Failed to delete candidate or not found. %s\n", get_last_error());
}
