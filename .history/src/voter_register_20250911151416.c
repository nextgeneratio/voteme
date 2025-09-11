#include "data_handler.c"
#include <stdio.h>
#include <string.h>

#define VOTERS_FILE "../data/approved_voters.txt"

void add_voter(const char *voting_number, const char *nic, const char *name) {
	char line[MAX_LINE_LENGTH];
	snprintf(line, sizeof(line), "%s,%s,%s", voting_number, nic, name);
	append_line(VOTERS_FILE, line);
}

void view_voters() {
	FILE *fp = fopen(VOTERS_FILE, "r");
	if (!fp) { printf("Cannot open voters file.\n"); return; }
	char line[MAX_LINE_LENGTH];
	while (fgets(line, sizeof(line), fp)) {
		printf("%s", line);
	}
	fclose(fp);
}
