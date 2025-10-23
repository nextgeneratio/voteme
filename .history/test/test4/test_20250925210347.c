#include <stdio.h>
#include "../../src/data_handler.c"

char *read_record(const char *filename, char *primary_keys[], int num_keys);

int main(){
    char *primary_keys[] = {"0:00001"}; // Example  keys
    char *record = read_record("approved_voters.txt", primary_keys, 2);
    if (record) {
        printf("Record found: %s\n", record);
        free(record);
    } else {
        printf("Record not found.\n");
    }
    return 0;
}

