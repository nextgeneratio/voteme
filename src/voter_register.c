#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE *fp;
    char name[50], nic[20], address[100];
    int age;
    int voterID = 0;
    char ch, temp[20];
    char line[200];
    int nicExists = 0; // flag for duplicate check

    system("color 0A"); // green text on black background

    printf("\033[1;36m====================================\n");
    printf("       VOTER REGISTRATION SYSTEM     \n");
    printf("=====================================\033[0m\n");

    // Count existing voters
    fp = fopen("voters.txt", "r");
    if (fp != NULL) {
        while ((ch = fgetc(fp)) != EOF) {
            if (ch == '\n')
                voterID++;
        }
        fclose(fp);
    }
    voterID++;

    // --- Get inputs ---
    printf("\033[1;33m\nEnter Full Name: \033[0m");
    scanf(" %[^\n]", name);

    printf("\033[1;33mEnter NIC: \033[0m");
    scanf(" %[^\n]", nic);

    // --- Check NIC already exists ---
    fp = fopen("voters.txt", "r");
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, nic)) {  // check if NIC is found in any line
                nicExists = 1;
                break;
            }
        }
        fclose(fp);
    }

    if (nicExists == 1) {
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
