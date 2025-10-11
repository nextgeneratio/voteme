#include <stdio.h>
#include <string.h>
#include "voter_register.h"
#include "data_handler_enhanced.h"

int add_voter(const char *voting_number, const char *nic, const char *name) {
    // Use enhanced data handler for consistency
    return create_voter(voting_number, name, nic, "D01"); // Default district
}

int view_voters(void) {
    FILE *file = fopen("data/approved_voters.txt", "r");
    if (!file) {
        printf("❌ No voters file found!\n");
        return -1;
    }
    
    printf("📋 Registered Voters:\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("%-12s %-20s %-15s %-10s\n", "Voting ID", "Name", "NIC", "District");
    printf("───────────────────────────────────────────────────────────────────────────────\n");
    
    char line[256];
    int count = 0;
    
    // Skip header line
    if (fgets(line, sizeof(line), file)) {
        while (fgets(line, sizeof(line), file)) {
            char voting_number[20], name[100], nic[20], district[20];
            if (sscanf(line, "%19[^,],%99[^,],%19[^,],%19s", 
                      voting_number, name, nic, district) == 4) {
                printf("%-12s %-20s %-15s %-10s\n", voting_number, name, nic, district);
                count++;
            }
        }
    }
    
    if (count == 0) {
        printf("No voters registered yet.\n");
    } else {
        printf("───────────────────────────────────────────────────────────────────────────────\n");
        printf("Total voters: %d\n", count);
    }
    
    fclose(file);
    return 0;
}
        printf("\n\033[1;31m⚠️  Error: This NIC is already registered!\033[0m\n");
        printf("Registration cancelled.\n");
        return 0; // stop program
    }

    // --- Continue registration ---
    printf("\033[1;33mEnter Age: \033[0m");
    scanf("%d", &age);

    printf("\033[1;33mEnter Permanent Address: \033[0m");
    scanf(" %[^\n]", address);

    fp = fopen("voters.txt", "a");
    if (fp == NULL) {
        printf("\033[1;31mFile cannot be opened!\033[0m\n");
        return 0;
    }

    // --- Write new voter to file ---
    fputs("Voter ID: ", fp);
    snprintf(temp, sizeof(temp), "%d", voterID);
    fputs(temp, fp);

    fputs("\nName: ", fp);
    fputs(name, fp);

    fputs("\nNIC: ", fp);
    fputs(nic, fp);

    fputs("\nAge: ", fp);
    snprintf(temp, sizeof(temp), "%d", age);
    fputs(temp, fp);

    fputs("\nAddress: ", fp);
    fputs(address, fp);
    fputs("\n------------------------\n", fp);

    fclose(fp);

    printf("\n\033[1;32m✅ Voter Registered Successfully!\033[0m\n");
    printf("Assigned Voter ID: %d\n", voterID);

    return 0;
}
