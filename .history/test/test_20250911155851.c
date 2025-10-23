#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "data.txt"   // File where data is stored
#define TEMPFILE "temp.txt"   // Temporary file for update/delete
#define MAX_NAME 100          // Max length of name

// Struct to hold record data
typedef struct {
    int id;
    char name[MAX_NAME];
    int age;
} Record;

// Function declarations
void createRecord();
void readRecords();
void updateRecord();
void deleteRecord();

int main() {
    int choice;

    while (1) {
        printf("\n--- CRUD OPERATIONS ---\n");
        printf("1. Create Record\n");
        printf("2. Read Records\n");
        printf("3. Update Record\n");
        printf("4. Delete Record\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);  // Taking user's menu choice

        switch (choice) {
            case 1: createRecord(); break;
            case 2: readRecords(); break;
            case 3: updateRecord(); break;
            case 4: deleteRecord(); break;
            case 5: exit(0);  // Exit the program
            default: printf("Invalid choice.\n"); break;
        }
    }

    return 0;
}

// CREATE operation: Adds a new record to the file
void createRecord() {
    FILE *fp = fopen(FILENAME, "a");  // Open file in append mode
    if (fp == NULL) {
        printf("Error opening file.\n");
        return;
    }

    Record r;
    printf("Enter ID: ");
    scanf("%d", &r.id);
    printf("Enter Name: ");
    scanf(" %[^\n]", r.name);  // Read string with spaces
    printf("Enter Age: ");
    scanf("%d", &r.age);

    // Write record to file in format: ID Name Age
    fprintf(fp, "%d %s %d\n", r.id, r.name, r.age);
    fclose(fp);
    printf("Record added successfully.\n");
}

// READ operation: Displays all records from the file
void readRecords() {
    FILE *fp = fopen(FILENAME, "r");  // Open file in read mode
    if (fp == NULL) {
        printf("No records found.\n");
        return;
    }

    Record r;
    printf("\n--- Records ---\n");
    while (fscanf(fp, "%d %s %d", &r.id, r.name, &r.age) != EOF) {
        printf("ID: %d, Name: %s, Age: %d\n", r.id, r.name, r.age);
    }
    fclose(fp);
}

// UPDATE operation: Updates a record by ID
void updateRecord() {
    FILE *fp = fopen(FILENAME, "r");
    FILE *temp = fopen(TEMPFILE, "w");  // Temporary file to store new data

    if (fp == NULL || temp == NULL) {
        printf("Error opening file.\n");
        return;
    }

    int targetId;
    Record r;
    int found = 0;

    printf("Enter ID to update: ");
    scanf("%d", &targetId);

    while (fscanf(fp, "%d %s %d", &r.id, r.name, &r.age) != EOF) {
        if (r.id == targetId) {
            // Get new data from user
            printf("Enter new name: ");
            scanf(" %[^\n]", r.name);
            printf("Enter new age: ");
            scanf("%d", &r.age);
            found = 1;
        }
        // Write record to temp file (updated if matched)
        fprintf(temp, "%d %s %d\n", r.id, r.name, r.age);
    }

    fclose(fp);
    fclose(temp);

    remove(FILENAME);               // Delete original file
    rename(TEMPFILE, FILENAME);    // Rename temp to original

    if (found)
        printf("Record updated successfully.\n");
    else
        printf("Record with ID %d not found.\n", targetId);
}

// DELETE operation: Deletes a record by ID
void deleteRecord() {
    FILE *fp = fopen(FILENAME, "r");
    FILE *temp = fopen(TEMPFILE, "w");

    if (fp == NULL || temp == NULL) {
        printf("Error opening file.\n");
        return;
    }

    int targetId;
    Record r;
    int found = 0;

    printf("Enter ID to delete: ");
    scanf("%d", &targetId);

    while (fscanf(fp, "%d %s %d", &r.id, r.name, &r.age) != EOF) {
        if (r.id == targetId) {
            found = 1;  // Skip writing this record
            continue;
        }
        fprintf(temp, "%d %s %d\n", r.id, r.name, r.age);
    }

    fclose(fp);
    fclose(temp);

    remove(FILENAME);
    rename(TEMPFILE, FILENAME);

    if (found)
        printf("Record deleted successfully.\n");
    else
        printf("Record with ID %d not found.\n", targetId);
}
