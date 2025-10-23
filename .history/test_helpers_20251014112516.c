#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function prototypes (copied from main.c)
char* generate_next_voter_id(void);
char* generate_next_candidate_id(void);
int check_nic_exists_voters(const char *nic);
int check_nic_exists_candidates(const char *nic);
void display_districts(void);
int validate_district_id(const char *district_id);
void display_parties(void);
int validate_party_id(const char *party_id);

// Color definitions
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define CYAN "\033[36m"

int main()
{
    printf(BOLD CYAN "🧪 Testing Enhanced VoteMe Helper Functions\n" RESET);
    printf("===========================================\n\n");

    // Test 1: Auto-generated IDs
    printf(YELLOW "Test 1: Auto-generated IDs\n" RESET);
    printf("─────────────────────────\n");
    char *next_voter_id = generate_next_voter_id();
    char *next_candidate_id = generate_next_candidate_id();
    
    if (next_voter_id) {
        printf(GREEN "✅ Next Voter ID: %s\n" RESET, next_voter_id);
        free(next_voter_id);
    } else {
        printf(RED "❌ Failed to generate voter ID\n" RESET);
    }
    
    if (next_candidate_id) {
        printf(GREEN "✅ Next Candidate ID: %s\n" RESET, next_candidate_id);
        free(next_candidate_id);
    } else {
        printf(RED "❌ Failed to generate candidate ID\n" RESET);
    }
    printf("\n");

    // Test 2: NIC validation
    printf(YELLOW "Test 2: NIC Duplicate Checking\n" RESET);
    printf("──────────────────────────────\n");
    
    // Test with existing NIC
    const char *existing_nic = "123456789V";
    if (check_nic_exists_voters(existing_nic)) {
        printf(GREEN "✅ Correctly detected existing voter NIC: %s\n" RESET, existing_nic);
    } else {
        printf(RED "❌ Failed to detect existing voter NIC: %s\n" RESET, existing_nic);
    }
    
    // Test with new NIC
    const char *new_nic = "999999999V";
    if (!check_nic_exists_voters(new_nic)) {
        printf(GREEN "✅ Correctly identified new NIC as available: %s\n" RESET, new_nic);
    } else {
        printf(RED "❌ Incorrectly flagged new NIC as existing: %s\n" RESET, new_nic);
    }
    printf("\n");

    // Test 3: District display and validation
    printf(YELLOW "Test 3: District Display and Validation\n" RESET);
    printf("──────────────────────────────────────\n");
    display_districts();
    printf("\n");
    
    // Test district validation
    if (validate_district_id("D01")) {
        printf(GREEN "✅ Valid district D01 correctly validated\n" RESET);
    } else {
        printf(RED "❌ Valid district D01 incorrectly rejected\n" RESET);
    }
    
    if (!validate_district_id("D99")) {
        printf(GREEN "✅ Invalid district D99 correctly rejected\n" RESET);
    } else {
        printf(RED "❌ Invalid district D99 incorrectly accepted\n" RESET);
    }
    printf("\n");

    // Test 4: Party display and validation
    printf(YELLOW "Test 4: Party Display and Validation\n" RESET);
    printf("───────────────────────────────────\n");
    display_parties();
    printf("\n");
    
    // Test party validation
    if (validate_party_id("P001")) {
        printf(GREEN "✅ Valid party P001 correctly validated\n" RESET);
    } else {
        printf(RED "❌ Valid party P001 incorrectly rejected\n" RESET);
    }
    
    if (!validate_party_id("P99")) {
        printf(GREEN "✅ Invalid party P99 correctly rejected\n" RESET);
    } else {
        printf(RED "❌ Invalid party P99 incorrectly accepted\n" RESET);
    }
    
    printf("\n" BOLD GREEN "🎉 All helper function tests completed!\n" RESET);
    return 0;
}

// Helper function implementations (from main.c)
// ... (all the helper functions would be included here)