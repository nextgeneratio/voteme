// Refactored voter_register to use data_handler_enhanced APIs and enforce no-duplicates in approved_voters.txt

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "data_handler_enhanced.h"
#include "data_errors.h"

#define APPROVED_VOTERS_FILE "data/approved_voters.txt"

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

// Function prototypes
static void createVoter(void);
static void readVoters(void);
static void updateVoter(void);
static void deleteVoter(void);

int main(void)
{
    int choice;
    while (1)
    {
        printf("\n====================================\n");
        printf("       VOTER MANAGEMENT SYSTEM       \n");
        printf("====================================\n");
        printf("1. Register New Voter\n");
        printf("2. View All Voters\n");
        printf("3. Update Voter\n");
        printf("4. Delete Voter\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            // clear invalid input
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF)
                ;
            printf("Invalid input.\n");
            continue;
        }
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;

        switch (choice)
        {
        case 1:
            createVoter();
            break;
        case 2:
            readVoters();
            break;
        case 3:
            updateVoter();
            break;
        case 4:
            deleteVoter();
            break;
        case 5:
            printf("Goodbye!\n");
            return 0;
        default:
            printf("Invalid choice! Try again.\n");
        }
    }
}

// ---------------- Create Function ----------------
static void createVoter(void)
{
    char voting_number[64], name[128], nic[64], district_id[64];

    prompt("Enter Voting Number", voting_number, sizeof voting_number);
    if (voting_number[0] == '\0')
    {
        printf("Voting Number cannot be empty.\n");
        return;
    }

    // Ensure voting_number isn't already used
    char *existing = read_voter(voting_number);
    if (existing)
    {
        free(existing);
        printf("Error: Voting Number already exists.\n");
        return;
    }

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
    prompt("Enter District ID", district_id, sizeof district_id);
    if (district_id[0] == '\0')
    {
        printf("District ID cannot be empty.\n");
        return;
    }

    int rc = create_voter(voting_number, name, nic, district_id);
    if (rc == DATA_SUCCESS)
        printf("✅ Voter Registered Successfully.\n");
    else
        printf("❌ Failed to register voter.\n");
}

// ---------------- Read Function ----------------
static void readVoters(void)
{
    FILE *f = fopen(APPROVED_VOTERS_FILE, "r");
    if (!f)
    {
        printf("No voter data found.\n");
        return;
    }
    printf("\n======= APPROVED VOTERS =======\n");
    printf("%-16s | %-20s | %-14s | %-12s\n", "voting_number", "name", "nic", "district_id");
    printf("--------------------------------+----------------------+----------------+-------------\n");
    char line[1024];
    while (fgets(line, sizeof line, f))
    {
        if (strncmp(line, "voting_number,", 14) == 0)
            continue; // skip header
        char *p = line;
        char *fields[4] = {0};
        for (int i = 0; i < 4; ++i)
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
        for (int i = 0; i < 4 && fields[i]; ++i)
            trim(fields[i]);
        if (fields[0] && *fields[0])
            printf("%-16s | %-20s | %-14s | %-12s\n",
                   fields[0], fields[1] ? fields[1] : "",
                   fields[2] ? fields[2] : "",
                   fields[3] ? fields[3] : "");
    }
    fclose(f);
}

// ---------------- Update Function ----------------
static void updateVoter(void)
{
    char voting_number[64];
    prompt("Enter Voting Number to update", voting_number, sizeof voting_number);
    if (voting_number[0] == '\0')
    {
        printf("Voting Number is required.\n");
        return;
    }

    char *current = read_voter(voting_number);
    if (!current)
    {
        printf("Voter not found.\n");
        return;
    }
    printf("Current record: %s\n", current);
    free(current);

    printf("Choose field to update:\n");
    printf("1. Name\n2. NIC\n3. District ID\nEnter choice (1-3): ");
    int choice = 0;
    if (scanf("%d", &choice) != 1)
    {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;
        printf("Invalid input.\n");
        return;
    }
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
    if (choice < 1 || choice > 3)
    {
        printf("Invalid choice.\n");
        return;
    }

    char new_value[128];
    prompt("Enter new value", new_value, sizeof new_value);
    if (new_value[0] == '\0')
    {
        printf("Value cannot be empty.\n");
        return;
    }

    // If updating NIC, ensure uniqueness (excluding this voter)
    if (choice == 2 && nic_exists_in_file(new_value, voting_number))
    {
        printf("Error: NIC already exists.\n");
        return;
    }

    int rc = update_voter(voting_number, choice /* field index aligns: 1=name,2=nic,3=district */, new_value);
    if (rc == DATA_SUCCESS)
        printf("✅ Voter Updated Successfully.\n");
    else
        printf("❌ Failed to update voter.\n");
}

// ---------------- Delete Function ----------------
static void deleteVoter(void)
{
    char voting_number[64];
    prompt("Enter Voting Number to delete", voting_number, sizeof voting_number);
    if (voting_number[0] == '\0')
    {
        printf("Voting Number is required.\n");
        return;
    }
    int rc = delete_voter(voting_number);
    if (rc == DATA_SUCCESS)
        printf("✅ Voter Deleted Successfully.\n");
    else
        printf("❌ Failed to delete voter or voter not found.\n");
}