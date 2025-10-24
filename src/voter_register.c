#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "voters.txt"

// Function prototypes
void createVoter();
void readVoters();
void updateVoter();
void deleteVoter();

int main() {
    int choice;
    system("color 0A"); // green text

    while (1) {
        printf("\033[1;36m\n====================================\n");
        printf("       VOTER MANAGEMENT SYSTEM       \n");
        printf("====================================\033[0m\n");
        printf("\033[1;33m1. Register New Voter\n");
        printf("2. View All Voters\n");
        printf("3. Update Voter\n");
        printf("4. Delete Voter\n");
        printf("5. Exit\n");
        printf("Enter your choice: \033[0m");
        scanf("%d", &choice);
        getchar(); // consume newline

        switch (choice) {
            case 1: createVoter(); break;
            case 2: readVoters(); break;
            case 3: updateVoter(); break;
            case 4: deleteVoter(); break;
            case 5: 
                printf("\033[1;32mExiting Program... Goodbye!\033[0m\n");
                return 0;
            default: 
                printf("\033[1;31mInvalid choice! Try again.\033[0m\n");
        }
    }
}

// ---------------- Create Function ----------------
void createVoter() {
    FILE *fp;
    char name[50], nic[20], address[100];
    int age, voterID = 0;
    char ch, temp[20], line[200];
    int nicExists = 0;

    // Count existing voters
    fp = fopen(FILE_NAME, "r");
    if (fp != NULL) {
        while ((ch = fgetc(fp)) != EOF) {
            if (ch == '\n')
                voterID++;
        }
        fclose(fp);
    }
    voterID++;

    printf("\n\033[1;33mEnter Full Name: \033[0m");
    scanf(" %[^\n]", name);

    printf("\033[1;33mEnter NIC: \033[0m");
    scanf(" %[^\n]", nic);

    // Check for duplicate NIC
    fp = fopen(FILE_NAME, "r");
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, nic)) {
                nicExists = 1;
                break;
            }
        }
        fclose(fp);
    }

    if (nicExists) {
        printf("\033[1;31m⚠️ Error: NIC already exists!\033[0m\n");
        return;
    }

    printf("\033[1;33mEnter Age: \033[0m");
    scanf("%d", &age);

    printf("\033[1;33mEnter Address: \033[0m");
    scanf(" %[^\n]", address);

    fp = fopen(FILE_NAME, "a");
    if (fp == NULL) {
        printf("\033[1;31mFile cannot be opened!\033[0m\n");
        return;
    }

    fprintf(fp, "Voter ID: %d\nName: %s\nNIC: %s\nAge: %d\nAddress: %s\n------------------------\n",
            voterID, name, nic, age, address);
    fclose(fp);

    printf("\033[1;32m✅ Voter Registered Successfully!\033[0m\n");
}

// ---------------- Read Function ----------------
void readVoters() {
    FILE *fp;
    char line[200];
    fp = fopen(FILE_NAME, "r");
    if (fp == NULL) {
        printf("\033[1;31mNo voter data found!\033[0m\n");
        return;
    }

    printf("\033[1;36m\n======= VOTER LIST =======\n\033[0m");
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);
}

// ---------------- Update Function ----------------
void updateVoter() {
    FILE *fp, *tempFp;
    char nic[20], line[200];
    char name[50], address[100];
    int age, found = 0;
    char currentNIC[20];

    printf("\033[1;33mEnter NIC to update: \033[0m");
    scanf(" %[^\n]", nic);

    fp = fopen(FILE_NAME, "r");
    tempFp = fopen("temp.txt", "w");

    if (fp == NULL) {
        printf("\033[1;31mNo file found!\033[0m\n");
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, nic)) {
            found = 1;
            printf("\033[1;33mEnter New Name: \033[0m");
            scanf(" %[^\n]", name);
            printf("\033[1;33mEnter New Age: \033[0m");
            scanf("%d", &age);
            printf("\033[1;33mEnter New Address: \033[0m");
            scanf(" %[^\n]", address);

            fprintf(tempFp, "Voter ID: (Updated)\nName: %s\nNIC: %s\nAge: %d\nAddress: %s\n------------------------\n",
                    name, nic, age, address);
            // Skip old data
            while (fgets(line, sizeof(line), fp)) {
                if (strstr(line, "------------------------")) break;
            }
        } else {
            fputs(line, tempFp);
        }
    }

    fclose(fp);
    fclose(tempFp);
    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    if (found)
        printf("\033[1;32m✅ Voter Updated Successfully!\033[0m\n");
    else
        printf("\033[1;31m❌ NIC not found!\033[0m\n");
}

// ---------------- Delete Function ----------------
void deleteVoter() {
    FILE *fp, *tempFp;
    char nic[20], line[200];
    int found = 0;

    printf("\033[1;33mEnter NIC to delete: \033[0m");
    scanf(" %[^\n]", nic);

    fp = fopen(FILE_NAME, "r");
    tempFp = fopen("temp.txt", "w");

    if (fp == NULL) {
        printf("\033[1;31mNo file found!\033[0m\n");
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, nic)) {
            found = 1;
            // Skip deleting lines until separator
            while (fgets(line, sizeof(line), fp)) {
                if (strstr(line, "------------------------")) break;
            }
        } else {
            fputs(line, tempFp);
        }
    }

    fclose(fp);
    fclose(tempFp);
    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    if (found)
        printf("\033[1;32m✅ Voter Deleted Successfully!\033[0m\n");
    else
        printf("\033[1;31m❌ NIC not found!\033[0m\n");
}