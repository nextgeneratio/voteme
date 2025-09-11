#include "data_handler.h"
#include <stdio.h>
#include <string.h>

#define CANDIDATES_FILE "../data/approved_candidates.txt"

void add_candidate(const char *candidate_number, const char *nic, const char *party_id, const char *name) {
	char line[MAX_LINE_LENGTH];
	snprintf(line, sizeof(line), "%s,%s,%s,%s", candidate_number, nic, party_id, name);
	append_line(CANDIDATES_FILE, line);
}

void view_candidates() {
	FILE *fp = fopen(CANDIDATES_FILE, "r");
	if (!fp) { printf("Cannot open candidates file.\n"); return; }
	char line[MAX_LINE_LENGTH];
	while (fgets(line, sizeof(line), fp)) {
		printf("%s", line);
	}
	fclose(fp);
}
