// Voter Registration CLI
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #include <stdbool.h>

    #define VOTERS_FILE "data/approved_voters.txt"
    #define DISTRICT_FILE "data/district.txt"

    static void trim_newline(char *s) {
        if (!s) return;
        size_t n = strlen(s);
        while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r')) { s[--n] = '\0'; }
    }

    static void press_enter_to_exit(void) {
        int c;
        printf("\nPress ENTER to return to main menu...\n");
        while ((c = getchar()) != '\n' && c != EOF) {}
    }

    static bool ensure_voters_header(FILE *fp) {
        // fp is opened as r+ or a+
        long pos = ftell(fp);
        fseek(fp, 0, SEEK_END);
        long end = ftell(fp);
        if (end == 0) {
            fseek(fp, 0, SEEK_SET);
            fprintf(fp, "voting_number,name,nic,district_id\n");
            fflush(fp);
            return true;
        }
        // check first line
        fseek(fp, 0, SEEK_SET);
        char line[256];
        if (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "voting_number,", 14) != 0) {
                // prepend header? simplest: if header missing, we will not modify; appends still fine
            }
        }
        fseek(fp, pos, SEEK_SET);
        return true;
    }

    static int count_voter_rows(void) {
        FILE *fp = fopen(VOTERS_FILE, "r");
        if (!fp) return 0;
        char line[512];
        int count = 0;
        bool first = true;
        while (fgets(line, sizeof(line), fp)) {
            if (first && strncmp(line, "voting_number,", 14) == 0) { first = false; continue; }
            first = false;
            // skip empty
            char *p = line; while (*p && isspace((unsigned char)*p)) p++;
            if (*p == '\0') continue;
            count++;
        }
        fclose(fp);
        return count;
    }

    static bool nic_exists(const char *nic) {
        FILE *fp = fopen(VOTERS_FILE, "r");
        if (!fp) return false;
        char line[512];
        bool first = true;
        while (fgets(line, sizeof(line), fp)) {
            if (first && strncmp(line, "voting_number,", 14) == 0) { first = false; continue; }
            first = false;
            trim_newline(line);
            char *tok = strtok(line, ","); // voting_number
            tok = strtok(NULL, ",");       // name
            tok = strtok(NULL, ",");       // nic
            if (tok && strcmp(tok, nic) == 0) { fclose(fp); return true; }
        }
        fclose(fp);
        return false;
    }

    static void generate_next_voting_number(char out[8]) {
        int maxn = 0;
        FILE *fp = fopen(VOTERS_FILE, "r");
        if (fp) {
            char line[512];
            bool first = true;
            while (fgets(line, sizeof(line), fp)) {
                if (first && strncmp(line, "voting_number,", 14) == 0) { first = false; continue; }
                first = false;
                trim_newline(line);
                char *id = strtok(line, ",");
                if (id && id[0] == 'V') {
                    int n = atoi(id + 1);
                    if (n > maxn) maxn = n;
                }
            }
            fclose(fp);
        }
        maxn += 1;
        snprintf(out, 8, "V%04d", maxn);
    }

    static void list_districts(void) {
        FILE *fp = fopen(DISTRICT_FILE, "r");
        if (!fp) { printf("\n[!] Could not open %s\n", DISTRICT_FILE); return; }
        char line[512];
        if (!fgets(line, sizeof(line), fp)) { fclose(fp); return; } // header
        printf("\nAvailable Districts:\n");
        while (fgets(line, sizeof(line), fp)) {
            trim_newline(line);
            char *id = strtok(line, ",");
            char *name = strtok(NULL, "");
            if (!id || !name) continue;
            printf("  %s - %s\n", id, name);
        }
        fclose(fp);
    }

    static bool valid_district_id(const char *did) {
        FILE *fp = fopen(DISTRICT_FILE, "r");
        if (!fp) return false;
        char line[512];
        if (!fgets(line, sizeof(line), fp)) { fclose(fp); return false; }
        bool ok = false;
        while (fgets(line, sizeof(line), fp)) {
            trim_newline(line);
            char *id = strtok(line, ",");
            if (id && strcmp(id, did) == 0) { ok = true; break; }
        }
        fclose(fp);
        return ok;
    }

    int main(void) {
        printf("\n===============================\n");
        printf("  Voter Registration (CLI)     \n");
        printf("===============================\n\n");

        // Collect inputs
        char name[128];
        char nic[64];
        char did[16];

        printf("Full name: ");
        if (!fgets(name, sizeof(name), stdin)) return 1;
        trim_newline(name);
        if (name[0] == '\0') { printf("Name cannot be empty.\n"); press_enter_to_exit(); return 1; }

        printf("NIC: ");
        if (!fgets(nic, sizeof(nic), stdin)) return 1;
        trim_newline(nic);
        if (nic[0] == '\0') { printf("NIC cannot be empty.\n"); press_enter_to_exit(); return 1; }
        if (nic_exists(nic)) { printf("\n[!] NIC already registered.\n"); press_enter_to_exit(); return 1; }

        list_districts();
        printf("\nEnter district id: ");
        if (!fgets(did, sizeof(did), stdin)) return 1;
        trim_newline(did);
        if (!valid_district_id(did)) { printf("\n[!] Invalid district id.\n"); press_enter_to_exit(); return 1; }

        // Open file and ensure header
        FILE *fp = fopen(VOTERS_FILE, "a+");
        if (!fp) { perror("open voters file"); press_enter_to_exit(); return 1; }
        ensure_voters_header(fp);

        // Generate ID and append
        char id[8];
        generate_next_voting_number(id);
        fprintf(fp, "%s,%s,%s,%s\n", id, name, nic, did);
        fclose(fp);

        printf("\n[✓] Voter registered successfully with ID %s\n", id);
        press_enter_to_exit();
        return 0;
    }
