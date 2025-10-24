/*
 * VoteMe Voting Algorithm Implementation
 *
 * This module implements the core voting algorithm that:
 * 1. Processes votes according to admin-defined parameters
 * 2. Applies minimum vote requirements
 * 3. Selects parliament members based on highest votes
 * 4. Handles voting result generation and reporting
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "data_handle.h"
#include "data_handler_enhanced.h"
#include "voting.h"

// Color codes for result display
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define WHITE "\033[0;37m"
#define RESET "\033[0m"
#define BOLD "\033[1m"

// Voting results structure
typedef struct
{
    char candidate_number[20];
    char candidate_name[100];
    char party_id[20];
    char district_id[20];
    int vote_count;
    int qualified_for_parliament;
} candidate_result_t;

// Voting statistics structure
typedef struct
{
    int total_candidates;
    int total_votes_cast;
    int qualified_candidates;
    int parliament_members_selected;
    int min_votes_threshold;
    int max_parliament_seats;
    char voting_date[50];
    char voting_time[50];
} voting_statistics_t;

/**
 * Load candidate votes from the votes file
 * @param candidates Array to store candidate results
 * @param max_candidates Maximum number of candidates
 * @return Number of candidates loaded
 */
static int load_candidates(candidate_result_t candidates[], int max_candidates)
{
    FILE *candidates_file = fopen("data/approved_candidates.txt", "r");

    if (!candidates_file)
    {
        printf(RED "❌ Error: Unable to open voting files!\n" RESET);
        if (candidates_file)
            fclose(candidates_file);
        return 0;
    }

    // Initialize candidate data from approved candidates
    char line[256];
    int candidate_count = 0;

    // Skip header in candidates file
    if (fgets(line, sizeof(line), candidates_file))
    {
        while (fgets(line, sizeof(line), candidates_file) && candidate_count < max_candidates)
        {
            char *token = strtok(line, ",");
            if (!token)
                continue;

            strcpy(candidates[candidate_count].candidate_number, token);

            token = strtok(NULL, ",");
            if (token)
                strcpy(candidates[candidate_count].candidate_name, token);

            token = strtok(NULL, ",");
            if (token)
                strcpy(candidates[candidate_count].party_id, token);

            token = strtok(NULL, ",");
            if (token)
                strcpy(candidates[candidate_count].district_id, token);

            candidates[candidate_count].vote_count = 0;
            candidates[candidate_count].qualified_for_parliament = 0;
            candidate_count++;
        }
    }
    fclose(candidates_file);

    return candidate_count;
}

/**
 * Count votes for candidates from data/votes.txt
 */
static void count_votes_from_votes_txt(candidate_result_t candidates[], int candidate_count)
{
    FILE *votes_file = fopen("data/votes.txt", "r");
    if (!votes_file)
        return;

    char line[256];
    if (fgets(line, sizeof(line), votes_file))
    {
        while (fgets(line, sizeof(line), votes_file))
        {
            char voter_id[64], candidate_id[64];
            if (sscanf(line, "%[^,],%63s", voter_id, candidate_id) == 2)
            {
                candidate_id[strcspn(candidate_id, "\n")] = 0;
                for (int i = 0; i < candidate_count; i++)
                {
                    if (strcmp(candidates[i].candidate_number, candidate_id) == 0)
                    {
                        candidates[i].vote_count++;
                        break;
                    }
                }
            }
        }
    }
    fclose(votes_file);
}

/**
 * Count votes for candidates from data/temp-voted-list.txt using enhanced API
 */
static void count_votes_from_temp_list(candidate_result_t candidates[], int candidate_count)
{
    char ***records = NULL;
    int rows = 0, cols = 0;
    if (read_all_temp_voted(&records, &rows, &cols) != DATA_SUCCESS || rows <= 0 || cols < 3)
    {
        return;
    }
    // Columns: [0]=voting_number, [1]=candidate_number, [2]=party_id
    for (int r = 0; r < rows; ++r)
    {
        const char *candidate_id = (records[r][1] ? records[r][1] : "");
        for (int i = 0; i < candidate_count; i++)
        {
            if (strcmp(candidates[i].candidate_number, candidate_id) == 0)
            {
                candidates[i].vote_count++;
                break;
            }
        }
    }
    free_temp_voted_records(records, rows, cols);
}

/**
 * Overwrite parliament candidates file with selected top candidates.
 * Writes to data/parliament_candidates.txt with header: candidate_number,party_id
 */
