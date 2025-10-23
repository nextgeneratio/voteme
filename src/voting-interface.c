#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_handler_enhanced.h"

int main(void) {
    const char votenumber[] = "V0001";

    /* call read_voter with the required argument */
    char *vote_output = read_voter(votenumber);
    if (vote_output != NULL) {
        printf("read_voter(%s) -> %s\n", votenumber, vote_output);
    } else {
        printf("read_voter(%s) returned NULL\n", votenumber);
    }

    /* simple pointer example */
    int x = 42;
    int *ip = &x;    /* ip points to x */
    *ip = 10;        /* write through pointer */
    (void)ip;        /* silence unused-var warning */

    /* dynamic allocation example */
    char *buf = malloc(128);
    if (buf) {
        strcpy(buf, "some text");
        printf("buf: %s\n", buf);
        free(buf);
    }

    /* pointer-to-pointer example */
    char *s = "hello";
    char **pp = &s; /* pp points to s */
    (void)pp;

    return 0;
}