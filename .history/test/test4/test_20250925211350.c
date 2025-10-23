#include <stdio.h>
#include "../../src/data_handler.c"

int update_record(const char *filename, char *primary_keys[], int num_keys, const char *field_to_update);

int main(){
    char *primary_keys[] = {"0:00001"};
    const char *field_to_update = "name:John";
    int result = update_record("approved_voters.txt", primary_keys, 1, field_to_update);
    if (result == 0) {
        printf("Record updated successfully.\n");
    } else {
        printf("Failed to update record.\n");
    }
    return 0;
}

