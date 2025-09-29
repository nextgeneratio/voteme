#include <stdio.h>
#include "../../src/data_handler.h"

int main(){
    char *primary_keys[] = {"0:00001"};
    const char *field_to_update = "1:John";
    int result = update_record("../../data/approved_voters.txt", primary_keys, 1, field_to_update);
    if (result == 1) {
        printf("Record updated successfully.\n");
    } else {
        printf("Failed to update record.\n");
    }
    return 0;
}