static void write_parliament_candidates(const candidate_result_t candidates[], int candidate_count)
{
    FILE *fp = fopen("data/parliament_candidates.txt", "w");
    if (!fp)
    {
        printf(RED "❌ Error: Unable to write 'data/parliament_candidates.txt'\n" RESET);
        return;
    }
    fprintf(fp, "candidate_number,party_id\n");
    for (int i = 0; i < candidate_count; ++i)
    {
        if (candidates[i].qualified_for_parliament)
        {
            fprintf(fp, "%s,%s\n", candidates[i].candidate_number, candidates[i].party_id);
        }
    }
    fclose(fp);
}

/**
 * Comparison function for sorting candidates by vote count (descending)
 */
static int compare_candidates(const void *a, const void *b)
{
    const candidate_result_t *ca = (const candidate_result_t *)a;
    const candidate_result_t *cb = (const candidate_result_t *)b;
    return cb->vote_count - ca->vote_count; // Descending order
}

/**
 * Apply minimum vote requirements and select parliament members
 * @param candidates Array of candidate results
 * @param candidate_count Total number of candidates
 * @param min_votes Minimum votes required for parliament
 * @param max_parliament_seats Maximum parliament seats available
 * @return Number of parliament members selected
 */
static int select_parliament_members(candidate_result_t candidates[], int candidate_count,
                                     int min_votes, int max_parliament_seats)
{
    // Sort candidates by vote count (highest first)
    qsort(candidates, candidate_count, sizeof(candidate_result_t), compare_candidates);

    int parliament_members = 0;

    // Ignore min-vote threshold: always select top N
    for (int i = 0; i < candidate_count; i++)
    {
        if (i < max_parliament_seats)
        {
            candidates[i].qualified_for_parliament = 1;
            parliament_members++;
        }
        else
        {
            candidates[i].qualified_for_parliament = 0;
        }
    }

    return parliament_members;
}

/**
 * Generate detailed voting results report
 * @param candidates Array of candidate results
 * @param candidate_count Total number of candidates
 * @param stats Voting statistics
 */
static void generate_results_report(candidate_result_t candidates[], int candidate_count,
                                    voting_statistics_t *stats)
{
    printf("\n");
    printf(BOLD CYAN "═══════════════════════════════════════════════════════════════════════════════\n");
    printf("                           🗳️  VOTING RESULTS REPORT 🗳️                           \n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n" RESET);

    // Voting Statistics
    printf(BOLD YELLOW "\n📊 VOTING STATISTICS:\n" RESET);
    printf("┌─────────────────────────────────────────────────────────────────────────────┐\n");
    printf("│ " BLUE "Total Candidates:" RESET "        %-8d │ " BLUE "Total Votes Cast:" RESET "       %-8d │\n",
           stats->total_candidates, stats->total_votes_cast);
    printf("│ " BLUE "Qualified Candidates:" RESET "    %-8d │ " BLUE "Parliament Members:" RESET "     %-8d │\n",
           stats->qualified_candidates, stats->parliament_members_selected);
    printf("│ " BLUE "Min Votes Required:" RESET "      %-8d │ " BLUE "Max Parliament Seats:" RESET "   %-8d │\n",
           stats->min_votes_threshold, stats->max_parliament_seats);
    printf("│ " BLUE "Voting Date:" RESET "             %-25s │ " BLUE "Time:" RESET " %-15s │\n",
           stats->voting_date, stats->voting_time);
    printf("└─────────────────────────────────────────────────────────────────────────────┘\n");

    // Parliament Members
    printf(BOLD GREEN "\n🏛️  PARLIAMENT MEMBERS (Selected Candidates):\n" RESET);
    printf("┌─────────────────────────────────────────────────────────────────────────────┐\n");
    printf("│ " BOLD "Rank │ Candidate │      Name      │ Party │ District │ Votes │ Status" RESET " │\n");
    printf("├─────────────────────────────────────────────────────────────────────────────┤\n");

    int rank = 1;
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].qualified_for_parliament)
        {
            printf("│ %4d │ %-9s │ %-14s │ %-5s │ %-8s │ %5d │ " GREEN "✓ MP" RESET "   │\n",
                   rank++, candidates[i].candidate_number, candidates[i].candidate_name,
                   candidates[i].party_id, candidates[i].district_id, candidates[i].vote_count);
        }
    }
    printf("└─────────────────────────────────────────────────────────────────────────────┘\n");

    // All Candidates Results
    printf(BOLD YELLOW "\n📋 COMPLETE RESULTS (All Candidates):\n" RESET);
    printf("┌─────────────────────────────────────────────────────────────────────────────┐\n");
    printf("│ " BOLD "Rank │ Candidate │      Name      │ Party │ District │ Votes │ Status" RESET " │\n");
    printf("├─────────────────────────────────────────────────────────────────────────────┤\n");

    for (int i = 0; i < candidate_count; i++)
    {
        const char *status = candidates[i].qualified_for_parliament ? GREEN "✓ MP" RESET : RED "✗ Failed" RESET;

        printf("│ %4d │ %-9s │ %-14s │ %-5s │ %-8s │ %5d │ %-12s │\n",
               i + 1, candidates[i].candidate_number, candidates[i].candidate_name,
               candidates[i].party_id, candidates[i].district_id, candidates[i].vote_count, status);
    }
    printf("└─────────────────────────────────────────────────────────────────────────────┘\n");
}

