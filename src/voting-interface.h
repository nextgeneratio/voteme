#ifndef VOTING_INTERFACE_H
#define VOTING_INTERFACE_H

// Interactive voting flow:
// 1) Prompt voter id and validate against approved voters
// 2) Show party list and prompt for a valid party id
// 3) Show candidates filtered by party and prompt for candidate id
// 4) Append the vote as "voter_id,candidate_id" to data/votes.txt
//
// Returns 0 on success, negative error code (from data_errors.h) on failure.
int vote_for_candidate_interactive(void);

#endif // VOTING_INTERFACE_H
