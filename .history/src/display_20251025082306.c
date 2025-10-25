#include "display.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

void clearscreen(void){
	printf("\033[H\033[2J\033[3J");
}

void typewrite(const char *s, useconds_t delay){
	for(int i = 0; s[i] != '\0'; i++){
		putchar(s[i]);
		fflush(stdout);
		usleep(delay);
	}
}

void clearinputbuff(void){
	int c;
	while((c = getchar())!='\n' && c!=EOF)
		;
}

void showbanner(void){
	typewrite(
	"\n" 
	"---------------------------------------------------------------\n\n"
	" /$$    /$$            /$$               /$$      /$$\n"
	"| $$   | $$           | $$              | $$$    /$$$\n"
	"| $$   | $$ /$$$$$$  /$$$$$$    /$$$$$$ | $$$$  /$$$$  /$$$$$$\n"
	"|  $$ / $$//$$__  $$|_  $$_/   /$$__  $$| $$ $$/$$ $$ /$$__  $$\n"
	" \\  $$ $$/| $$  \\ $$  | $$    | $$$$$$$$| $$  $$$| $$| $$$$$$$$\n"
	"  \\  $$$/ | $$  | $$  | $$ /$$| $$_____/| $$\\  $ | $$| $$_____/\n"
	"   \\  $/  |  $$$$$$/  |  $$$$/|  $$$$$$$| $$ \\/  | $$|  $$$$$$$\n"
	"    \\_/    \\______/    \\___/   \\_______/|__/     |__/ \\_______/\n\n"

	"---------------------------------------------------------------\n"
	"\n", 500);
}

void showabout(void){
	clearscreen();
	typewrite(
		"\n" CYAN_ON_BLACK "Developers: \n"
		"\tTG/2024/2061 : M.H.M.Amjad\n"
		"\tTG/2024/2098 : K.C.Nimash\n"
		"\tTG/2024/2139 : W.K.C.Kalhara\n"
		"\tTG/2021/2140 : A.J.C.Wickramasooriya\n"
		RESET_COLORS, 1000);
	printf("\n\n" RED_ON_BLACK "...PRESS ENTER TO GET BACK TO THE MAIN MENU...\n" RESET_COLORS);
	fflush(stdout);
	clearinputbuff();
	getchar();
}

static bool file_exists_and_executable(const char *path) {
	struct stat st;
	if (stat(path, &st) != 0) return false;
	if (!S_ISREG(st.st_mode)) return false;
	return access(path, X_OK) == 0;
}

typedef struct {
	char id[16];
	char name[256];
} NameMap;

static size_t load_party_map(const char *filename, NameMap out[], size_t max_items) {
	FILE *fp = fopen(filename, "r");
	if (!fp) return 0;
	char line[512];
	size_t n = 0;
	// skip header
	if (!fgets(line, sizeof(line), fp)) { fclose(fp); return 0; }
	while (fgets(line, sizeof(line), fp) && n < max_items) {
		// Trim newline
		line[strcspn(line, "\r\n")] = '\0';
		char *id = strtok(line, ",");
		char *name = strtok(NULL, ""); // rest of line
		if (!id || !name) continue;
		strncpy(out[n].id, id, sizeof(out[n].id) - 1);
		out[n].id[sizeof(out[n].id) - 1] = '\0';
		strncpy(out[n].name, name, sizeof(out[n].name) - 1);
		out[n].name[sizeof(out[n].name) - 1] = '\0';
		n++;
	}
	fclose(fp);
	return n;
}

static size_t load_candidate_name_map(const char *filename, NameMap out[], size_t max_items) {
	FILE *fp = fopen(filename, "r");
	if (!fp) return 0;
	char line[512];
	size_t n = 0;
	// skip header
	if (!fgets(line, sizeof(line), fp)) { fclose(fp); return 0; }
	while (fgets(line, sizeof(line), fp) && n < max_items) {
		// columns: candidate_number,name,party_id,district_id,nic
		line[strcspn(line, "\r\n")] = '\0';
		char *cand_id = strtok(line, ",");
		char *name = strtok(NULL, ",");
		if (!cand_id || !name) continue;
		strncpy(out[n].id, cand_id, sizeof(out[n].id) - 1);
		out[n].id[sizeof(out[n].id) - 1] = '\0';
		strncpy(out[n].name, name, sizeof(out[n].name) - 1);
		out[n].name[sizeof(out[n].name) - 1] = '\0';
		n++;
	}
	fclose(fp);
	return n;
}

static const char* map_lookup(const NameMap map[], size_t n, const char *id) {
	for (size_t i = 0; i < n; i++) {
		if (strcmp(map[i].id, id) == 0) return map[i].name;
	}
	return NULL;
}

