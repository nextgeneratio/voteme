#include <stdio.h>
#include <string.h>

// ANSI color codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"

int main() {
    FILE *fp;
    char name[50], nic[15], party[50], address[100];
    float netWorth, regFee;
    int candidateID = 0;
    char ch;
    int isUnique;

    printf(YELLOW "=====================================\n");
    printf("     CANDIDATE REGISTRATION SYSTEM   \n");
    printf("=====================================\n" RESET);

    // Count existing candidates to assign next ID
    fp = fopen("candidates.txt", "r");
    if (fp != NULL) {
        while ((ch = fgetc(fp)) != EOF) {
            if (ch == '\n')
                candidateID++;
        }
        fclose(fp);
    }
    candidateID++; // next candidate ID

    // Input candidate details
    printf(YELLOW "\nEnter Full Name: " RESET);
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0; // remove newline

    // NIC input with uniqueness check
    do {
        isUnique = 1;
        printf(YELLOW "Enter NIC: " RESET);
        fgets(nic, sizeof(nic), stdin);
        nic[strcspn(nic, "\n")] = 0; // remove newline

        // Check if NIC already exists
        fp = fopen("candidates.txt", "r");
        if (fp != NULL) {
            char line[300], existingNIC[15];
            while (fgets(line, sizeof(line), fp) != NULL) {
                sscanf(line, "%*d,%*[^,],%[^,],%*[^,],%*f,%*[^,],%*f", existingNIC);
                if (strcmp(nic, existingNIC) == 0) {
                    printf(RED "❌ NIC already registered. Enter a unique NIC.\n" RESET);
                    isUnique = 0;
                    break;
                }
            }
            fclose(fp);
        }
    } while (!isUnique);

    printf(YELLOW "Enter Party Name: " RESET);
    fgets(party, sizeof(party), stdin);
    party[strcspn(party, "\n")] = 0;

    printf(YELLOW "Enter Net Worth (LKR): " RESET);
    scanf("%f", &netWorth);
    getchar(); // consume newline

    printf(YELLOW "Enter Permanent Address: " RESET);
    fgets(address, sizeof(address), stdin);
    address[strcspn(address, "\n")] = 0;

    printf(YELLOW "Enter Registration Fee (LKR): " RESET);
    scanf("%f", &regFee);

    // Save candidate to file
    fp = fopen("candidates.txt", "a");
    if (fp == NULL) {
        printf(RED "Error opening file!\n" RESET);
        return 0;
    }

    fprintf(fp, "%d,%s,%s,%s,%.2f,%s,%.2f\n", candidateID, name, nic, party, netWorth, address, regFee);
    fclose(fp);

    printf(GREEN "\n✅ Candidate Registered Successfully!\n" RESET);
    printf(GREEN "Assigned Candidate ID: %d\n" RESET, candidateID);

    return 0;
}
