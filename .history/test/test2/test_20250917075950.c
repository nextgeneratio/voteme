#include <stdio.h>

struct test
{
    int id;
    char name[50];
};


int main() {
    struct test t;
    FILE *fp;
    fp = fopen("test.txt", "a");
    if (fp == NULL) {
        fp = fopen("test.txt", "w");
        if (fp == NULL) {
            perror("Error opening file");
            return -1;
    }
    printf("Please enter id and name: ");
    scanf("%d %s", &t.id, t.name);
    fprintf(fp, "ID: %d, Name: %s\n", t.id, t.name);
    fclose(fp);
    return 0;
}
