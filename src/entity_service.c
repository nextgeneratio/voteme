#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "entity_service.h"
#include "data_errors.h"

int create_candidate_struct(const Candidate *c)
{
    if (!c) { set_error_message("Error: NULL candidate"); return DATA_ERROR_INVALID_INPUT; }
    char buf[256];
    if (!format_candidate_line(c, buf, sizeof buf))
    {
        set_error_message("Error: candidate formatting failed");
        return DATA_ERROR_INVALID_INPUT;
    }
    return create_record("data/approved_candidates.txt", buf);
}

int read_candidate_struct(const char *candidate_number, Candidate *out)
{
    if (!candidate_number || !out) { set_error_message("Error: invalid args"); return DATA_ERROR_INVALID_INPUT; }
    char *line = read_candidate(candidate_number);
    if (!line) return DATA_ERROR_RECORD_NOT_FOUND;
    int ok = parse_candidate_line(line, out);
    free(line);
    if (!ok) { set_error_message("Error: parse candidate failed"); return DATA_ERROR_MALFORMED_DATA; }
    return DATA_SUCCESS;
}

int create_voter_struct(const Voter *v)
{
    if (!v) { set_error_message("Error: NULL voter"); return DATA_ERROR_INVALID_INPUT; }
    char buf[256];
    if (!format_voter_line(v, buf, sizeof buf))
    {
        set_error_message("Error: voter formatting failed");
        return DATA_ERROR_INVALID_INPUT;
    }
    return create_record("data/approved_voters.txt", buf);
}

int read_voter_struct(const char *voting_number, Voter *out)
{
    if (!voting_number || !out) { set_error_message("Error: invalid args"); return DATA_ERROR_INVALID_INPUT; }
    char *line = read_voter(voting_number);
    if (!line) return DATA_ERROR_RECORD_NOT_FOUND;
    int ok = parse_voter_line(line, out);
    free(line);
    if (!ok) { set_error_message("Error: parse voter failed"); return DATA_ERROR_MALFORMED_DATA; }
    return DATA_SUCCESS;
}
