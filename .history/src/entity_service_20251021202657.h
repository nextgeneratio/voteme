#ifndef ENTITY_SERVICE_H
#define ENTITY_SERVICE_H

#include "models.h"
#include "data_handler_enhanced.h"
#include "entity_codec.h"

// Candidate wrappers
int create_candidate_struct(const Candidate *c);
int read_candidate_struct(const char *candidate_number, Candidate *out);

// Voter wrappers
int create_voter_struct(const Voter *v);
int read_voter_struct(const char *voting_number, Voter *out);

#endif // ENTITY_SERVICE_H