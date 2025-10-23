/*
 * VoteMe Voting Algorithm Header
 *
 * This header defines the interface for the voting algorithm system
 * that processes votes according to admin-defined parameters.
 */

#ifndef VOTING_H
#define VOTING_H

/**
 * Execute the main voting algorithm
 *
 * This function processes all votes according to the specified parameters:
 * 1. Reads all candidate votes from the votes file
 * 2. Applies minimum vote requirements for parliament eligibility
 * 3. Selects top candidates based on maximum parliament seats
 * 4. Generates comprehensive results report
 * 5. Saves results to file
 *
 * @param min_votes_required Minimum votes needed for parliament eligibility
 * @param max_parliament_members Maximum number of parliament seats available
 * @return DATA_SUCCESS on success, error code on failure
 */
int execute_voting_algorithm(int min_votes_required, int max_parliament_members);

/**
 * Create a sample votes file for testing purposes
 *
 * This function creates a basic votes file with sample data if one doesn't exist.
 * Used for testing the voting algorithm when no actual votes have been cast.
 *
 * @return DATA_SUCCESS on success, error code on failure
 */
int create_sample_votes_file(void);

/**
 * Display the current voting algorithm parameters
 *
 * Shows the minimum votes required and maximum parliament seats
 * in a formatted display for user confirmation.
 *
 * @param min_votes Minimum votes required for parliament
 * @param max_parliament_seats Maximum parliament seats available
 */
void display_voting_parameters(int min_votes, int max_parliament_seats);

#endif // VOTING_H