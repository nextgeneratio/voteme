// Minimal unit test for temp-voted-list CRUD helpers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_handler_enhanced.h"

static void ensure_data_dir(void)
{
    system("mkdir -p data");
}

static int line_count(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (!fp)
        return -1;
    int c, lines = 0;
    while ((c = fgetc(fp)) != EOF)
        if (c == '\n')
            lines++;
    fclose(fp);
    return lines;
}

static void trim(char *s)
{
    if (!s)
        return;
    char *p = s;
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
        p++;
    if (p != s)
        memmove(s, p, strlen(p) + 1);
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == ' ' || s[n - 1] == '\t' || s[n - 1] == '\r' || s[n - 1] == '\n'))
        s[--n] = '\0';
}

static void parse_triplet(const char *rec, char *out_v, char *out_c, char *out_p)
{
    // Expect format: voting_number, candidate_number, party_id (spaces optional)
    const char *a = strchr(rec, ',');
    const char *b = a ? strchr(a + 1, ',') : NULL;
    if (!a || !b)
    {
        out_v[0] = out_c[0] = out_p[0] = '\0';
        return;
    }
    size_t lv = (size_t)(a - rec);
    size_t lc = (size_t)(b - a - 1);
    strncpy(out_v, rec, lv);
    out_v[lv] = '\0';
    trim(out_v);
    strncpy(out_c, a + 1, lc);
    out_c[lc] = '\0';
    trim(out_c);
    strncpy(out_p, b + 1, MAX_LINE_LENGTH - 1);
    out_p[MAX_LINE_LENGTH - 1] = '\0';
    trim(out_p);
}

int main(void)
{
    ensure_data_dir();

    // Start with a clean header-only file
    if (clear_temp_voted() != DATA_SUCCESS)
    {
        fprintf(stderr, "clear_temp_voted failed: %s\n", get_last_error());
        return 1;
    }

    int lc = line_count("data/temp-voted-list.txt");
    if (lc < 1)
    {
        fprintf(stderr, "temp-voted-list missing header (lines=%d)\n", lc);
        return 1;
    }

    // Create
    if (create_temp_voted("V100", "C123", "P01") != DATA_SUCCESS)
    {
        fprintf(stderr, "create_temp_voted failed: %s\n", get_last_error());
        return 1;
    }

    char *rec = read_temp_voted("V100");
    if (!rec)
    {
        fprintf(stderr, "read_temp_voted after create returned NULL: %s\n", get_last_error());
        return 1;
    }
    char v[MAX_LINE_LENGTH], c[MAX_LINE_LENGTH], p[MAX_LINE_LENGTH];
    parse_triplet(rec, v, c, p);
    if (strcmp(v, "V100") != 0 || strcmp(c, "C123") != 0 || strcmp(p, "P01") != 0)
    {
        fprintf(stderr, "Unexpected values after create: '%s' -> v='%s' c='%s' p='%s'\n", rec, v, c, p);
        free(rec);
        return 1;
    }
    free(rec);

    // Update
    if (update_temp_voted("V100", "C777", "P099") != DATA_SUCCESS)
    {
        fprintf(stderr, "update_temp_voted failed: %s\n", get_last_error());
        return 1;
    }
    rec = read_temp_voted("V100");
    if (!rec)
    {
        fprintf(stderr, "read_temp_voted after update returned NULL: %s\n", get_last_error());
        return 1;
    }
    parse_triplet(rec, v, c, p);
    if (strcmp(v, "V100") != 0 || strcmp(c, "C777") != 0 || strcmp(p, "P099") != 0)
    {
        fprintf(stderr, "Unexpected values after update: '%s' -> v='%s' c='%s' p='%s'\n", rec, v, c, p);
        free(rec);
        return 1;
    }
    free(rec);

    // Clear
    if (clear_temp_voted() != DATA_SUCCESS)
    {
        fprintf(stderr, "clear_temp_voted (2) failed: %s\n", get_last_error());
        return 1;
    }
    rec = read_temp_voted("V100");
    if (rec)
    {
        fprintf(stderr, "Expected no record after clear, but got: %s\n", rec);
        free(rec);
        return 1;
    }

    printf("PASS: temp_voted CRUD test.\n");
    return 0;
}