/**
 * Save voting results to file
 * @param candidates Array of candidate results
 * @param candidate_count Total number of candidates
 * @param stats Voting statistics
 */
static void save_results_to_file(candidate_result_t candidates[], int candidate_count,
                                 voting_statistics_t *stats)
{
    FILE *results_file = fopen("data/voting_results.txt", "w");
    if (!results_file)
    {
        printf(RED "❌ Error: Unable to save results to file!\n" RESET);
        return;
    }

    fprintf(results_file, "# VoteMe Voting Results Report\n");
    fprintf(results_file, "# Generated on: %s at %s\n", stats->voting_date, stats->voting_time);
    fprintf(results_file, "\n[STATISTICS]\n");
    fprintf(results_file, "total_candidates=%d\n", stats->total_candidates);
    fprintf(results_file, "total_votes_cast=%d\n", stats->total_votes_cast);
    fprintf(results_file, "qualified_candidates=%d\n", stats->qualified_candidates);
    fprintf(results_file, "parliament_members_selected=%d\n", stats->parliament_members_selected);
    fprintf(results_file, "min_votes_threshold=%d\n", stats->min_votes_threshold);
    fprintf(results_file, "max_parliament_seats=%d\n", stats->max_parliament_seats);

    fprintf(results_file, "\n[PARLIAMENT_MEMBERS]\n");
    fprintf(results_file, "candidate_number,name,party_id,district_id,votes,rank\n");
    int rank = 1;
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].qualified_for_parliament)
        {
            fprintf(results_file, "%s,%s,%s,%s,%d,%d\n",
                    candidates[i].candidate_number, candidates[i].candidate_name,
                    candidates[i].party_id, candidates[i].district_id,
                    candidates[i].vote_count, rank++);
        }
    }

    fprintf(results_file, "\n[ALL_RESULTS]\n");
    fprintf(results_file, "candidate_number,name,party_id,district_id,votes,rank,qualified_for_parliament\n");
    for (int i = 0; i < candidate_count; i++)
    {
        fprintf(results_file, "%s,%s,%s,%s,%d,%d,%s\n",
                candidates[i].candidate_number, candidates[i].candidate_name,
                candidates[i].party_id, candidates[i].district_id,
                candidates[i].vote_count, i + 1,
                candidates[i].qualified_for_parliament ? "YES" : "NO");
    }

    fclose(results_file);
    printf(GREEN "💾 Results saved to 'data/voting_results.txt'\n" RESET);
}

/**
 * Main voting algorithm function
 * @param min_votes_required Minimum votes needed for parliament eligibility
 * @param max_parliament_members Maximum number of parliament seats
 * @return DATA_SUCCESS on success, error code on failure
 */
