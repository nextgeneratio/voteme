#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include <stdio.h>

#define MAX_LINE_LENGTH 256

// Generic file operations
int read_csv_line(FILE *fp, char *fields[], int max_fields, char delimiter);
int append_line(const char *filename, const char *line);
int overwrite_file(const char *filename, const char *content);

// Generic CRUD operations
int create_record(const char *filename, const char *record);
char* read_record(const char *filename, char *primary_keys[], int num_keys);
int update_record(const char *filename, char *primary_keys[], int num_keys, const char *field_to_update);
int delete_record(const char *filename, char *primary_keys[], int num_keys);

// File paths
#define CANDIDATES_FILE "../data/approved_candidates.txt"
#define VOTERS_FILE "../data/approved_voters.txt"
#define PARLIAMENT_CANDIDATES_FILE "../data/parliament_candidates.txt"
#define PARTIES_FILE "../data/party_name.txt"
#define VOTER_COUNT_FILE "../data/voter_count.txt"
#define DISTRICT_FILE "../data/district.txt"

/* === Candidate Functions === */
int create_candidate(const char *candidate_number, const char *name, const char *party_id, const char *district_id, const char *nic);
char* read_candidate(const char *candidate_number);
int update_candidate(const char *candidate_number, int field_index, const char *new_value);
int delete_candidate(const char *candidate_number);

/* === Voter Functions === */
int create_voter(const char *voting_number, const char *name, const char *nic, const char *district_id);
char* read_voter(const char *voting_number);
int update_voter(const char *voting_number, int field_index, const char *new_value);
int delete_voter(const char *voting_number);

/* === Party Functions === */
int create_party(const char *party_id, const char *party_name);
char* read_party(const char *party_id);
int update_party(const char *party_id, const char *new_party_name);
int delete_party(const char *party_id);

/* === District Functions === */
int create_district(const char *district_id, const char *district_name);
char* read_district(const char *district_id);
int update_district(const char *district_id, const char *new_district_name);
int delete_district(const char *district_id);

/* === Parliament Candidate Functions === */
int create_parliament_candidate(const char *candidate_number, const char *party_id);
char* read_parliament_candidate(const char *candidate_number, const char *party_id);
int delete_parliament_candidate(const char *candidate_number, const char *party_id);

/* === Voter Count Functions === */
int create_voter_count(const char *voting_number, const char *candidate_number, 
                      const char *party_id, const char *district_id, const char *count);
char* read_voter_count(const char *voting_number, const char *candidate_number, 
                     const char *party_id, const char *district_id);
int update_voter_count(const char *voting_number, const char *candidate_number, 
                     const char *party_id, const char *district_id, const char *new_count);
int delete_voter_count(const char *voting_number, const char *candidate_number, 
                     const char *party_id, const char *district_id);

#endif /* DATA_HANDLER_H */