#include <stdio.h>
#include "voter_register.c"
#include "candidate_register.c"

int main() {
	int choice;
	while (1) {
		printf("\n--- Voting System Main Menu ---\n");
		printf("1. Add Voter\n");
		printf("2. View Voters\n");
		printf("3. Add Candidate\n");
		printf("4. View Candidates\n");
		printf("0. Exit\n");
		printf("Enter choice: ");
		scanf("%d", &choice);
		if (choice == 0) break;
		if (choice == 1) {
			char voting_number[10], nic[20], name[50];
			printf("Enter voting number: "); scanf("%s", voting_number);
			printf("Enter NIC: "); scanf("%s", nic);
			printf("Enter name: "); scanf("%s", name);
			add_voter(voting_number, nic, name);
		} else if (choice == 2) {
			view_voters();
		} else if (choice == 3) {
			char candidate_number[10], nic[20], party_id[10], name[50];
			printf("Enter candidate number: "); scanf("%s", candidate_number);
			printf("Enter NIC: "); scanf("%s", nic);
			printf("Enter party ID: "); scanf("%s", party_id);
			printf("Enter name: "); scanf("%s", name);
			add_candidate(candidate_number, nic, party_id, name);
		} else if (choice == 4) {
			view_candidates();
		} else {
			printf("Invalid choice.\n");
		}
	}
	return 0;
}