static void show_voting_results(void) {
	clearscreen();
	typewrite("\n" CYAN_ON_BLACK "Loading voting results..." RESET_COLORS "\n\n", 500);

	// Load lookup maps
	NameMap parties[256];
	NameMap candidates[1024];
	size_t party_count = load_party_map("data/party_name.txt", parties, 256);
	size_t cand_count = load_candidate_name_map("data/approved_candidates.txt", candidates, 1024);

	if (party_count == 0 || cand_count == 0) {
		printf(RED_ON_BLACK "Error: Missing or empty party/candidate data files." RESET_COLORS "\n");
		printf("\n" RED_ON_BLACK "...PRESS ENTER TO RETURN..." RESET_COLORS "\n");
		clearinputbuff(); getchar();
		return;
	}

	FILE *fp = fopen("data/parliament_candidates.txt", "r");
	if (!fp) {
		printf(RED_ON_BLACK "Error: Could not open data/parliament_candidates.txt" RESET_COLORS "\n");
		printf("\n" RED_ON_BLACK "...PRESS ENTER TO RETURN..." RESET_COLORS "\n");
		clearinputbuff(); getchar();
		return;
	}

	char line[256];
	// header
	if (!fgets(line, sizeof(line), fp)) { fclose(fp); }

	printf(GREEN_ON_BLACK "%-8s  %-24s  %-6s  %-30s" RESET_COLORS "\n", "ID", "Candidate", "Party", "Party Name");
	printf("--------------------------------------------------------------------------------\n");

	size_t shown = 0;
	while (fgets(line, sizeof(line), fp)) {
		line[strcspn(line, "\r\n")] = '\0';
		char *cand_id = strtok(line, ",");
		char *party_id = strtok(NULL, ",");
		if (!cand_id || !party_id) continue;
		const char *cand_name = map_lookup(candidates, cand_count, cand_id);
		const char *party_name = map_lookup(parties, party_count, party_id);
		printf("%-8s  %-24s  %-6s  %-30s\n",
			   cand_id,
			   cand_name ? cand_name : "<unknown>",
			   party_id,
			   party_name ? party_name : "<unknown>");
		shown++;
		if (shown % 30 == 0) {
			printf("\n" YELLOW "-- More -- Press ENTER to continue --" RESET_COLORS "\n");
			clearinputbuff(); getchar();
		}
	}
	fclose(fp);

	printf("\n" RED_ON_BLACK "...PRESS ENTER TO GET BACK TO THE MAIN MENU..." RESET_COLORS "\n");
	clearinputbuff(); getchar();
}

void showmainmenu(void){
	clearscreen();
	printf(RED_ON_BLACK);
	showbanner();
	typewrite(
		"\n\t" CYAN_ON_BLACK "1" RESET_COLORS ") " CYAN_ON_BLACK "Admin" RESET_COLORS "\n"
		"\t" CYAN_ON_BLACK "2" RESET_COLORS ") " CYAN_ON_BLACK "Candidate" RESET_COLORS "\n"
		"\t" CYAN_ON_BLACK "3" RESET_COLORS ") " CYAN_ON_BLACK "Voter" RESET_COLORS "\n"
		"\t" CYAN_ON_BLACK "4" RESET_COLORS ") " CYAN_ON_BLACK "About" RESET_COLORS "\n"
		"\t" CYAN_ON_BLACK "5" RESET_COLORS ") " CYAN_ON_BLACK "View Voting Results" RESET_COLORS "\n"
		"\t" CYAN_ON_BLACK "0" RESET_COLORS ") " CYAN_ON_BLACK "Exit" RESET_COLORS "\n"
        , 800);
	char option;
	for(;;){
		printf("\n" RESET_COLORS "[" GREEN_ON_BLACK "*" RESET_COLORS "]" GREEN_ON_BLACK);
		typewrite(" Option : ", 1000);
		scanf(" %c", &option);
		switch(option){
			case '1':
				if (file_exists_and_executable("bin/admin")) {
					system("./bin/admin");
				} else {
					printf(RED_ON_BLACK "Admin executable not found. Build it with 'make admin'." RESET_COLORS "\n");
					printf(RED_ON_BLACK "...PRESS ENTER TO RETURN..." RESET_COLORS "\n");
					clearinputbuff(); getchar();
				}
				clearscreen();
				showbanner();
				break;
			case '2':
				if (file_exists_and_executable("bin/candidate_register")) {
					system("./bin/candidate_register");
				} else {
					printf(RED_ON_BLACK "Candidate tool not found (bin/candidate_register)." RESET_COLORS "\n");
					printf(YELLOW "Tip: build it or use Admin to manage candidates." RESET_COLORS "\n");
					printf(RED_ON_BLACK "...PRESS ENTER TO RETURN..." RESET_COLORS "\n");
					clearinputbuff(); getchar();
				}
				clearscreen();
				showbanner();
				break;
			case '3':
				if (file_exists_and_executable("bin/vote")) {
					system("./bin/vote");
				} else if (file_exists_and_executable("bin/voter_register")) {
					system("./bin/voter_register");
				} else {
					printf(RED_ON_BLACK "Voter tools not found (bin/vote or bin/voter_register)." RESET_COLORS "\n");
					printf(YELLOW "Tip: run voting UI build or use Admin to manage voters." RESET_COLORS "\n");
					printf(RED_ON_BLACK "...PRESS ENTER TO RETURN..." RESET_COLORS "\n");
					clearinputbuff(); getchar();
				}
				clearscreen();
				showbanner();
				break;
			case '4':
				showabout();
				break;
			case '5':
				show_voting_results();
				clearscreen();
				showbanner();
				break;
			case '0':
				printf("\n" CYAN_ON_BLACK "Have a nice day (^_^)" RESET_COLORS "\n");
				return;
				break;
			default:
				printf("\n" RESET_COLORS "[" RED_ON_BLACK "-" RESET_COLORS "] " RED_ON_BLACK "Unknown Option!" RESET_COLORS "\n");
				continue;
		}
		// continue menu loop after each action
	}
	return;
}

