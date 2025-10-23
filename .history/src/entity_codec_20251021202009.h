#ifndef ENTITY_CODEC_H
#define ENTITY_CODEC_H

#include <stddef.h>
#include "models.h"

// Candidate parse/format
int parse_candidate_line(const char *line, Candidate *out);
int format_candidate_line(const Candidate *in, char *buf, size_t bufsize);

// Voter parse/format
int parse_voter_line(const char *line, Voter *out);
int format_voter_line(const Voter *in, char *buf, size_t bufsize);

#endif // ENTITY_CODEC_H
