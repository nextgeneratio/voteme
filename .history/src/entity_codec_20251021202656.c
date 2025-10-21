#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "entity_codec.h"

static void trim(char *s)
{
    if (!s)
        return;
    char *start = s;
    while (*start && isspace((unsigned char)*start))
        start++;
    char *end = s + strlen(s);
    while (end > start && isspace((unsigned char)*(end - 1)))
        end--;
    size_t len = (size_t)(end - start);
    if (start != s)
        memmove(s, start, len);
    s[len] = '\0';
}

static int split_csv_line(const char *line, char *fields[], int max_fields)
{
    if (!line || !fields || max_fields <= 0)
        return 0;
    char *copy = malloc(strlen(line) + 1);
    if (!copy)
        return 0;
    strcpy(copy, line);
    int count = 0;
    char *token = strtok(copy, ",");
    while (token && count < max_fields)
    {
        trim(token);
        fields[count++] = token;
        token = strtok(NULL, ",");
    }

    // Copy back into independent buffers to free the temp copy
    int ok = 1;
    for (int i = 0; i < count; i++)
    {
        size_t len = strlen(fields[i]);
        char *dup = malloc(len + 1);
        if (!dup)
        {
            ok = 0;
            break;
        }
        memcpy(dup, fields[i], len + 1);
        fields[i] = dup;
    }
    free(copy);
    if (!ok)
    {
        for (int i = 0; i < count; i++)
            free(fields[i]);
        return 0;
    }
    return count;
}

static int check_fit(const char *src, size_t limit)
{
    if (!src)
        return 0;
    return strlen(src) < limit; // must be strictly less to fit NUL in fixed arrays
}

int parse_candidate_line(const char *line, Candidate *out)
{
    if (!line || !out)
        return 0;
    char *fields[5];
    int n = split_csv_line(line, fields, 5);
    if (n != 5)
    {
        for (int i = 0; i < n; i++)
            free(fields[i]);
        return 0;
    }
    int ok = check_fit(fields[0], sizeof out->candidate_number) &&
             check_fit(fields[1], sizeof out->name) &&
             check_fit(fields[2], sizeof out->party_id) &&
             check_fit(fields[3], sizeof out->district_id) &&
             check_fit(fields[4], sizeof out->nic);
    if (ok)
    {
        strcpy(out->candidate_number, fields[0]);
        strcpy(out->name, fields[1]);
        strcpy(out->party_id, fields[2]);
        strcpy(out->district_id, fields[3]);
        strcpy(out->nic, fields[4]);
    }
    for (int i = 0; i < n; i++)
        free(fields[i]);
    return ok;
}

int format_candidate_line(const Candidate *in, char *buf, size_t bufsize)
{
    if (!in || !buf || bufsize == 0)
        return 0;
    int r = snprintf(buf, bufsize, "%s, %s, %s, %s, %s",
                     in->candidate_number, in->name, in->party_id, in->district_id, in->nic);
    return r > 0 && (size_t)r < bufsize;
}

int parse_voter_line(const char *line, Voter *out)
{
    if (!line || !out)
        return 0;
    char *fields[4];
    int n = split_csv_line(line, fields, 4);
    if (n != 4)
    {
        for (int i = 0; i < n; i++)
            free(fields[i]);
        return 0;
    }
    int ok = check_fit(fields[0], sizeof out->voting_number) &&
             check_fit(fields[1], sizeof out->name) &&
             check_fit(fields[2], sizeof out->nic) &&
             check_fit(fields[3], sizeof out->district_id);
    if (ok)
    {
        strcpy(out->voting_number, fields[0]);
        strcpy(out->name, fields[1]);
        strcpy(out->nic, fields[2]);
        strcpy(out->district_id, fields[3]);
    }
    for (int i = 0; i < n; i++)
        free(fields[i]);
    return ok;
}

int format_voter_line(const Voter *in, char *buf, size_t bufsize)
{
    if (!in || !buf || bufsize == 0)
        return 0;
    int r = snprintf(buf, bufsize, "%s, %s, %s, %s",
                     in->voting_number, in->name, in->nic, in->district_id);
    return r > 0 && (size_t)r < bufsize;
}
