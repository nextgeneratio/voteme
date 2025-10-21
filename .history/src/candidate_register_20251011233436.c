#include <stdio.h>
#include <string.h>
#include "candidate_register.h"
#include "data_handler_enhanced.h"

int add_candidate(const char *candidate_number, const char *nic, const char *party_id, const char *name)
{
	// Use enhanced data handler for consistency
	return create_candidate(candidate_number, name, party_id, "D01", nic); // Default district
}

int view_candidates(void)
{
	FILE *file = fopen("data/approved_candidates.txt", "r");
	if (!file)
	{
		printf("❌ No candidates file found!\n");
		return -1;
	}

	printf("🏆 Registered Candidates:\n");
	printf("═══════════════════════════════════════════════════════════════════════════════\n");
	printf("%-12s %-20s %-10s %-10s %-15s\n", "Candidate ID", "Name", "Party", "District", "NIC");
	printf("───────────────────────────────────────────────────────────────────────────────\n");

	char line[256];
	int count = 0;

	// Skip header line
	if (fgets(line, sizeof(line), file))
	{
		while (fgets(line, sizeof(line), file))
		{
			char candidate_number[20], name[100], party_id[20], district[20], nic[20];
			if (sscanf(line, "%19[^,],%99[^,],%19[^,],%19[^,],%19s",
					   candidate_number, name, party_id, district, nic) == 5)
			{
				printf("%-12s %-20s %-10s %-10s %-15s\n",
					   candidate_number, name, party_id, district, nic);
				count++;
			}
		}
	}

	if (count == 0)
	{
		printf("No candidates registered yet.\n");
	}
	else
	{
		printf("───────────────────────────────────────────────────────────────────────────────\n");
		printf("Total candidates: %d\n", count);
	}

	fclose(file);
	return 0;
}