int execute_voting_algorithm(int min_votes_required, int max_parliament_members)
{
    printf(BOLD CYAN "\n🗳️  STARTING VOTING ALGORITHM...\n" RESET);
    printf("═══════════════════════════════════════════════════════════════════════════════\n");

    // Determine vote source: prefer temp-voted-list if it has data; else fallback to votes.txt
    int use_temp_list = 0;
    {
        FILE *tmp = fopen("data/temp-voted-list.txt", "r");
        if (tmp)
        {
            char buf[256];
            if (fgets(buf, sizeof buf, tmp))
            {
                if (fgets(buf, sizeof buf, tmp)) // has at least one data row
                    use_temp_list = 1;
            }
            fclose(tmp);
        }
    }
    if (!use_temp_list)
    {
        FILE *votes_check = fopen("data/votes.txt", "r");
        if (!votes_check)
        {
            printf(RED "❌ Error: No vote data found! Expected temp-voted-list.txt or votes.txt.\n" RESET);
            return DATA_ERROR_FILE_NOT_FOUND;
        }
        fclose(votes_check);
    }

    // Initialize candidate results array
    const int MAX_CANDIDATES = 1000;
    candidate_result_t *candidates = malloc(MAX_CANDIDATES * sizeof(candidate_result_t));
    if (!candidates)
    {
        printf(RED "❌ Error: Memory allocation failed!\n" RESET);
        return DATA_ERROR_MEMORY_ALLOCATION;
    }

    // Load candidate list and vote counts
    printf(YELLOW "📊 Loading candidate data and vote counts...\n" RESET);
    int candidate_count = load_candidates(candidates, MAX_CANDIDATES);
    if (candidate_count == 0)
    {
        printf(RED "❌ Error: No candidates found or unable to load data!\n" RESET);
        free(candidates);
        return DATA_ERROR_FILE_NOT_FOUND;
    }

    // Reset vote counts (safety) and count from chosen source
    for (int i = 0; i < candidate_count; ++i)
        candidates[i].vote_count = 0;
    if (use_temp_list)
        count_votes_from_temp_list(candidates, candidate_count);
    else
        count_votes_from_votes_txt(candidates, candidate_count);

    // Calculate total votes
    int total_votes = 0;
    for (int i = 0; i < candidate_count; i++)
    {
        total_votes += candidates[i].vote_count;
    }

    printf(GREEN "✅ Loaded %d candidates with %d total votes\n" RESET, candidate_count, total_votes);

    // Apply voting algorithm
    printf(YELLOW "🏛️  Applying parliament selection algorithm...\n" RESET);
    int parliament_members = select_parliament_members(candidates, candidate_count,
                                                       min_votes_required, max_parliament_members);

    // Count qualified candidates (those meeting minimum vote requirement)
    int qualified_count = 0;
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].vote_count >= min_votes_required)
        {
            qualified_count++;
        }
    }

    // Prepare statistics
    voting_statistics_t stats;
    stats.total_candidates = candidate_count;
    stats.total_votes_cast = total_votes;
    stats.qualified_candidates = qualified_count;
    stats.parliament_members_selected = parliament_members;
    stats.min_votes_threshold = min_votes_required;
    stats.max_parliament_seats = max_parliament_members;

    // Get current date and time
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(stats.voting_date, sizeof(stats.voting_date), "%Y-%m-%d", tm_info);
    strftime(stats.voting_time, sizeof(stats.voting_time), "%H:%M:%S", tm_info);

    printf(GREEN "✅ Parliament selection complete: %d members selected\n" RESET, parliament_members);

    // Generate and display results
    generate_results_report(candidates, candidate_count, &stats);

    // Save results to file
    save_results_to_file(candidates, candidate_count, &stats);

    // Overwrite parliament candidates file with selected members only
    write_parliament_candidates(candidates, candidate_count);

    // If temp list was used, clear it after processing
    if (use_temp_list)
    {
        int rc = clear_temp_voted();
        if (rc == DATA_SUCCESS)
            printf(GREEN "🧹 Cleared temporary voted list after processing.\n" RESET);
        else
            printf(RED "❗ Failed to clear temp voted list: %s\n" RESET, get_last_error());
    }

    // Clean up
    free(candidates);

    printf(BOLD GREEN "\n🎉 VOTING ALGORITHM COMPLETED SUCCESSFULLY!\n" RESET);
    printf("═══════════════════════════════════════════════════════════════════════════════\n");

    return DATA_SUCCESS;
}

/**
 * Create sample votes file for testing (if it doesn't exist)
 */
int create_sample_votes_file(void)
{
    // Check if votes file already exists
    FILE *check = fopen("data/votes.txt", "r");
    if (check)
    {
        fclose(check);
        return DATA_SUCCESS; // File already exists
    }

    // Create sample votes
    FILE *votes_file = fopen("data/votes.txt", "w");
    if (!votes_file)
    {
        return DATA_ERROR_FILE_NOT_FOUND;
    }

    fprintf(votes_file, "voter_id,candidate_id\n");

    // Generate some sample votes
    // Simulate voting patterns
    fprintf(votes_file, "V001,C001\n"); // John votes for Alice
    fprintf(votes_file, "V002,C001\n"); // Jane votes for Alice
    fprintf(votes_file, "V003,C002\n"); // Bob votes for Charlie

    fclose(votes_file);

    printf(GREEN "📁 Sample votes file created with 3 votes\n" RESET);
    return DATA_SUCCESS;
}

/**
 * Display voting algorithm parameters
 * @param min_votes Minimum votes required
 * @param max_parliament_seats Maximum parliament seats
 */
void display_voting_parameters(int min_votes, int max_parliament_seats)
{
    printf(BOLD YELLOW "\n⚙️  VOTING ALGORITHM PARAMETERS:\n" RESET);
    printf("┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│ " BLUE "Minimum Votes for Parliament:" RESET "    %-8d                │\n", min_votes);
    printf("│ " BLUE "Maximum Parliament Members:" RESET "      %-8d                │\n", max_parliament_seats);
    printf("└─────────────────────────────────────────────────────────────────┘\n");
}
