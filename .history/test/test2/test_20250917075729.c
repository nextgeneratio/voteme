#include <stdio.h>

struct test
{
    int id;
    char name[50];
};


int main() {
    struct test t;
    FILE *fp;
    fp = fopen("test.txt", "A");
    {
        /* data */
    };
    
    return 0;
}
