#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../../src/data_handler.h"

// Test result tracking
int entity_tests_passed = 0;
int entity_tests_failed = 0;

// Color codes for output
#define GREEN "\033[32m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

void print_entity_test_header(const char *test_name)
{
    printf("\n" BLUE "=== Testing %s ===" RESET "\n", test_name);
}

void assert_entity_test(int condition, const char *test_description)
{
    if (condition)
    {
        printf(GREEN "✓ PASS: %s" RESET "\n", test_description);
        entity_tests_passed++;
    }
    else
    {
        printf(RED "✗ FAIL: %s" RESET "\n", test_description);
        entity_tests_failed++;
    }
}

void print_entity_summary()
{
    printf("\n" YELLOW "=== Entity Function Test Summary ===" RESET "\n");
    printf("Tests Passed: " GREEN "%d" RESET "\n", entity_tests_passed);
    printf("Tests Failed: " RED "%d" RESET "\n", entity_tests_failed);
    printf("Total Tests: %d\n", entity_tests_passed + entity_tests_failed);

    if (entity_tests_failed == 0)
    {
        printf(GREEN "All entity tests passed!" RESET "\n");
    }
    else
    {
        printf(RED "Some entity tests failed. Please check the implementation." RESET "\n");
    }
}

void setup_entity_test_data()
{
    printf(YELLOW "Setting up entity test data..." RESET "\n");

    // Setup voters file
    FILE *fp = fopen("../../data/approved_voters.txt", "w");
    if (fp)
    {
        fprintf(fp, "voting_number,name,NIC,district_id\n");
        fprintf(fp, "V001,Alice Smith,199512345678,D001\n");
        fprintf(fp, "V002,Bob Johnson,198876543210,D002\n");
        fclose(fp);
    }

    // Setup candidates file
    fp = fopen("../../data/approved_candidates.txt", "w");
    if (fp)
    {
        fprintf(fp, "candidate_number,name,party_id,district_id,nic\n");
        fprintf(fp, "C001,John Doe,P001,D001,198012345678\n");
        fprintf(fp, "C002,Jane Smith,P002,D002,197587654321\n");
        fclose(fp);
    }

    // Setup parties file
    fp = fopen("../../data/party_name.txt", "w");
    if (fp)
    {
        fprintf(fp, "party_id,party_name\n");
        fprintf(fp, "P001,Democratic Party\n");
        fprintf(fp, "P002,Republican Party\n");
        fclose(fp);
    }

    // Setup districts file
    fp = fopen("../../data/district.txt", "w");
    if (fp)
    {
        fprintf(fp, "district_id,district_name\n");
        fprintf(fp, "D001,Central District\n");
        fprintf(fp, "D002,Eastern District\n");
        fclose(fp);
    }

    // Setup parliament candidates file
    fp = fopen("../../data/parliament_candidates.txt", "w");
    if (fp)
    {
        fprintf(fp, "candidate_number,party_id\n");
        fprintf(fp, "C001,P001\n");
        fprintf(fp, "C002,P002\n");
        fclose(fp);
    }

    // Setup voter count file
    fp = fopen("../../data/voter_count.txt", "w");
    if (fp)
    {
        fprintf(fp, "voting_number,candidate_number,party_id,district_id,count\n");
        fprintf(fp, "V001,C001,P001,D001,1\n");
        fprintf(fp, "V002,C002,P002,D002,1\n");
        fclose(fp);
    }

    printf(GREEN "Entity test data setup complete." RESET "\n");
}

void test_voter_functions()
{
    print_entity_test_header("Voter Functions");

    // Test create_voter
    int result = create_voter("V003", "Charlie Brown", "199012345678", "D001");
    assert_entity_test(result == 1, "create_voter should return 1 on success");

    // Test read_voter
    char *voter = read_voter("V001");
    assert_entity_test(voter != NULL, "read_voter should return non-NULL for existing voter");
    assert_entity_test(strstr(voter, "Alice Smith") != NULL, "read_voter should return correct voter data");
    if (voter)
        free(voter);

    // Test read non-existing voter
    voter = read_voter("V999");
    assert_entity_test(voter == NULL, "read_voter should return NULL for non-existing voter");

    // Test update_voter (name field - index 1)
    result = update_voter("V001", 1, "Alice Johnson");
    assert_entity_test(result == 1, "update_voter should return 1 on success");

    // Verify update
    voter = read_voter("V001");
    assert_entity_test(voter != NULL && strstr(voter, "Alice Johnson") != NULL, "update_voter should modify the correct field");
    if (voter)
        free(voter);

    // Test delete_voter
    result = delete_voter("V002");
    assert_entity_test(result == 1, "delete_voter should return 1 on success");

    // Verify deletion
    voter = read_voter("V002");
    assert_entity_test(voter == NULL, "delete_voter should remove the voter completely");
}

void test_candidate_functions()
{
    print_entity_test_header("Candidate Functions");

    // Test create_candidate
    int result = create_candidate("C003", "Mike Wilson", "P001", "D001", "199512345678");
    assert_entity_test(result == 1, "create_candidate should return 1 on success");

    // Test read_candidate
    char *candidate = read_candidate("C001");
    assert_entity_test(candidate != NULL, "read_candidate should return non-NULL for existing candidate");
    assert_entity_test(strstr(candidate, "John Doe") != NULL, "read_candidate should return correct candidate data");
    if (candidate)
        free(candidate);

    // Test update_candidate (name field - index 1)
    result = update_candidate("C001", 1, "John Smith");
    assert_entity_test(result == 1, "update_candidate should return 1 on success");

    // Verify update
    candidate = read_candidate("C001");
    assert_entity_test(candidate != NULL && strstr(candidate, "John Smith") != NULL, "update_candidate should modify the correct field");
    if (candidate)
        free(candidate);

    // Test delete_candidate
    result = delete_candidate("C002");
    assert_entity_test(result == 1, "delete_candidate should return 1 on success");

    // Verify deletion
    candidate = read_candidate("C002");
    assert_entity_test(candidate == NULL, "delete_candidate should remove the candidate completely");
}

