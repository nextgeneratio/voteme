/*
 * VoteMe - Complete Voting System
 *
 * Main application entry point with integrated console interface
 * Features:
 * - Voter and Candidate Management
 * - Admin System with CRUD Operations
 * - Voting Algorithm Execution
 * - Data Viewing and System Configuration
 * - Beautiful Console UI with Colors
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "data_handler_enhanced.h"
#include "voting.h"

// Include original registration functions
int add_voter(const char *voting_number, const char *nic, const char *name);
int view_voters(void);
int add_candidate(const char *candidate_number, const char *nic, const char *party_id, const char *name);
int view_candidates(void);

// Color codes for console UI
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define WHITE "\033[0;37m"
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define BRIGHT_GREEN "\033[1;32m"
#define BRIGHT_BLUE "\033[1;34m"
#define BRIGHT_CYAN "\033[1;36m"

// System configuration structure (simplified for main.c)
typedef struct
{
	int max_voters;
	int max_candidates;
	int min_votes_for_parliament;
	int max_parliament_members;
	int voting_enabled;
} simple_config_t;

// Global configuration
static simple_config_t config = {
	.max_voters = 10000,
	.max_candidates = 500,
	.min_votes_for_parliament = 100,
	.max_parliament_members = 225,
	.voting_enabled = 1};

// Function declarations
void display_main_banner(void);
void display_main_menu(void);
void clear_screen(void);
void pause_for_user(void);
int get_user_choice(const char *prompt, int min, int max);
void handle_voter_management(void);
void handle_candidate_management(void);
void handle_voting_process(void);
void handle_data_viewing(void);
void handle_system_config(void);
void display_system_status(void);
void create_sample_data(void);
int count_records_in_file(const char *filename);

// Helper function declarations for enhanced registration
char* generate_next_voter_id(void);
char* generate_next_candidate_id(void);
int check_nic_exists_voters(const char *nic);
int check_nic_exists_candidates(const char *nic);
void display_districts(void);
int validate_district_id(const char *district_id);
void display_parties(void);
int validate_party_id(const char *party_id);

int main()
{
	// Initialize system
	clear_screen();
	display_main_banner();

	// Ensure data directory exists
	if (system("mkdir -p data") != 0)
	{
		printf(RED "❌ Error: Failed to create data directory!\n" RESET);
		return 1;
	}

	// Create basic data files if they don't exist
	create_sample_data();

	printf(BRIGHT_GREEN "🎉 Welcome to VoteMe - Complete Voting System!\n" RESET);
	printf(CYAN "System initialized successfully. Ready to proceed.\n\n" RESET);
	pause_for_user();

	int choice;
	do
	{
		display_main_menu();
		choice = get_user_choice("Enter your choice", 0, 6);

		switch (choice)
		{
		case 1:
			handle_voter_management();
			break;
		case 2:
			handle_candidate_management();
			break;
		case 3:
			handle_voting_process();
			break;
		case 4:
			handle_data_viewing();
			break;
		case 5:
			handle_system_config();
			break;
		case 6:
			display_system_status();
			pause_for_user();
			break;
		case 0:
			printf(BRIGHT_GREEN "\n👋 Thank you for using VoteMe!\n" RESET);
			printf(CYAN "🗳️  Democracy in action - Every vote counts!\n" RESET);
			break;
		default:
			printf(RED "❌ Invalid choice! Please try again.\n" RESET);
			pause_for_user();
			break;
		}
	} while (choice != 0);

	return 0;
}

void display_main_banner(void)
{
	printf(BOLD BRIGHT_CYAN);
	printf("╔══════════════════════════════════════════════════════════════════════════════╗\n");
	printf("║                                                                              ║\n");
	printf("║                        " BRIGHT_GREEN "🗳️  VoteMe - Voting System 🗳️" BRIGHT_CYAN "                        ║\n");
	printf("║                                                                              ║\n");
	printf("║                    " WHITE "Complete Electoral Management Platform" BRIGHT_CYAN "                     ║\n");
	printf("║                                                                              ║\n");
	printf("║══════════════════════════════════════════════════════════════════════════════║\n");
	printf("║  " BRIGHT_GREEN "✓ Voter Registration & Management" BRIGHT_CYAN "   ║  " BRIGHT_GREEN "✓ Advanced Voting Algorithm" BRIGHT_CYAN "      ║\n");
	printf("║  " BRIGHT_GREEN "✓ Candidate Registration System" BRIGHT_CYAN "    ║  " BRIGHT_GREEN "✓ Real-time Data Management" BRIGHT_CYAN "      ║\n");
	printf("║  " BRIGHT_GREEN "✓ Democratic Voting Process" BRIGHT_CYAN "        ║  " BRIGHT_GREEN "✓ Comprehensive Reporting" BRIGHT_CYAN "        ║\n");
	printf("║  " BRIGHT_GREEN "✓ System Administration Tools" BRIGHT_CYAN "      ║  " BRIGHT_GREEN "✓ Beautiful Console Interface" BRIGHT_CYAN "    ║\n");
	printf("╚══════════════════════════════════════════════════════════════════════════════╝\n");
	printf(RESET "\n");
}

void display_main_menu(void)
{
	clear_screen();
	printf(BOLD BRIGHT_BLUE "🏛️  VoteMe - Main System Menu\n" RESET);
	printf("═══════════════════════════════════════════════════════════════════════════════\n\n");

	printf(YELLOW "1." RESET " 👥 " BOLD "Voter Management" RESET "     - Register, view, and manage voters\n");
	printf(YELLOW "2." RESET " 🏆 " BOLD "Candidate Management" RESET "  - Register, view, and manage candidates\n");
	printf(YELLOW "3." RESET " 🗳️  " BOLD "Voting Process" RESET "       - Execute voting algorithm and view results\n");
	printf(YELLOW "4." RESET " 📊 " BOLD "Data Viewing" RESET "         - Browse and analyze system data\n");
	printf(YELLOW "5." RESET " ⚙️  " BOLD "System Configuration" RESET " - Configure voting parameters and limits\n");
	printf(YELLOW "6." RESET " 📈 " BOLD "System Status" RESET "        - View current system information\n");
	printf(YELLOW "0." RESET " 🚪 " BOLD "Exit System" RESET "          - Save and quit application\n\n");

	// Quick status bar
	printf(CYAN "═══════════════════════════════════════════════════════════════════════════════\n");
	printf("Quick Status: " RESET);
	printf("Voters: " BRIGHT_GREEN "%d" RESET "/%d | ",
		   count_records_in_file("data/approved_voters.txt"), config.max_voters);
	printf("Candidates: " BRIGHT_GREEN "%d" RESET "/%d | ",
		   count_records_in_file("data/approved_candidates.txt"), config.max_candidates);
	printf("Voting: %s\n",
		   config.voting_enabled ? BRIGHT_GREEN "ENABLED" RESET : RED "DISABLED" RESET);
	printf(CYAN "═══════════════════════════════════════════════════════════════════════════════\n\n" RESET);
}

void handle_voter_management(void)
{
	int choice;
	do
	{
		clear_screen();
		printf(BOLD BRIGHT_BLUE "👥 Voter Management System\n" RESET);
		printf("═══════════════════════════════════════════════════════════════════════════════\n\n");

		printf(YELLOW "1." RESET " ➕ Add New Voter\n");
		printf(YELLOW "2." RESET " 📋 View All Voters\n");
		printf(YELLOW "3." RESET " 🔍 Search Voter\n");
		printf(YELLOW "4." RESET " 📊 Voter Statistics\n");
		printf(YELLOW "0." RESET " ⬅️  Back to Main Menu\n\n");

		choice = get_user_choice("Enter your choice", 0, 4);

		switch (choice)
		{
		case 1:
		{
			printf(BRIGHT_CYAN "\n➕ Adding New Voter\n" RESET);
			printf("═══════════════════════════════════════════════════════════════════════════════\n");

			char nic[20], name[100], district_id[20];
			char *voting_number;

			// Auto-generate voter ID
			voting_number = generate_next_voter_id();
			if (!voting_number)
			{
				printf(RED "❌ Failed to generate voter ID!\n" RESET);
				break;
			}

			printf(BRIGHT_GREEN "📋 Auto-generated Voter ID: %s\n" RESET, voting_number);
			printf("─────────────────────────────────────────────────────────────────────────────\n");

			// Get NIC with validation
			while (1)
			{
				printf("Enter NIC: ");
				if (scanf("%19s", nic) != 1)
				{
					printf(RED "❌ Invalid input!\n" RESET);
					continue;
				}

				// Check for duplicate NIC
				if (check_nic_exists_voters(nic) || check_nic_exists_candidates(nic))
				{
					printf(RED "❌ Error: This NIC is already registered!\n" RESET);
					printf(YELLOW "Please enter a different NIC: " RESET);
					continue;
				}
				break;
			}

			// Get full name
			printf("Enter Full Name: ");
			getchar(); // consume newline
			if (fgets(name, sizeof(name), stdin) == NULL)
			{
				printf(RED "❌ Invalid input!\n" RESET);
				free(voting_number);
				break;
			}
			name[strcspn(name, "\n")] = 0; // remove newline

			// Display districts and get selection
			printf("\n🗺️  Available Districts:\n");
			printf("─────────────────────────────────────────────────────────────────────────────\n");
			display_districts();
			printf("─────────────────────────────────────────────────────────────────────────────\n");

			while (1)
			{
				printf("Enter District ID: ");
				if (scanf("%19s", district_id) != 1)
				{
					printf(RED "❌ Invalid input!\n" RESET);
					continue;
				}

				if (!validate_district_id(district_id))
				{
					printf(RED "❌ Error: District ID '%s' is not available!\n" RESET, district_id);
					printf(YELLOW "Please choose from the available districts above.\n" RESET);
					continue;
				}
				break;
			}

			// Create voter with enhanced data handler
			if (create_voter(voting_number, name, nic, district_id) == DATA_SUCCESS)
			{
				printf(BRIGHT_GREEN "\n✅ Voter registered successfully!\n" RESET);
				printf("📋 Voter ID: " CYAN "%s" RESET "\n", voting_number);
				printf("👤 Name: " CYAN "%s" RESET "\n", name);
				printf("🆔 NIC: " CYAN "%s" RESET "\n", nic);
				printf("🗺️  District: " CYAN "%s" RESET "\n", district_id);
			}
			else
			{
				printf(RED "❌ Failed to register voter!\n" RESET);
			}

			free(voting_number);
			pause_for_user();
			break;
		}
		case 2:
			printf(BRIGHT_CYAN "\n📋 All Registered Voters\n" RESET);
			printf("═══════════════════════════════════════════════════════════════════════════════\n");
			view_voters();
			pause_for_user();
			break;
		case 3:
			printf(BRIGHT_CYAN "\n🔍 Voter Search\n" RESET);
			printf("═══════════════════════════════════════════════════════════════════════════════\n");
			printf(YELLOW "Feature coming soon - Search by voting number or name\n" RESET);
			pause_for_user();
			break;
		case 4:
			printf(BRIGHT_CYAN "\n📊 Voter Statistics\n" RESET);
			printf("═══════════════════════════════════════════════════════════════════════════════\n");
			printf("Total Registered Voters: " BRIGHT_GREEN "%d" RESET "\n",
				   count_records_in_file("data/approved_voters.txt"));
			printf("Maximum Allowed Voters: " CYAN "%d" RESET "\n", config.max_voters);
			printf("Registration Capacity: " YELLOW "%.1f%%" RESET "\n",
				   (float)count_records_in_file("data/approved_voters.txt") / config.max_voters * 100);
			pause_for_user();
			break;
		}
	} while (choice != 0);
}

void handle_candidate_management(void)
{
	int choice;
	do
	{
		clear_screen();
		printf(BOLD BRIGHT_BLUE "🏆 Candidate Management System\n" RESET);
		printf("═══════════════════════════════════════════════════════════════════════════════\n\n");

		printf(YELLOW "1." RESET " ➕ Add New Candidate\n");
		printf(YELLOW "2." RESET " 📋 View All Candidates\n");
		printf(YELLOW "3." RESET " 🔍 Search Candidate\n");
		printf(YELLOW "4." RESET " 📊 Candidate Statistics\n");
		printf(YELLOW "0." RESET " ⬅️  Back to Main Menu\n\n");

		choice = get_user_choice("Enter your choice", 0, 4);

		switch (choice)
		{
		case 1:
		{
			printf(BRIGHT_CYAN "\n➕ Adding New Candidate\n" RESET);
			printf("═══════════════════════════════════════════════════════════════════════════════\n");

			char candidate_number[20], nic[20], party_id[20], name[100], district_id[20];

			printf("Enter Candidate Number: ");
			if (scanf("%19s", candidate_number) != 1)
			{
				printf(RED "❌ Invalid input!\n" RESET);
				break;
			}

			printf("Enter Full Name: ");
			getchar(); // consume newline
			if (fgets(name, sizeof(name), stdin) == NULL)
			{
				printf(RED "❌ Invalid input!\n" RESET);
				break;
			}
			name[strcspn(name, "\n")] = 0; // remove newline

			printf("Enter Party ID: ");
			if (scanf("%19s", party_id) != 1)
			{
				printf(RED "❌ Invalid input!\n" RESET);
				break;
			}

			printf("Enter District ID: ");
			if (scanf("%19s", district_id) != 1)
			{
				printf(RED "❌ Invalid input!\n" RESET);
				break;
			}

			printf("Enter NIC: ");
			if (scanf("%19s", nic) != 1)
			{
				printf(RED "❌ Invalid input!\n" RESET);
				break;
			}

			if (create_candidate(candidate_number, name, party_id, district_id, nic) == DATA_SUCCESS)
			{
				printf(BRIGHT_GREEN "✅ Candidate added successfully!\n" RESET);
			}
			else
			{
				printf(RED "❌ Failed to add candidate!\n" RESET);
			}
			pause_for_user();
			break;
		}
		case 2:
			printf(BRIGHT_CYAN "\n📋 All Registered Candidates\n" RESET);
			printf("═══════════════════════════════════════════════════════════════════════════════\n");
			view_candidates();
			pause_for_user();
			break;
		case 3:
			printf(BRIGHT_CYAN "\n🔍 Candidate Search\n" RESET);
			printf("═══════════════════════════════════════════════════════════════════════════════\n");
			printf(YELLOW "Feature coming soon - Search by candidate number or name\n" RESET);
			pause_for_user();
			break;
		case 4:
			printf(BRIGHT_CYAN "\n📊 Candidate Statistics\n" RESET);
			printf("═══════════════════════════════════════════════════════════════════════════════\n");
			printf("Total Registered Candidates: " BRIGHT_GREEN "%d" RESET "\n",
				   count_records_in_file("data/approved_candidates.txt"));
			printf("Maximum Allowed Candidates: " CYAN "%d" RESET "\n", config.max_candidates);
			printf("Registration Capacity: " YELLOW "%.1f%%" RESET "\n",
				   (float)count_records_in_file("data/approved_candidates.txt") / config.max_candidates * 100);
			pause_for_user();
			break;
		}
	} while (choice != 0);
}

void handle_voting_process(void)
{
	clear_screen();
	printf(BOLD BRIGHT_BLUE "🗳️  Voting Process Management\n" RESET);
	printf("═══════════════════════════════════════════════════════════════════════════════\n\n");

	// Display voting parameters
	printf(BOLD YELLOW "⚙️  Current Voting Parameters:\n" RESET);
	printf("┌─────────────────────────────────────────────────────────────────────────────┐\n");
	printf("│ " BLUE "Minimum Votes for Parliament:" RESET "    %-8d                        │\n", config.min_votes_for_parliament);
	printf("│ " BLUE "Maximum Parliament Members:" RESET "      %-8d                        │\n", config.max_parliament_members);
	printf("│ " BLUE "Voting Status:" RESET "                  %-12s                    │\n",
		   config.voting_enabled ? BRIGHT_GREEN "ENABLED" RESET : RED "DISABLED" RESET);
	printf("└─────────────────────────────────────────────────────────────────────────────┘\n\n");

	if (!config.voting_enabled)
	{
		printf(RED "⚠️  Warning: Voting is currently disabled!\n" RESET);
		printf(YELLOW "Please enable voting in System Configuration before proceeding.\n" RESET);
		pause_for_user();
		return;
	}

	// Check if votes file exists
	FILE *votes_check = fopen("data/votes.txt", "r");
	if (!votes_check)
	{
		printf(YELLOW "⚠️  No votes file found. Creating sample votes for demonstration...\n" RESET);
		if (create_sample_votes_file() == DATA_SUCCESS)
		{
			printf(BRIGHT_GREEN "✅ Sample votes created successfully!\n" RESET);
		}
		else
		{
			printf(RED "❌ Failed to create sample votes!\n" RESET);
			pause_for_user();
			return;
		}
	}
	else
	{
		fclose(votes_check);
		printf(BRIGHT_GREEN "✅ Votes file found and ready for processing.\n" RESET);
	}

	printf(BOLD YELLOW "\n🔥 Ready to execute voting algorithm!\n" RESET);
	printf("This will process all votes and generate official results.\n");
	printf(BOLD "Do you want to proceed? (y/n): " RESET);

	char confirm;
	if (scanf(" %c", &confirm) != 1)
	{
		confirm = 'n';
	}

	if (confirm == 'y' || confirm == 'Y')
	{
		printf(BOLD BRIGHT_GREEN "\n🚀 Executing voting algorithm...\n" RESET);

		int result = execute_voting_algorithm(config.min_votes_for_parliament,
											  config.max_parliament_members);

		if (result == DATA_SUCCESS)
		{
			printf(BOLD BRIGHT_GREEN "\n🎉 Voting algorithm completed successfully!\n" RESET);
			printf("📄 Detailed results have been saved to 'data/voting_results.txt'\n");
			printf("🏛️  Parliament members selected according to configured parameters.\n");
		}
		else
		{
			printf(BOLD RED "\n❌ Voting algorithm failed!\n" RESET);
			printf("🔧 Please check system configuration and data files.\n");
		}
	}
	else
	{
		printf(YELLOW "⚠️  Voting algorithm execution cancelled.\n" RESET);
	}

	pause_for_user();
}

void handle_data_viewing(void)
{
	int choice;
	do
	{
		clear_screen();
		printf(BOLD BRIGHT_BLUE "📊 Data Viewing & Analysis\n" RESET);
		printf("═══════════════════════════════════════════════════════════════════════════════\n\n");

		printf(YELLOW "1." RESET " 👥 View Voter Data\n");
		printf(YELLOW "2." RESET " 🏆 View Candidate Data\n");
		printf(YELLOW "3." RESET " 🗳️  View Voting Results\n");
		printf(YELLOW "4." RESET " 🎯 View Party Information\n");
		printf(YELLOW "5." RESET " 🗺️  View District Information\n");
		printf(YELLOW "6." RESET " 📈 System Statistics\n");
		printf(YELLOW "0." RESET " ⬅️  Back to Main Menu\n\n");

		choice = get_user_choice("Enter your choice", 0, 6);

		switch (choice)
		{
		case 1:
			printf(BRIGHT_CYAN "\n👥 Voter Data\n" RESET);
			printf("═══════════════════════════════════════════════════════════════════════════════\n");
			system("cat data/approved_voters.txt 2>/dev/null || echo 'No voter data found'");
			pause_for_user();
			break;
		case 2:
			printf(BRIGHT_CYAN "\n🏆 Candidate Data\n" RESET);
			printf("═══════════════════════════════════════════════════════════════════════════════\n");
			system("cat data/approved_candidates.txt 2>/dev/null || echo 'No candidate data found'");
			pause_for_user();
			break;
		case 3:
			printf(BRIGHT_CYAN "\n🗳️  Voting Results\n" RESET);
			printf("═══════════════════════════════════════════════════════════════════════════════\n");
			system("cat data/voting_results.txt 2>/dev/null || echo 'No voting results found - run voting process first'");
			pause_for_user();
			break;
		case 4:
			printf(BRIGHT_CYAN "\n🎯 Party Information\n" RESET);
			printf("═══════════════════════════════════════════════════════════════════════════════\n");
			system("cat data/party_name.txt 2>/dev/null || echo 'No party data found'");
			pause_for_user();
			break;
		case 5:
			printf(BRIGHT_CYAN "\n🗺️  District Information\n" RESET);
			printf("═══════════════════════════════════════════════════════════════════════════════\n");
			system("cat data/district.txt 2>/dev/null || echo 'No district data found'");
			pause_for_user();
			break;
		case 6:
			printf(BRIGHT_CYAN "\n📈 System Statistics\n" RESET);
			printf("═══════════════════════════════════════════════════════════════════════════════\n");
			printf("📊 " BOLD "Data Files Summary:\n" RESET);
			printf("   • Voters: " BRIGHT_GREEN "%d" RESET " registered\n", count_records_in_file("data/approved_voters.txt"));
			printf("   • Candidates: " BRIGHT_GREEN "%d" RESET " registered\n", count_records_in_file("data/approved_candidates.txt"));
			printf("   • Parties: " BRIGHT_GREEN "%d" RESET " registered\n", count_records_in_file("data/party_name.txt"));
			printf("   • Districts: " BRIGHT_GREEN "%d" RESET " registered\n", count_records_in_file("data/district.txt"));
			pause_for_user();
			break;
		}
	} while (choice != 0);
}

void handle_system_config(void)
{
	int choice;
	do
	{
		clear_screen();
		printf(BOLD BRIGHT_BLUE "⚙️  System Configuration\n" RESET);
		printf("═══════════════════════════════════════════════════════════════════════════════\n\n");

		printf(BOLD YELLOW "Current Configuration:\n" RESET);
		printf("┌─────────────────────────────────────────────────────────────────────────────┐\n");
		printf("│ " BLUE "Maximum Voters:" RESET "              %-8d                        │\n", config.max_voters);
		printf("│ " BLUE "Maximum Candidates:" RESET "          %-8d                        │\n", config.max_candidates);
		printf("│ " BLUE "Minimum Votes for Parliament:" RESET " %-8d                        │\n", config.min_votes_for_parliament);
		printf("│ " BLUE "Maximum Parliament Members:" RESET "   %-8d                        │\n", config.max_parliament_members);
		printf("│ " BLUE "Voting Enabled:" RESET "              %-8s                        │\n", config.voting_enabled ? "YES" : "NO");
		printf("└─────────────────────────────────────────────────────────────────────────────┘\n\n");

		printf(YELLOW "1." RESET " 📊 Change Maximum Voters\n");
		printf(YELLOW "2." RESET " 🏆 Change Maximum Candidates\n");
		printf(YELLOW "3." RESET " 🏛️  Change Minimum Votes for Parliament\n");
		printf(YELLOW "4." RESET " 👥 Change Maximum Parliament Members\n");
		printf(YELLOW "5." RESET " 🗳️  Toggle Voting Enable/Disable\n");
		printf(YELLOW "6." RESET " 💾 Save Configuration\n");
		printf(YELLOW "0." RESET " ⬅️  Back to Main Menu\n\n");

		choice = get_user_choice("Enter your choice", 0, 6);

		switch (choice)
		{
		case 1:
			printf("Enter new maximum voters: ");
			if (scanf("%d", &config.max_voters) == 1 && config.max_voters > 0)
			{
				printf(BRIGHT_GREEN "✅ Maximum voters updated to %d\n" RESET, config.max_voters);
			}
			else
			{
				printf(RED "❌ Invalid input!\n" RESET);
			}
			pause_for_user();
			break;
		case 2:
			printf("Enter new maximum candidates: ");
			if (scanf("%d", &config.max_candidates) == 1 && config.max_candidates > 0)
			{
				printf(BRIGHT_GREEN "✅ Maximum candidates updated to %d\n" RESET, config.max_candidates);
			}
			else
			{
				printf(RED "❌ Invalid input!\n" RESET);
			}
			pause_for_user();
			break;
		case 3:
			printf("Enter new minimum votes for parliament: ");
			if (scanf("%d", &config.min_votes_for_parliament) == 1 && config.min_votes_for_parliament >= 0)
			{
				printf(BRIGHT_GREEN "✅ Minimum votes for parliament updated to %d\n" RESET, config.min_votes_for_parliament);
			}
			else
			{
				printf(RED "❌ Invalid input!\n" RESET);
			}
			pause_for_user();
			break;
		case 4:
			printf("Enter new maximum parliament members: ");
			if (scanf("%d", &config.max_parliament_members) == 1 && config.max_parliament_members > 0)
			{
				printf(BRIGHT_GREEN "✅ Maximum parliament members updated to %d\n" RESET, config.max_parliament_members);
			}
			else
			{
				printf(RED "❌ Invalid input!\n" RESET);
			}
			pause_for_user();
			break;
		case 5:
			config.voting_enabled = !config.voting_enabled;
			printf(BRIGHT_GREEN "✅ Voting %s\n" RESET, config.voting_enabled ? "ENABLED" : "DISABLED");
			pause_for_user();
			break;
		case 6:
			printf(BRIGHT_GREEN "✅ Configuration saved!\n" RESET);
			printf(YELLOW "Note: Configuration is stored in memory for this session.\n" RESET);
			pause_for_user();
			break;
		}
	} while (choice != 0);
}

void display_system_status(void)
{
	clear_screen();
	printf(BOLD BRIGHT_BLUE "📈 VoteMe System Status\n" RESET);
	printf("═══════════════════════════════════════════════════════════════════════════════\n\n");

	printf(BOLD YELLOW "🎯 System Health:\n" RESET);
	printf("   • Application Status: " BRIGHT_GREEN "RUNNING" RESET "\n");
	printf("   • Data Directory: " BRIGHT_GREEN "ACCESSIBLE" RESET "\n");
	printf("   • Voting System: %s\n", config.voting_enabled ? BRIGHT_GREEN "ENABLED" RESET : RED "DISABLED" RESET);

	printf(BOLD YELLOW "\n📊 Data Summary:\n" RESET);
	printf("   • Registered Voters: " BRIGHT_GREEN "%d" RESET " / %d (%.1f%% capacity)\n",
		   count_records_in_file("data/approved_voters.txt"), config.max_voters,
		   (float)count_records_in_file("data/approved_voters.txt") / config.max_voters * 100);
	printf("   • Registered Candidates: " BRIGHT_GREEN "%d" RESET " / %d (%.1f%% capacity)\n",
		   count_records_in_file("data/approved_candidates.txt"), config.max_candidates,
		   (float)count_records_in_file("data/approved_candidates.txt") / config.max_candidates * 100);
	printf("   • Political Parties: " BRIGHT_GREEN "%d" RESET " registered\n", count_records_in_file("data/party_name.txt"));
	printf("   • Electoral Districts: " BRIGHT_GREEN "%d" RESET " configured\n", count_records_in_file("data/district.txt"));

	printf(BOLD YELLOW "\n⚙️  Configuration:\n" RESET);
	printf("   • Parliament Size Limit: " CYAN "%d" RESET " members\n", config.max_parliament_members);
	printf("   • Vote Threshold: " CYAN "%d" RESET " minimum votes for parliament\n", config.min_votes_for_parliament);

	printf(BOLD YELLOW "\n🗳️  Voting Status:\n" RESET);
	FILE *votes_file = fopen("data/votes.txt", "r");
	if (votes_file)
	{
		fclose(votes_file);
		printf("   • Votes File: " BRIGHT_GREEN "AVAILABLE" RESET "\n");
	}
	else
	{
		printf("   • Votes File: " YELLOW "NOT FOUND" RESET " (will create sample data when needed)\n");
	}

	FILE *results_file = fopen("data/voting_results.txt", "r");
	if (results_file)
	{
		fclose(results_file);
		printf("   • Latest Results: " BRIGHT_GREEN "AVAILABLE" RESET "\n");
	}
	else
	{
		printf("   • Latest Results: " YELLOW "NOT GENERATED" RESET " (run voting process to create)\n");
	}

	printf("\n" BRIGHT_GREEN "🎉 System is ready for electoral operations!\n" RESET);
}

void create_sample_data(void)
{
	// Create basic data files with headers if they don't exist
	FILE *file;

	// Voters file
	file = fopen("data/approved_voters.txt", "r");
	if (!file)
	{
		file = fopen("data/approved_voters.txt", "w");
		if (file)
		{
			fprintf(file, "voting_number,name,nic,district_id\n");
			fclose(file);
		}
	}
	else
	{
		fclose(file);
	}

	// Candidates file
	file = fopen("data/approved_candidates.txt", "r");
	if (!file)
	{
		file = fopen("data/approved_candidates.txt", "w");
		if (file)
		{
			fprintf(file, "candidate_number,name,party_id,district_id,nic\n");
			fclose(file);
		}
	}
	else
	{
		fclose(file);
	}

	// Party file
	file = fopen("data/party_name.txt", "r");
	if (!file)
	{
		file = fopen("data/party_name.txt", "w");
		if (file)
		{
			fprintf(file, "party_id,party_name\n");
			fclose(file);
		}
	}
	else
	{
		fclose(file);
	}

	// District file
	file = fopen("data/district.txt", "r");
	if (!file)
	{
		file = fopen("data/district.txt", "w");
		if (file)
		{
			fprintf(file, "district_id,district_name\n");
			fclose(file);
		}
	}
	else
	{
		fclose(file);
	}
}

int count_records_in_file(const char *filename)
{
	FILE *file = fopen(filename, "r");
	if (!file)
		return 0;

	int count = 0;
	char line[256];

	// Skip header line
	if (fgets(line, sizeof(line), file))
	{
		while (fgets(line, sizeof(line), file))
		{
			if (strlen(line) > 1)
			{ // Ignore empty lines
				count++;
			}
		}
	}

	fclose(file);
	return count;
}

int get_user_choice(const char *prompt, int min, int max)
{
	int choice;
	char buffer[100];

	while (1)
	{
		printf(BOLD CYAN "%s (%d-%d): " RESET, prompt, min, max);

		if (fgets(buffer, sizeof(buffer), stdin) != NULL)
		{
			if (sscanf(buffer, "%d", &choice) == 1)
			{
				if (choice >= min && choice <= max)
				{
					return choice;
				}
			}
		}

		printf(RED "❌ Error: Invalid input! Please enter a number between %d and %d.\n" RESET, min, max);
	}
}

void clear_screen(void)
{
#ifdef _WIN32
	if (system("cls") != 0)
	{ /* Ignore */
	}
#else
	if (system("clear") != 0)
	{ /* Ignore */
	}
#endif
}

void pause_for_user(void)
{
	printf("\n" CYAN "Press Enter to continue..." RESET);
	getchar();
}
