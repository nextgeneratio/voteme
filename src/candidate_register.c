#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ANSI color codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"

#define FILE_NAME "candidates.txt"

// ---------- Function Declarations ----------
int create_candidate(const char *candidate_number, const char *name, const char *party_id,
                     const char *district_id, const char *nic);
char* read_candidate(const char *candidate_number);
int update_candidate(const char *candidate_number, int field_index, const char *new_value);
int delete_candidate(const char *candidate_number);

// ---------- Function Definitions ----------

// Create candidate
int create_candidate(const char *candidate_number, const char *name, const char *party_id,
                     const char *district_id, const char *nic) {
    FILE *fp;
    char line[300], existingNIC[20];
    int isUnique = 1;

    // Check NIC uniqueness
    fp = fopen(FILE_NAME, "r");
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp) != NULL) {
            sscanf(line, "%*[^,],%*[^,],%*[^,],%*[^,],%[^,\n]", existingNIC);
            if (strcmp(nic, existingNIC) == 0) {
                printf(RED "❌ NIC already registered. Please use a unique NIC.\n" RESET);
                isUnique = 0;
                break;
            }
        }
        fclose(fp);
    }

    if (!isUnique) return 0;

    fp = fopen(FILE_NAME, "a");
    if (fp == NULL) {
        printf(RED "Error opening file for writing!\n" RESET);
        return 0;
    }

    fprintf(fp, "%s,%s,%s,%s,%s\n", candidate_number, name, party_id, district_id, nic);
    fclose(fp);

    printf(GREEN "✅ Candidate created successfully! ID: %s\n" RESET, candidate_number);
    return 1;
}

// Read candidate
char* read_candidate(const char *candidate_number) {
    FILE *fp;
    char line[300];
    char *result = NULL;
    char tempCandidate[20];

    fp = fopen(FILE_NAME, "r");
    if (fp == NULL) {
        printf(RED "Error opening file for reading!\n" RESET);
        return NULL;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        sscanf(line, "%[^,]", tempCandidate);
        if (strcmp(tempCandidate, candidate_number) == 0) {
            result = malloc(strlen(line) + 1);
            strcpy(result, line);
            break;
        }
    }

    fclose(fp);

    if (result == NULL)
        printf(RED "❌ Candidate not found.\n" RESET);
    else
        printf(GREEN "✅ Candidate found: %s" RESET, result);

    return result;
}

// Update candidate
int update_candidate(const char *candidate_number, int field_index, const char *new_value) {
    FILE *fp, *temp;
    char line[300];
    char cand_no[20], name[50], party[20], district[20], nic[20];
    int found = 0;

    fp = fopen(FILE_NAME, "r");
    temp = fopen("temp.txt", "w");
    if (fp == NULL || temp == NULL) {
        printf(RED "Error opening file!\n" RESET);
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,\n]", cand_no, name, party, district, nic);

        if (strcmp(cand_no, candidate_number) == 0) {
            found = 1;
            switch (field_index) {
                case 0: strcpy(cand_no, new_value); break;
                case 1: strcpy(name, new_value); break;
                case 2: strcpy(party, new_value); break;
                case 3: strcpy(district, new_value); break;
                case 4: strcpy(nic, new_value); break;
                default: printf(RED "Invalid field index!\n" RESET); break;
            }
        }

        fprintf(temp, "%s,%s,%s,%s,%s\n", cand_no, name, party, district, nic);
    }

    fclose(fp);
    fclose(temp);

    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    if (found)
        printf(GREEN "✅ Candidate updated successfully!\n" RESET);
    else
        printf(RED "❌ Candidate not found.\n" RESET);

    return found;
}

// Delete candidate
int delete_candidate(const char *candidate_number) {
    FILE *fp, *temp;
    char line[300];
    char cand_no[20];
    int found = 0;

    fp = fopen(FILE_NAME, "r");
    temp = fopen("temp.txt", "w");
    if (fp == NULL || temp == NULL) {
        printf(RED "Error opening file!\n" RESET);
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        sscanf(line, "%[^,]", cand_no);
        if (strcmp(cand_no, candidate_number) != 0)
            fputs(line, temp);
        else
            found = 1;
    }

    fclose(fp);
    fclose(temp);

    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    if (found)
        printf(GREEN "✅ Candidate deleted successfully!\n" RESET);
    else
        printf(RED "❌ Candidate not found.\n" RESET);

    return found;
}

// ---------- Menu ----------
int main() {
    int choice, field_index;
    char input[10];
    char candidate_number[10], name[50], party_id[20], district_id[20], nic[20], new_value[50];
    char *candidate;

    while (1) {
        printf(YELLOW "\n=====================================\n");
        printf("       CANDIDATE MANAGEMENT MENU     \n");
        printf("=====================================\n" RESET);
        printf("1. Create Candidate\n");
        printf("2. Read Candidate\n");
        printf("3. Update Candidate\n");
        printf("4. Delete Candidate\n");
        printf("5. Exit\n");
        printf(YELLOW "Choose an option: " RESET);

        fgets(input, sizeof(input), stdin);
        choice = atoi(input);

        switch (choice) {
            case 1:
                printf(YELLOW "Enter Candidate Number: " RESET);
                fgets(candidate_number, sizeof(candidate_number), stdin);
                candidate_number[strcspn(candidate_number, "\n")] = 0;

                printf(YELLOW "Enter Full Name: " RESET);
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;

                printf(YELLOW "Enter Party ID: " RESET);
                fgets(party_id, sizeof(party_id), stdin);
                party_id[strcspn(party_id, "\n")] = 0;

                printf(YELLOW "Enter District ID: " RESET);
                fgets(district_id, sizeof(district_id), stdin);
                district_id[strcspn(district_id, "\n")] = 0;

                printf(YELLOW "Enter NIC: " RESET);
                fgets(nic, sizeof(nic), stdin);
                nic[strcspn(nic, "\n")] = 0;

                create_candidate(candidate_number, name, party_id, district_id, nic);
                break;

            case 2:
                printf(YELLOW "Enter Candidate Number to read: " RESET);
                fgets(candidate_number, sizeof(candidate_number), stdin);
                candidate_number[strcspn(candidate_number, "\n")] = 0;
                candidate = read_candidate(candidate_number);
                if (candidate) free(candidate);
                break;

            case 3:
                printf(YELLOW "Enter Candidate Number to update: " RESET);
                fgets(candidate_number, sizeof(candidate_number), stdin);
                candidate_number[strcspn(candidate_number, "\n")] = 0;

                printf(YELLOW "Enter field index (0=candidate_no,1=name,2=party_id,3=district_id,4=nic): " RESET);
                fgets(input, sizeof(input), stdin);
                field_index = atoi(input);

                printf(YELLOW "Enter new value: " RESET);
                fgets(new_value, sizeof(new_value), stdin);
                new_value[strcspn(new_value, "\n")] = 0;

                update_candidate(candidate_number, field_index, new_value);
                break;

            case 4:
                printf(YELLOW "Enter Candidate Number to delete: " RESET);
                fgets(candidate_number, sizeof(candidate_number), stdin);
                candidate_number[strcspn(candidate_number, "\n")] = 0;
                delete_candidate(candidate_number);
                break;

            case 5:
                printf(GREEN "\nExiting program. Goodbye!\n" RESET);
                return 0;

            default:
                printf(RED "Invalid choice! Try again.\n" RESET);
        }
    }
}