void test_party_functions()
{
    print_entity_test_header("Party Functions");

    // Test create_party
    int result = create_party("P003", "Independent Party");
    assert_entity_test(result == 1, "create_party should return 1 on success");

    // Test read_party
    char *party = read_party("P001");
    assert_entity_test(party != NULL, "read_party should return non-NULL for existing party");
    assert_entity_test(strstr(party, "Democratic Party") != NULL, "read_party should return correct party data");
    if (party)
        free(party);

    // Test update_party
    result = update_party("P001", "New Democratic Party");
    assert_entity_test(result == 1, "update_party should return 1 on success");

    // Verify update
    party = read_party("P001");
    assert_entity_test(party != NULL && strstr(party, "New Democratic Party") != NULL, "update_party should modify the party name");
    if (party)
        free(party);

    // Test delete_party
    result = delete_party("P002");
    assert_entity_test(result == 1, "delete_party should return 1 on success");

    // Verify deletion
    party = read_party("P002");
    assert_entity_test(party == NULL, "delete_party should remove the party completely");
}

void test_district_functions()
{
    print_entity_test_header("District Functions");

    // Test create_district
    int result = create_district("D003", "Western District");
    assert_entity_test(result == 1, "create_district should return 1 on success");

    // Test read_district
    char *district = read_district("D001");
    assert_entity_test(district != NULL, "read_district should return non-NULL for existing district");
    assert_entity_test(strstr(district, "Central District") != NULL, "read_district should return correct district data");
    if (district)
        free(district);

    // Test update_district
    result = update_district("D001", "New Central District");
    assert_entity_test(result == 1, "update_district should return 1 on success");

    // Verify update
    district = read_district("D001");
    assert_entity_test(district != NULL && strstr(district, "New Central District") != NULL, "update_district should modify the district name");
    if (district)
        free(district);

    // Test delete_district
    result = delete_district("D002");
    assert_entity_test(result == 1, "delete_district should return 1 on success");

    // Verify deletion
    district = read_district("D002");
    assert_entity_test(district == NULL, "delete_district should remove the district completely");
}

void test_parliament_candidate_functions()
{
    print_entity_test_header("Parliament Candidate Functions");

    // Test create_parliament_candidate
    int result = create_parliament_candidate("C003", "P001");
    assert_entity_test(result == 1, "create_parliament_candidate should return 1 on success");

    // Test read_parliament_candidate
    char *parliament_candidate = read_parliament_candidate("C001", "P001");
    assert_entity_test(parliament_candidate != NULL, "read_parliament_candidate should return non-NULL for existing record");
    if (parliament_candidate)
        free(parliament_candidate);

    // Test delete_parliament_candidate
    result = delete_parliament_candidate("C002", "P002");
    assert_entity_test(result == 1, "delete_parliament_candidate should return 1 on success");

    // Verify deletion
    parliament_candidate = read_parliament_candidate("C002", "P002");
    assert_entity_test(parliament_candidate == NULL, "delete_parliament_candidate should remove the record completely");
}

void test_voter_count_functions()
{
    print_entity_test_header("Voter Count Functions");

    // Test create_voter_count
    int result = create_voter_count("V003", "C001", "P001", "D001", "1");
    assert_entity_test(result == 1, "create_voter_count should return 1 on success");

    // Test read_voter_count
    char *voter_count = read_voter_count("V001", "C001", "P001", "D001");
    assert_entity_test(voter_count != NULL, "read_voter_count should return non-NULL for existing record");
    if (voter_count)
        free(voter_count);

    // Test update_voter_count
    result = update_voter_count("V001", "C001", "P001", "D001", "5");
    assert_entity_test(result == 1, "update_voter_count should return 1 on success");

    // Verify update
    voter_count = read_voter_count("V001", "C001", "P001", "D001");
    assert_entity_test(voter_count != NULL && strstr(voter_count, "5") != NULL, "update_voter_count should modify the count");
    if (voter_count)
        free(voter_count);

    // Test delete_voter_count
    result = delete_voter_count("V002", "C002", "P002", "D002");
    assert_entity_test(result == 1, "delete_voter_count should return 1 on success");

    // Verify deletion
    voter_count = read_voter_count("V002", "C002", "P002", "D002");
    assert_entity_test(voter_count == NULL, "delete_voter_count should remove the record completely");
}

int main()
{
    printf(BLUE "=== Entity-Specific Function Testing Suite ===" RESET "\n");
    printf("This suite will test all entity-specific functions declared in data_handler.h\n");

    // Setup
    setup_entity_test_data();

    // Run all entity tests
    test_voter_functions();
    test_candidate_functions();
    test_party_functions();
    test_district_functions();
    test_parliament_candidate_functions();
    test_voter_count_functions();

    // Summary
    print_entity_summary();

    return (entity_tests_failed == 0) ? 0 : 1;
}