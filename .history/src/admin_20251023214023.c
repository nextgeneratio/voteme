/*
 * Enhanced Admin System for VoteMe Application
 *
 * Features:
 * - Complete CRUD operations for all entities
 * - Data viewing and browsing capabilities
 * - System limit management and configuration
 * - Interactive menu-driven interface
 * - Comprehensive error handling
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "data_handle.h"
#include "ui_utils.h"
#include "voting.h"
#include "voting.h"
#include "voting-interface.h"

// Color codes for better user interface
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define WHITE "\033[0;37m"
#define RESET "\033[0m"
#define BOLD "\033[1m"

// System configuration structure
typedef struct
{
    int max_voters;
    int max_candidates;
    int min_votes_for_parliament;
    int max_parliament_members;
    int max_parties;
    int max_districts;
    int voting_enabled;
} system_config_t;

// Global system configuration
static system_config_t sys_config = {
    .max_voters = 10000,
    .max_candidates = 500,
    .min_votes_for_parliament = 100,
    .max_parliament_members = 225,
    .max_parties = 50,
    .max_districts = 25,
    .voting_enabled = 1};

// Configuration file path
#define CONFIG_FILE "data/system_config.txt"

// Data file definitions
typedef struct
{
    const char *filename;
    const char *description;
    const char *header;
} data_file_t;

static const data_file_t data_files[] = {
    {"data/approved_voters.txt", "Approved Voters", "voting_number,name,nic,district_id"},
    {"data/approved_candidates.txt", "Approved Candidates", "candidate_number,name,party_id,district_id,nic"},
    {"data/party_name.txt", "Political Parties", "party_id,party_name"},
    {"data/district.txt", "Electoral Districts", "district_id,district_name"},
    {"data/parliament_candidates.txt", "Parliament Candidates", "candidate_number,party_id"},
    {"data/voter_count.txt", "Vote Counts", "voting_number,candidate_number,party_id,district_id,count"},
    {NULL, NULL, NULL} // Sentinel
};

// Function prototypes
void display_main_menu(void);
void handle_crud_operations(void);
void handle_data_viewing(void);
void handle_voting_algorithm(void);
void handle_voting_algorithm(void);
void handle_system_limits(void);
// moved to ui_utils.c
void display_banner(void);
void pause_for_user(void);
void clear_screen(void);

// CRUD operation functions
void crud_menu(void);
void create_entity_menu(void);
void read_entity_menu(void);
void update_entity_menu(void);
void delete_entity_menu(void);

// Data viewing functions
void view_all_data_files(void);
void view_specific_file(const char *filename, const char *description);
void display_file_list(void);

// System configuration functions
void load_system_config(void);
void save_system_config(void);
void display_current_limits(void);
void modify_system_limits(void);
void reset_to_defaults(void);

// Entity-specific CRUD functions
void create_voter_interactive(void);
void create_candidate_interactive(void);
void create_party_interactive(void);
void create_district_interactive(void);

void read_voter_interactive(void);
void read_candidate_interactive(void);
void read_party_interactive(void);
void read_district_interactive(void);

void update_voter_interactive(void);
void update_candidate_interactive(void);
void update_party_interactive(void);
void update_district_interactive(void);

void delete_voter_interactive(void);
void delete_candidate_interactive(void);
void delete_party_interactive(void);
void delete_district_interactive(void);

// Utility functions
// moved to ui_utils.c
void get_user_input(const char *prompt, char *buffer, size_t size);
int get_user_choice(const char *prompt, int min, int max);
int validate_system_limits(void);
// moved to ui_utils.c
void display_error(const char *message);
void display_success(const char *message);
void display_info(const char *message);
int count_records_in_file(const char *filename);

// =====================================================
// Main function and menu system
// =====================================================

int main(void)
{
    // Initialize system
    clear_screen();
    display_banner();

    // Load system configuration
    load_system_config();

    // Ensure data directory exists
    if (system("mkdir -p data") != 0)
    {
        display_error("Failed to create data directory!");
    }

    int choice;
    do
    {
        display_main_menu();
        choice = get_user_choice("Enter your choice", 0, 5);

        switch (choice)
        {
        case 1:
            handle_crud_operations();
            break;
        case 2:
            handle_data_viewing();
            break;
        case 3:
            handle_system_limits();
            break;
        case 4:
            printf(CYAN "\n📊 System Status:\n" RESET);
            display_current_limits();
            pause_for_user();
            break;
        case 5:
            handle_voting_algorithm();
            break;
        case 0:
            printf(GREEN "\n👋 Thank you for using VoteMe Admin System!\n" RESET);
            save_system_config();
            break;
        default:
            display_error("Invalid choice! Please try again.");
            break;
        }
    } while (choice != 0);

    return 0;
}

// display_banner moved to ui_utils.c

void display_main_menu(void)
{
    clear_screen();
    printf(BOLD CYAN "🏛️  VoteMe Admin System - Main Menu\n" RESET);
    printf("═══════════════════════════════════════\n\n");

    printf(YELLOW "1." RESET " 📝 " BOLD "CRUD Operations" RESET " - Create, Read, Update, Delete records\n");
    printf(YELLOW "2." RESET " 📊 " BOLD "Data Viewing" RESET " - Browse and view all data files\n");
    printf(YELLOW "3." RESET " ⚙️  " BOLD "System Limits" RESET " - Configure voting system parameters\n");
    printf(YELLOW "4." RESET " 📈 " BOLD "System Status" RESET " - View current configuration\n");
    printf(YELLOW "5." RESET " 🗳️  " BOLD "Execute Voting" RESET " - Run voting algorithm and generate results\n");
    printf(YELLOW "6." RESET " 👤 " BOLD "Vote (Interactive)" RESET " - Validate voter and cast a vote\n");
    printf(YELLOW "0." RESET " 🚪 " BOLD "Exit" RESET " - Save and quit\n\n");

    // Display quick status
    printf(CYAN "Quick Status: " RESET);
    printf("Voters: %d/%d | Candidates: %d/%d | Voting: %s\n\n",
           count_records_in_file("data/approved_voters.txt"), sys_config.max_voters,
           count_records_in_file("data/approved_candidates.txt"), sys_config.max_candidates,
           sys_config.voting_enabled ? GREEN "ENABLED" RESET : RED "DISABLED" RESET);
}

// =====================================================
// CRUD Operations Handler
// =====================================================

void handle_crud_operations(void)
{
    int choice;
    do
    {
        crud_menu();
        choice = get_user_choice("Enter your choice", 0, 4);

        switch (choice)
        {
        case 1:
            create_entity_menu();
            break;
        case 2:
            read_entity_menu();
            break;
        case 3:
            update_entity_menu();
            break;
        case 4:
            delete_entity_menu();
            break;
        case 0:
            break;
        default:
            display_error("Invalid choice! Please try again.");
            break;
        }
    } while (choice != 0);
}

void crud_menu(void)
{
    clear_screen();
    printf(BOLD GREEN "📝 CRUD Operations Menu\n" RESET);
    printf("═══════════════════════════\n\n");

    printf(YELLOW "1." RESET " ➕ " BOLD "Create" RESET " - Add new records (Voters, Candidates, Parties, Districts)\n");
    printf(YELLOW "2." RESET " 🔍 " BOLD "Read" RESET " - Search and view specific records\n");
    printf(YELLOW "3." RESET " ✏️  " BOLD "Update" RESET " - Modify existing records\n");
    printf(YELLOW "4." RESET " 🗑️  " BOLD "Delete" RESET " - Remove records from system\n");
    printf(YELLOW "0." RESET " ⬅️  " BOLD "Back" RESET " - Return to main menu\n\n");
}

void create_entity_menu(void)
{
    clear_screen();
    printf(BOLD GREEN "➕ Create New Record\n" RESET);
    printf("══════════════════════\n\n");

    printf("Select entity type to create:\n\n");
    printf(YELLOW "1." RESET " 👤 Create Voter\n");
    printf(YELLOW "2." RESET " 🏛️  Create Candidate\n");
    printf(YELLOW "3." RESET " 🎭 Create Party\n");
    printf(YELLOW "4." RESET " 🏘️  Create District\n");
    printf(YELLOW "0." RESET " ⬅️  Back\n\n");

    int choice = get_user_choice("Enter your choice", 0, 4);

    switch (choice)
    {
    case 1:
        create_voter_interactive();
        break;
    case 2:
        create_candidate_interactive();
        break;
    case 3:
        create_party_interactive();
        break;
    case 4:
        create_district_interactive();
        break;
    case 0:
        break;
    default:
        display_error("Invalid choice!");
        break;
    }
}

// =====================================================
// Data Viewing Handler
// =====================================================

void handle_data_viewing(void)
{
    clear_screen();
    printf(BOLD BLUE "📊 Data Viewing and Browsing\n" RESET);
    printf("════════════════════════════\n\n");

    printf("Select viewing option:\n\n");
    printf(YELLOW "1." RESET " 📋 View All Data Files Summary\n");
    printf(YELLOW "2." RESET " 🔍 Browse Specific File\n");
    printf(YELLOW "0." RESET " ⬅️  Back to Main Menu\n\n");

    int choice = get_user_choice("Enter your choice", 0, 2);

    switch (choice)
    {
    case 1:
        view_all_data_files();
        break;
    case 2:
        display_file_list();
        break;
    case 0:
        break;
    default:
        display_error("Invalid choice!");
        break;
    }
}

void view_all_data_files(void)
{
    clear_screen();
    printf(BOLD BLUE "📋 All Data Files Summary\n" RESET);
    printf("══════════════════════════\n\n");

    printf("%-5s %-30s %-15s %s\n", "No.", "File Description", "Record Count", "Status");
    printf("─────────────────────────────────────────────────────────────────\n");

    for (int i = 0; data_files[i].filename != NULL; i++)
    {
        int count = count_records_in_file(data_files[i].filename);
        const char *status = (count >= 0) ? GREEN "✓ Available" RESET : RED "✗ Missing" RESET;

        printf("%-5d %-30s %-15d %s\n",
               i + 1, data_files[i].description,
               (count >= 0) ? count : 0, status);
    }

    printf("\n");
    pause_for_user();
}

void display_file_list(void)
{
    clear_screen();
    printf(BOLD BLUE "🔍 Select File to Browse\n" RESET);
    printf("═══════════════════════════\n\n");

    for (int i = 0; data_files[i].filename != NULL; i++)
    {
        int count = count_records_in_file(data_files[i].filename);
        printf(YELLOW "%d." RESET " %s (%d records)\n",
               i + 1, data_files[i].description,
               (count >= 0) ? count : 0);
    }
    printf(YELLOW "0." RESET " ⬅️  Back\n\n");

    int choice = get_user_choice("Enter file number", 0, 6);

    if (choice > 0 && choice <= 6)
    {
        view_specific_file(data_files[choice - 1].filename,
                           data_files[choice - 1].description);
    }
}

void view_specific_file(const char *filename, const char *description)
{
    clear_screen();
    printf(BOLD BLUE "📄 Viewing: %s\n" RESET, description);
    printf("═══════════════════════════════════════\n\n");

    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        display_error("File not found or cannot be opened!");
        pause_for_user();
        return;
    }

    char line[MAX_LINE_LENGTH];
    int line_count = 0;
    int display_count = 0;
    const int MAX_DISPLAY = 20; // Display 20 records at a time

    // Read and display header
    if (fgets(line, sizeof(line), fp))
    {
        printf(BOLD CYAN "Header: " RESET "%s", line);
        printf("─────────────────────────────────────────────────────────────\n");
        line_count++;
    }

    // Read and display data
    while (fgets(line, sizeof(line), fp) && display_count < MAX_DISPLAY)
    {
        printf(YELLOW "%3d:" RESET " %s", line_count + 1, line);
        line_count++;
        display_count++;
    }

    // Check if there are more records
    if (fgets(line, sizeof(line), fp))
    {
        printf(CYAN "\n... and %d more records (showing first %d)\n" RESET,
               count_records_in_file(filename) - MAX_DISPLAY, MAX_DISPLAY);
    }

    fclose(fp);

    printf("\n" CYAN "Total records: %d\n" RESET, count_records_in_file(filename));
    pause_for_user();
}

// =====================================================
// System Limits Handler
// =====================================================

void handle_system_limits(void)
{
    int choice;
    do
    {
        clear_screen();
        printf(BOLD MAGENTA "⚙️  System Limits Configuration\n" RESET);
        printf("═══════════════════════════════════\n\n");

        display_current_limits();

        printf("\nConfiguration Options:\n\n");
        printf(YELLOW "1." RESET " ✏️  Modify System Limits\n");
        printf(YELLOW "2." RESET " 🔄 Reset to Default Values\n");
        printf(YELLOW "3." RESET " 💾 Save Configuration\n");
        printf(YELLOW "4." RESET " 🔄 Reload Configuration\n");
        printf(YELLOW "5." RESET " ✅ Validate Current Settings\n");
        printf(YELLOW "0." RESET " ⬅️  Back to Main Menu\n\n");

        choice = get_user_choice("Enter your choice", 0, 6);

        switch (choice)
        {
        case 5:
            modify_system_limits();
            break;
        case 6:
        {
            int rc = vote_for_candidate_interactive();
            if (rc == DATA_SUCCESS)
            {
                display_success("Vote recorded successfully.");
            }
            else
            {
                char msg[256];
                snprintf(msg, sizeof(msg), "Failed to record vote: %s", get_last_error());
                display_error(msg);
            }
            pause_for_user();
            break;
        }
        case 2:
            reset_to_defaults();
            display_success("System limits reset to default values!");
            pause_for_user();
            break;
        case 3:
            save_system_config();
            display_success("Configuration saved successfully!");
            pause_for_user();
            break;
        case 4:
            load_system_config();
            display_success("Configuration reloaded from file!");
            pause_for_user();
            break;
        case 5:
            if (validate_system_limits())
            {
                display_success("All system limits are valid!");
            }
            else
            {
                display_error("Some system limits may cause issues!");
            }
            pause_for_user();
            break;
        case 0:
            break;
        default:
            display_error("Invalid choice!");
            break;
        }
    } while (choice != 0);
}

void display_current_limits(void)
{
    printf(BOLD CYAN "Current System Configuration:\n" RESET);
    printf("╭─────────────────────────────────────────╮\n");
    printf("│ " YELLOW "Maximum Voters:" RESET "              %-8d │\n", sys_config.max_voters);
    printf("│ " YELLOW "Maximum Candidates:" RESET "          %-8d │\n", sys_config.max_candidates);
    printf("│ " YELLOW "Minimum Votes for Parliament:" RESET " %-8d │\n", sys_config.min_votes_for_parliament);
    printf("│ " YELLOW "Maximum Parliament Members:" RESET "   %-8d │\n", sys_config.max_parliament_members);
    printf("│ " YELLOW "Maximum Parties:" RESET "             %-8d │\n", sys_config.max_parties);
    printf("│ " YELLOW "Maximum Districts:" RESET "           %-8d │\n", sys_config.max_districts);
    printf("│ " YELLOW "Voting System:" RESET "               %-8s │\n",
           sys_config.voting_enabled ? GREEN "ENABLED" RESET : RED "DISABLED" RESET);
    printf("╰─────────────────────────────────────────╯\n");
}

void modify_system_limits(void)
{
    clear_screen();
    printf(BOLD MAGENTA "✏️  Modify System Limits\n" RESET);
    printf("═══════════════════════════\n\n");

    display_current_limits();
    printf("\n");

    printf("Select parameter to modify:\n\n");
    printf(YELLOW "1." RESET " Maximum Voters (current: %d)\n", sys_config.max_voters);
    printf(YELLOW "2." RESET " Maximum Candidates (current: %d)\n", sys_config.max_candidates);
    printf(YELLOW "3." RESET " Minimum Votes for Parliament (current: %d)\n", sys_config.min_votes_for_parliament);
    printf(YELLOW "4." RESET " Maximum Parliament Members (current: %d)\n", sys_config.max_parliament_members);
    printf(YELLOW "5." RESET " Maximum Parties (current: %d)\n", sys_config.max_parties);
    printf(YELLOW "6." RESET " Maximum Districts (current: %d)\n", sys_config.max_districts);
    printf(YELLOW "7." RESET " Voting System Status (current: %s)\n",
           sys_config.voting_enabled ? "ENABLED" : "DISABLED");
    printf(YELLOW "0." RESET " ⬅️  Back\n\n");

    int choice = get_user_choice("Enter parameter number", 0, 7);
    int new_value;

    switch (choice)
    {
    case 1:
        new_value = get_user_choice("Enter new maximum voters (1-100000)", 1, 100000);
        sys_config.max_voters = new_value;
        display_success("Maximum voters updated!");
        break;
    case 2:
        new_value = get_user_choice("Enter new maximum candidates (1-10000)", 1, 10000);
        sys_config.max_candidates = new_value;
        display_success("Maximum candidates updated!");
        break;
    case 3:
        new_value = get_user_choice("Enter new minimum votes for parliament (1-10000)", 1, 10000);
        sys_config.min_votes_for_parliament = new_value;
        display_success("Minimum votes for parliament updated!");
        break;
    case 4:
        new_value = get_user_choice("Enter new maximum parliament members (1-1000)", 1, 1000);
        sys_config.max_parliament_members = new_value;
        display_success("Maximum parliament members updated!");
        break;
    case 5:
        new_value = get_user_choice("Enter new maximum parties (1-200)", 1, 200);
        sys_config.max_parties = new_value;
        display_success("Maximum parties updated!");
        break;
    case 6:
        new_value = get_user_choice("Enter new maximum districts (1-100)", 1, 100);
        sys_config.max_districts = new_value;
        display_success("Maximum districts updated!");
        break;
    case 7:
        new_value = get_user_choice("Enable voting? (1=Yes, 0=No)", 0, 1);
        sys_config.voting_enabled = new_value;
        display_success(new_value ? "Voting enabled!" : "Voting disabled!");
        break;
    case 0:
        return;
    default:
        display_error("Invalid choice!");
        break;
    }

    pause_for_user();
}

// =====================================================
// Configuration Management Functions
// =====================================================

void load_system_config(void)
{
    FILE *fp = fopen(CONFIG_FILE, "r");
    if (!fp)
    {
        // Create default config file if it doesn't exist
        save_system_config();
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp))
    {
        if (strncmp(line, "max_voters=", 11) == 0)
        {
            sys_config.max_voters = atoi(line + 11);
        }
        else if (strncmp(line, "max_candidates=", 15) == 0)
        {
            sys_config.max_candidates = atoi(line + 15);
        }
        else if (strncmp(line, "min_votes_for_parliament=", 25) == 0)
        {
            sys_config.min_votes_for_parliament = atoi(line + 25);
        }
        else if (strncmp(line, "max_parliament_members=", 23) == 0)
        {
            sys_config.max_parliament_members = atoi(line + 23);
        }
        else if (strncmp(line, "max_parties=", 12) == 0)
        {
            sys_config.max_parties = atoi(line + 12);
        }
        else if (strncmp(line, "max_districts=", 14) == 0)
        {
            sys_config.max_districts = atoi(line + 14);
        }
        else if (strncmp(line, "voting_enabled=", 15) == 0)
        {
            sys_config.voting_enabled = atoi(line + 15);
        }
    }

    fclose(fp);
}

void save_system_config(void)
{
    FILE *fp = fopen(CONFIG_FILE, "w");
    if (!fp)
    {
        display_error("Cannot save configuration file!");
        return;
    }

    fprintf(fp, "# VoteMe System Configuration\n");
    fprintf(fp, "# Generated automatically - modify with admin interface\n\n");
    fprintf(fp, "max_voters=%d\n", sys_config.max_voters);
    fprintf(fp, "max_candidates=%d\n", sys_config.max_candidates);
    fprintf(fp, "min_votes_for_parliament=%d\n", sys_config.min_votes_for_parliament);
    fprintf(fp, "max_parliament_members=%d\n", sys_config.max_parliament_members);
    fprintf(fp, "max_parties=%d\n", sys_config.max_parties);
    fprintf(fp, "max_districts=%d\n", sys_config.max_districts);
    fprintf(fp, "voting_enabled=%d\n", sys_config.voting_enabled);

    fclose(fp);
}

void reset_to_defaults(void)
{
    sys_config.max_voters = 10000;
    sys_config.max_candidates = 500;
    sys_config.min_votes_for_parliament = 100;
    sys_config.max_parliament_members = 225;
    sys_config.max_parties = 50;
    sys_config.max_districts = 25;
    sys_config.voting_enabled = 1;
}

// =====================================================
// Interactive CRUD Functions - Create Operations
// =====================================================

void create_voter_interactive(void)
{
    clear_screen();
    printf(BOLD GREEN "👤 Create New Voter\n" RESET);
    printf("═══════════════════\n\n");

    // Check limits
    int current_count = count_records_in_file("data/approved_voters.txt");
    if (current_count >= sys_config.max_voters)
    {
        display_error("Maximum voter limit reached! Cannot add more voters.");
        pause_for_user();
        return;
    }

    char voting_number[100], name[200], nic[50], district_id[50];

    get_user_input("Enter Voting Number", voting_number, sizeof(voting_number));
    get_user_input("Enter Full Name", name, sizeof(name));
    get_user_input("Enter NIC Number", nic, sizeof(nic));
    get_user_input("Enter District ID", district_id, sizeof(district_id));

    printf("\n" CYAN "Creating voter with details:\n" RESET);
    printf("Voting Number: %s\n", voting_number);
    printf("Name: %s\n", name);
    printf("NIC: %s\n", nic);
    printf("District: %s\n", district_id);

    if (get_user_choice("\nConfirm creation? (1=Yes, 0=No)", 0, 1))
    {
        int result = create_voter(voting_number, name, nic, district_id);
        if (result == DATA_SUCCESS)
        {
            display_success("Voter created successfully!");
        }
        else
        {
            display_error("Failed to create voter!");
            printf(RED "Error: %s\n" RESET, get_last_error());
        }
    }
    else
    {
        display_info("Voter creation cancelled.");
    }

    pause_for_user();
}

void create_candidate_interactive(void)
{
    clear_screen();
    printf(BOLD GREEN "🏛️  Create New Candidate\n" RESET);
    printf("═══════════════════════════\n\n");

    // Check limits
    int current_count = count_records_in_file("data/approved_candidates.txt");
    if (current_count >= sys_config.max_candidates)
    {
        display_error("Maximum candidate limit reached! Cannot add more candidates.");
        pause_for_user();
        return;
    }

    char candidate_number[100], name[200], party_id[50], district_id[50], nic[50];

    get_user_input("Enter Candidate Number", candidate_number, sizeof(candidate_number));
    get_user_input("Enter Full Name", name, sizeof(name));
    get_user_input("Enter Party ID", party_id, sizeof(party_id));
    get_user_input("Enter District ID", district_id, sizeof(district_id));
    get_user_input("Enter NIC Number", nic, sizeof(nic));

    printf("\n" CYAN "Creating candidate with details:\n" RESET);
    printf("Candidate Number: %s\n", candidate_number);
    printf("Name: %s\n", name);
    printf("Party ID: %s\n", party_id);
    printf("District: %s\n", district_id);
    printf("NIC: %s\n", nic);

    if (get_user_choice("\nConfirm creation? (1=Yes, 0=No)", 0, 1))
    {
        int result = create_candidate(candidate_number, name, party_id, district_id, nic);
        if (result == DATA_SUCCESS)
        {
            display_success("Candidate created successfully!");
        }
        else
        {
            display_error("Failed to create candidate!");
            printf(RED "Error: %s\n" RESET, get_last_error());
        }
    }
    else
    {
        display_info("Candidate creation cancelled.");
    }

    pause_for_user();
}

void create_party_interactive(void)
{
    clear_screen();
    printf(BOLD GREEN "🎭 Create New Party\n" RESET);
    printf("══════════════════\n\n");

    // Check limits
    int current_count = count_records_in_file("data/party_name.txt");
    if (current_count >= sys_config.max_parties)
    {
        display_error("Maximum party limit reached! Cannot add more parties.");
        pause_for_user();
        return;
    }

    char party_id[50], party_name[200];

    get_user_input("Enter Party ID", party_id, sizeof(party_id));
    get_user_input("Enter Party Name", party_name, sizeof(party_name));

    printf("\n" CYAN "Creating party with details:\n" RESET);
    printf("Party ID: %s\n", party_id);
    printf("Party Name: %s\n", party_name);

    if (get_user_choice("\nConfirm creation? (1=Yes, 0=No)", 0, 1))
    {
        int result = create_party(party_id, party_name);
        if (result == DATA_SUCCESS)
        {
            display_success("Party created successfully!");
        }
        else
        {
            display_error("Failed to create party!");
            printf(RED "Error: %s\n" RESET, get_last_error());
        }
    }
    else
    {
        display_info("Party creation cancelled.");
    }

    pause_for_user();
}

void create_district_interactive(void)
{
    clear_screen();
    printf(BOLD GREEN "🏘️  Create New District\n" RESET);
    printf("═══════════════════════\n\n");

    // Check limits
    int current_count = count_records_in_file("data/district.txt");
    if (current_count >= sys_config.max_districts)
    {
        display_error("Maximum district limit reached! Cannot add more districts.");
        pause_for_user();
        return;
    }

    char district_id[50], district_name[200];

    get_user_input("Enter District ID", district_id, sizeof(district_id));
    get_user_input("Enter District Name", district_name, sizeof(district_name));

    printf("\n" CYAN "Creating district with details:\n" RESET);
    printf("District ID: %s\n", district_id);
    printf("District Name: %s\n", district_name);

    if (get_user_choice("\nConfirm creation? (1=Yes, 0=No)", 0, 1))
    {
        int result = create_district(district_id, district_name);
        if (result == DATA_SUCCESS)
        {
            display_success("District created successfully!");
        }
        else
        {
            display_error("Failed to create district!");
            printf(RED "Error: %s\n" RESET, get_last_error());
        }
    }
    else
    {
        display_info("District creation cancelled.");
    }

    pause_for_user();
}

// =====================================================
// Read Entity Functions
// =====================================================

void read_entity_menu(void)
{
    clear_screen();
    printf(BOLD BLUE "🔍 Read/Search Records\n" RESET);
    printf("══════════════════════\n\n");

    printf("Select entity type to search:\n\n");
    printf(YELLOW "1." RESET " 👤 Search Voter\n");
    printf(YELLOW "2." RESET " 🏛️  Search Candidate\n");
    printf(YELLOW "3." RESET " 🎭 Search Party\n");
    printf(YELLOW "4." RESET " 🏘️  Search District\n");
    printf(YELLOW "0." RESET " ⬅️  Back\n\n");

    int choice = get_user_choice("Enter your choice", 0, 4);

    switch (choice)
    {
    case 1:
        read_voter_interactive();
        break;
    case 2:
        read_candidate_interactive();
        break;
    case 3:
        read_party_interactive();
        break;
    case 4:
        read_district_interactive();
        break;
    case 0:
        break;
    default:
        display_error("Invalid choice!");
        break;
    }
}

void read_voter_interactive(void)
{
    clear_screen();
    printf(BOLD BLUE "👤 Search Voter\n" RESET);
    printf("═══════════════\n\n");

    char voting_number[100];
    get_user_input("Enter Voting Number to search", voting_number, sizeof(voting_number));

    char *result = read_voter(voting_number);
    if (result)
    {
        printf("\n" GREEN "✓ Voter Found:\n" RESET);
        printf("═══════════════\n");
        printf("%s\n", result);
        free(result);
    }
    else
    {
        display_error("Voter not found or error occurred!");
        printf(RED "Error: %s\n" RESET, get_last_error());
    }

    pause_for_user();
}

void read_candidate_interactive(void)
{
    clear_screen();
    printf(BOLD BLUE "🏛️  Search Candidate\n" RESET);
    printf("═══════════════════\n\n");

    char candidate_number[100];
    get_user_input("Enter Candidate Number to search", candidate_number, sizeof(candidate_number));

    char *result = read_candidate(candidate_number);
    if (result)
    {
        printf("\n" GREEN "✓ Candidate Found:\n" RESET);
        printf("═══════════════════\n");
        printf("%s\n", result);
        free(result);
    }
    else
    {
        display_error("Candidate not found or error occurred!");
        printf(RED "Error: %s\n" RESET, get_last_error());
    }

    pause_for_user();
}

void read_party_interactive(void)
{
    clear_screen();
    printf(BOLD BLUE "🎭 Search Party\n" RESET);
    printf("═══════════════\n\n");

    char party_id[50];
    get_user_input("Enter Party ID to search", party_id, sizeof(party_id));

    char *result = read_party(party_id);
    if (result)
    {
        printf("\n" GREEN "✓ Party Found:\n" RESET);
        printf("═══════════════\n");
        printf("%s\n", result);
        free(result);
    }
    else
    {
        display_error("Party not found or error occurred!");
        printf(RED "Error: %s\n" RESET, get_last_error());
    }

    pause_for_user();
}

void read_district_interactive(void)
{
    clear_screen();
    printf(BOLD BLUE "🏘️  Search District\n" RESET);
    printf("═══════════════════\n\n");

    char district_id[50];
    get_user_input("Enter District ID to search", district_id, sizeof(district_id));

    char *result = read_district(district_id);
    if (result)
    {
        printf("\n" GREEN "✓ District Found:\n" RESET);
        printf("═══════════════════\n");
        printf("%s\n", result);
        free(result);
    }
    else
    {
        display_error("District not found or error occurred!");
        printf(RED "Error: %s\n" RESET, get_last_error());
    }

    pause_for_user();
}

// =====================================================
// Update Entity Functions
// =====================================================

void update_entity_menu(void)
{
    clear_screen();
    printf(BOLD YELLOW "✏️  Update Records\n" RESET);
    printf("══════════════════\n\n");

    printf("Select entity type to update:\n\n");
    printf(YELLOW "1." RESET " 👤 Update Voter\n");
    printf(YELLOW "2." RESET " 🏛️  Update Candidate\n");
    printf(YELLOW "3." RESET " 🎭 Update Party\n");
    printf(YELLOW "4." RESET " 🏘️  Update District\n");
    printf(YELLOW "0." RESET " ⬅️  Back\n\n");

    int choice = get_user_choice("Enter your choice", 0, 4);

    switch (choice)
    {
    case 1:
        update_voter_interactive();
        break;
    case 2:
        update_candidate_interactive();
        break;
    case 3:
        update_party_interactive();
        break;
    case 4:
        update_district_interactive();
        break;
    case 0:
        break;
    default:
        display_error("Invalid choice!");
        break;
    }
}

void update_voter_interactive(void)
{
    clear_screen();
    printf(BOLD YELLOW "👤 Update Voter\n" RESET);
    printf("═══════════════\n\n");

    char voting_number[100];
    get_user_input("Enter Voting Number to update", voting_number, sizeof(voting_number));

    // First, show current data
    char *current = read_voter(voting_number);
    if (!current)
    {
        display_error("Voter not found!");
        printf(RED "Error: %s\n" RESET, get_last_error());
        pause_for_user();
        return;
    }

    printf("\n" CYAN "Current voter data:\n" RESET);
    printf("%s\n", current);
    free(current);

    printf("\nSelect field to update:\n");
    printf(YELLOW "1." RESET " Name\n");
    printf(YELLOW "2." RESET " NIC Number\n");
    printf(YELLOW "3." RESET " District ID\n");
    printf(YELLOW "0." RESET " Cancel\n\n");

    int field_choice = get_user_choice("Enter field number", 0, 3);
    if (field_choice == 0)
        return;

    char new_value[200];
    get_user_input("Enter new value", new_value, sizeof(new_value));

    if (get_user_choice("Confirm update? (1=Yes, 0=No)", 0, 1))
    {
        int result = update_voter(voting_number, field_choice, new_value);
        if (result == DATA_SUCCESS)
        {
            display_success("Voter updated successfully!");
        }
        else
        {
            display_error("Failed to update voter!");
            printf(RED "Error: %s\n" RESET, get_last_error());
        }
    }
    else
    {
        display_info("Update cancelled.");
    }

    pause_for_user();
}

void update_candidate_interactive(void)
{
    clear_screen();
    printf(BOLD YELLOW "🏛️  Update Candidate\n" RESET);
    printf("═══════════════════\n\n");

    char candidate_number[100];
    get_user_input("Enter Candidate Number to update", candidate_number, sizeof(candidate_number));

    // First, show current data
    char *current = read_candidate(candidate_number);
    if (!current)
    {
        display_error("Candidate not found!");
        printf(RED "Error: %s\n" RESET, get_last_error());
        pause_for_user();
        return;
    }

    printf("\n" CYAN "Current candidate data:\n" RESET);
    printf("%s\n", current);
    free(current);

    printf("\nSelect field to update:\n");
    printf(YELLOW "1." RESET " Name\n");
    printf(YELLOW "2." RESET " Party ID\n");
    printf(YELLOW "3." RESET " District ID\n");
    printf(YELLOW "4." RESET " NIC Number\n");
    printf(YELLOW "0." RESET " Cancel\n\n");

    int field_choice = get_user_choice("Enter field number", 0, 4);
    if (field_choice == 0)
        return;

    char new_value[200];
    get_user_input("Enter new value", new_value, sizeof(new_value));

    if (get_user_choice("Confirm update? (1=Yes, 0=No)", 0, 1))
    {
        int result = update_candidate(candidate_number, field_choice, new_value);
        if (result == DATA_SUCCESS)
        {
            display_success("Candidate updated successfully!");
        }
        else
        {
            display_error("Failed to update candidate!");
            printf(RED "Error: %s\n" RESET, get_last_error());
        }
    }
    else
    {
        display_info("Update cancelled.");
    }

    pause_for_user();
}

void update_party_interactive(void)
{
    clear_screen();
    printf(BOLD YELLOW "🎭 Update Party\n" RESET);
    printf("═══════════════\n\n");

    char party_id[50];
    get_user_input("Enter Party ID to update", party_id, sizeof(party_id));

    // First, show current data
    char *current = read_party(party_id);
    if (!current)
    {
        display_error("Party not found!");
        printf(RED "Error: %s\n" RESET, get_last_error());
        pause_for_user();
        return;
    }

    printf("\n" CYAN "Current party data:\n" RESET);
    printf("%s\n", current);
    free(current);

    char new_name[200];
    get_user_input("Enter new party name", new_name, sizeof(new_name));

    if (get_user_choice("Confirm update? (1=Yes, 0=No)", 0, 1))
    {
        int result = update_party(party_id, new_name);
        if (result == DATA_SUCCESS)
        {
            display_success("Party updated successfully!");
        }
        else
        {
            display_error("Failed to update party!");
            printf(RED "Error: %s\n" RESET, get_last_error());
        }
    }
    else
    {
        display_info("Update cancelled.");
    }

    pause_for_user();
}

void update_district_interactive(void)
{
    clear_screen();
    printf(BOLD YELLOW "🏘️  Update District\n" RESET);
    printf("═══════════════════\n\n");

    char district_id[50];
    get_user_input("Enter District ID to update", district_id, sizeof(district_id));

    // First, show current data
    char *current = read_district(district_id);
    if (!current)
    {
        display_error("District not found!");
        printf(RED "Error: %s\n" RESET, get_last_error());
        pause_for_user();
        return;
    }

    printf("\n" CYAN "Current district data:\n" RESET);
    printf("%s\n", current);
    free(current);

    char new_name[200];
    get_user_input("Enter new district name", new_name, sizeof(new_name));

    if (get_user_choice("Confirm update? (1=Yes, 0=No)", 0, 1))
    {
        int result = update_district(district_id, new_name);
        if (result == DATA_SUCCESS)
        {
            display_success("District updated successfully!");
        }
        else
        {
            display_error("Failed to update district!");
            printf(RED "Error: %s\n" RESET, get_last_error());
        }
    }
    else
    {
        display_info("Update cancelled.");
    }

    pause_for_user();
}

// =====================================================
// Delete Entity Functions
// =====================================================

void delete_entity_menu(void)
{
    clear_screen();
    printf(BOLD RED "🗑️  Delete Records\n" RESET);
    printf("══════════════════\n\n");

    printf(RED "⚠️  WARNING: Deletion is permanent and cannot be undone!\n" RESET);
    printf("\nSelect entity type to delete:\n\n");
    printf(YELLOW "1." RESET " 👤 Delete Voter\n");
    printf(YELLOW "2." RESET " 🏛️  Delete Candidate\n");
    printf(YELLOW "3." RESET " 🎭 Delete Party\n");
    printf(YELLOW "4." RESET " 🏘️  Delete District\n");
    printf(YELLOW "0." RESET " ⬅️  Back\n\n");

    int choice = get_user_choice("Enter your choice", 0, 4);

    switch (choice)
    {
    case 1:
        delete_voter_interactive();
        break;
    case 2:
        delete_candidate_interactive();
        break;
    case 3:
        delete_party_interactive();
        break;
    case 4:
        delete_district_interactive();
        break;
    case 0:
        break;
    default:
        display_error("Invalid choice!");
        break;
    }
}

void delete_voter_interactive(void)
{
    clear_screen();
    printf(BOLD RED "👤 Delete Voter\n" RESET);
    printf("═══════════════\n\n");

    char voting_number[100];
    get_user_input("Enter Voting Number to delete", voting_number, sizeof(voting_number));

    // First, show current data
    char *current = read_voter(voting_number);
    if (!current)
    {
        display_error("Voter not found!");
        printf(RED "Error: %s\n" RESET, get_last_error());
        pause_for_user();
        return;
    }

    printf("\n" CYAN "Voter to be deleted:\n" RESET);
    printf("%s\n", current);
    free(current);

    printf(RED "⚠️  This action cannot be undone!\n" RESET);

    if (get_user_choice("Are you sure you want to delete this voter? (1=Yes, 0=No)", 0, 1))
    {
        int result = delete_voter(voting_number);
        if (result == DATA_SUCCESS)
        {
            display_success("Voter deleted successfully!");
        }
        else
        {
            display_error("Failed to delete voter!");
            printf(RED "Error: %s\n" RESET, get_last_error());
        }
    }
    else
    {
        display_info("Deletion cancelled.");
    }

    pause_for_user();
}

void delete_candidate_interactive(void)
{
    clear_screen();
    printf(BOLD RED "🏛️  Delete Candidate\n" RESET);
    printf("═══════════════════\n\n");

    char candidate_number[100];
    get_user_input("Enter Candidate Number to delete", candidate_number, sizeof(candidate_number));

    // First, show current data
    char *current = read_candidate(candidate_number);
    if (!current)
    {
        display_error("Candidate not found!");
        printf(RED "Error: %s\n" RESET, get_last_error());
        pause_for_user();
        return;
    }

    printf("\n" CYAN "Candidate to be deleted:\n" RESET);
    printf("%s\n", current);
    free(current);

    printf(RED "⚠️  This action cannot be undone!\n" RESET);

    if (get_user_choice("Are you sure you want to delete this candidate? (1=Yes, 0=No)", 0, 1))
    {
        int result = delete_candidate(candidate_number);
        if (result == DATA_SUCCESS)
        {
            display_success("Candidate deleted successfully!");
        }
        else
        {
            display_error("Failed to delete candidate!");
            printf(RED "Error: %s\n" RESET, get_last_error());
        }
    }
    else
    {
        display_info("Deletion cancelled.");
    }

    pause_for_user();
}

void delete_party_interactive(void)
{
    clear_screen();
    printf(BOLD RED "🎭 Delete Party\n" RESET);
    printf("═══════════════\n\n");

    char party_id[50];
    get_user_input("Enter Party ID to delete", party_id, sizeof(party_id));

    // First, show current data
    char *current = read_party(party_id);
    if (!current)
    {
        display_error("Party not found!");
        printf(RED "Error: %s\n" RESET, get_last_error());
        pause_for_user();
        return;
    }

    printf("\n" CYAN "Party to be deleted:\n" RESET);
    printf("%s\n", current);
    free(current);

    printf(RED "⚠️  This action cannot be undone!\n" RESET);
    printf(RED "⚠️  This may affect candidates associated with this party!\n" RESET);

    if (get_user_choice("Are you sure you want to delete this party? (1=Yes, 0=No)", 0, 1))
    {
        int result = delete_party(party_id);
        if (result == DATA_SUCCESS)
        {
            display_success("Party deleted successfully!");
        }
        else
        {
            display_error("Failed to delete party!");
            printf(RED "Error: %s\n" RESET, get_last_error());
        }
    }
    else
    {
        display_info("Deletion cancelled.");
    }

    pause_for_user();
}

void delete_district_interactive(void)
{
    clear_screen();
    printf(BOLD RED "🏘️  Delete District\n" RESET);
    printf("═══════════════════\n\n");

    char district_id[50];
    get_user_input("Enter District ID to delete", district_id, sizeof(district_id));

    // First, show current data
    char *current = read_district(district_id);
    if (!current)
    {
        display_error("District not found!");
        printf(RED "Error: %s\n" RESET, get_last_error());
        pause_for_user();
        return;
    }

    printf("\n" CYAN "District to be deleted:\n" RESET);
    printf("%s\n", current);
    free(current);

    printf(RED "⚠️  This action cannot be undone!\n" RESET);
    printf(RED "⚠️  This may affect voters and candidates in this district!\n" RESET);

    if (get_user_choice("Are you sure you want to delete this district? (1=Yes, 0=No)", 0, 1))
    {
        int result = delete_district(district_id);
        if (result == DATA_SUCCESS)
        {
            display_success("District deleted successfully!");
        }
        else
        {
            display_error("Failed to delete district!");
            printf(RED "Error: %s\n" RESET, get_last_error());
        }
    }
    else
    {
        display_info("Deletion cancelled.");
    }

    pause_for_user();
}

// =====================================================
// Utility Functions
// =====================================================

// get_user_input moved to ui_utils.c

// get_user_choice moved to ui_utils.c

int validate_system_limits(void)
{
    // Basic validation of system limits
    if (sys_config.max_voters <= 0 || sys_config.max_voters > 100000)
        return 0;
    if (sys_config.max_candidates <= 0 || sys_config.max_candidates > 10000)
        return 0;
    if (sys_config.min_votes_for_parliament <= 0)
        return 0;
    if (sys_config.max_parliament_members <= 0 || sys_config.max_parliament_members > 1000)
        return 0;
    if (sys_config.max_parties <= 0 || sys_config.max_parties > 200)
        return 0;
    if (sys_config.max_districts <= 0 || sys_config.max_districts > 100)
        return 0;

    return 1;
}

// display_error/success/info moved to ui_utils.c

// count_records_in_file moved to ui_utils.c

// pause_for_user moved to ui_utils.c

// clear_screen moved to ui_utils.c

// =====================================================
// Voting Algorithm Handler
// =====================================================

void handle_voting_algorithm(void)
{
    clear_screen();
    printf(BOLD CYAN "🗳️  VoteMe Voting Algorithm\n" RESET);
    printf("═══════════════════════════════════════\n\n");

    // Display current voting parameters
    display_voting_parameters(sys_config.min_votes_for_parliament, sys_config.max_parliament_members);

    // Check if voting is enabled
    if (!sys_config.voting_enabled)
    {
        printf(RED "\n❌ Warning: Voting is currently disabled in system configuration!\n" RESET);
        printf(YELLOW "Would you like to enable voting now? (y/n): " RESET);

        char response;
        if (scanf(" %c", &response) != 1)
        {
            response = 'n';
        }

        if (response == 'y' || response == 'Y')
        {
            sys_config.voting_enabled = 1;
            save_system_config();
            printf(GREEN "✅ Voting has been enabled!\n" RESET);
        }
        else
        {
            printf(YELLOW "⚠️  Voting remains disabled. Algorithm execution cancelled.\n" RESET);
            pause_for_user();
            return;
        }
    }

    // Check if votes file exists, create sample if not
    FILE *votes_check = fopen("data/votes.txt", "r");
    if (!votes_check)
    {
        printf(YELLOW "\n⚠️  No votes file found. Would you like to create sample votes for testing? (y/n): " RESET);

        char response;
        if (scanf(" %c", &response) != 1)
        {
            response = 'n';
        }

        if (response == 'y' || response == 'Y')
        {
            if (create_sample_votes_file() == DATA_SUCCESS)
            {
                printf(GREEN "✅ Sample votes file created successfully!\n" RESET);
            }
            else
            {
                printf(RED "❌ Failed to create sample votes file!\n" RESET);
                pause_for_user();
                return;
            }
        }
        else
        {
            printf(YELLOW "⚠️  No votes file available. Algorithm execution cancelled.\n" RESET);
            pause_for_user();
            return;
        }
    }
    else
    {
        fclose(votes_check);
        printf(GREEN "✅ Votes file found and ready for processing.\n" RESET);
    }

    // Confirm execution
    printf(BOLD YELLOW "\n🔥 Ready to execute voting algorithm with the following parameters:\n" RESET);
    printf("   • Minimum votes for parliament: %d\n", sys_config.min_votes_for_parliament);
    printf("   • Maximum parliament members: %d\n", sys_config.max_parliament_members);
    printf("\nThis will process all votes and generate the official results.\n");
    printf(BOLD "Are you sure you want to proceed? (y/n): " RESET);

    char confirm;
    if (scanf(" %c", &confirm) != 1)
    {
        confirm = 'n';
    }

    if (confirm != 'y' && confirm != 'Y')
    {
        printf(YELLOW "⚠️  Voting algorithm execution cancelled by user.\n" RESET);
        pause_for_user();
        return;
    }

    // Execute the voting algorithm
    printf(BOLD GREEN "\n🚀 Starting voting algorithm execution...\n" RESET);

    int result = execute_voting_algorithm(sys_config.min_votes_for_parliament,
                                          sys_config.max_parliament_members);

    if (result == DATA_SUCCESS)
    {
        printf(BOLD GREEN "\n🎉 Voting algorithm completed successfully!\n" RESET);
        printf("📄 Results have been saved to 'data/voting_results.txt'\n");
        printf("📊 Parliament members have been selected according to the configured parameters.\n");
    }
    else
    {
        printf(BOLD RED "\n❌ Voting algorithm failed with error code: %d\n" RESET, result);
        printf("🔧 Please check the system configuration and data files.\n");
    }

    pause_for_user();
}
