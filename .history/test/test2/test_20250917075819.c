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

    
    
    return 0;
}
