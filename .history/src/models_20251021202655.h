#ifndef MODELS_H
#define MODELS_H

#include <stddef.h>

// Keep lengths aligned with current validations

typedef struct
{
    char candidate_number[50];
    char name[100];
    char party_id[20];
    char district_id[20];
    char nic[20];
} Candidate;

typedef struct
{
    char voting_number[50];
    char name[100];
    char nic[20];
    char district_id[20];
} Voter;

typedef struct
{
    char party_id[20];
    char party_name[100];
} Party;

typedef struct
{
    char district_id[20];
    char district_name[100];
} District;

typedef struct
{
    char candidate_number[50];
    char party_id[20];
} ParliamentCandidate;

typedef struct
{
    char voting_number[50];
    char candidate_number[50];
    char party_id[20];
    char district_id[20];
    int count; // parsed as integer
} VoterCount;

#endif // MODELS_H
