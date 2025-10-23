#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/data_handler.h"

int main(){
    // Print the file path to verify we're accessing the correct file
    printf("Testing update on approved_voters.txt\n");
    
    // First read the current record
    char *primary_keys[] = {"0:00001"};
    char *record = read_record("../../data/approved_voters.txt", primary_keys, 1);
    
    if (record) {
        printf("Current record: %s\n", record);
        free(record);
    } else {
        printf("Record not found.\n");
        return 1;
    }
    
    // Now update the record - changing name from Mike to John
    const char *field_to_update = "1:John";
    int result = update_record("../../data/approved_voters.txt", primary_keys, 1, field_to_update);
    
    if (result == 1) {
        printf("Record updated successfully.\n");
        
        // Verify the update
        record = read_record("../../data/approved_voters.txt", primary_keys, 1);
        if (record) {
            printf("Updated record: %s\n", record);
            free(record);
        } else {
            printf("Failed to read updated record.\n");
        }
    } else {
        printf("Failed to update record.\n");
    }
    
    return 0;
}

