#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_utils.h"

void display_banner(void)
{
    printf(BOLD BLUE);
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    " WHITE "VoteMe Admin System" BLUE "                     ║\n");
    printf("║                  " WHITE "Enhanced Management Console" BLUE "               ║\n");
    printf("║══════════════════════════════════════════════════════════════║\n");
    printf("║  " GREEN "✓ Complete CRUD Operations" BLUE "   ║  " GREEN "✓ Data Viewing & Browsing" BLUE "  ║\n");
    printf("║  " GREEN "✓ System Limit Management" BLUE "    ║  " GREEN "✓ Real-time Validation" BLUE "    ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf(RESET "\n");
}

void pause_for_user(void)
{
    printf("\n" CYAN "Press Enter to continue..." RESET);
    getchar();
}

void clear_screen(void)
{
#ifdef _WIN32
    (void)system("cls");
#else
    (void)system("clear");
#endif
}

void get_user_input(const char *prompt, char *buffer, size_t size)
{
    printf(CYAN "%s: " RESET, prompt);
    fflush(stdout);

    if (fgets(buffer, size, stdin) != NULL)
    {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
        {
            buffer[len - 1] = '\0';
        }
    }
}

int get_user_choice(const char *prompt, int min, int max)
{
    int choice;
    char input[100];

    do
    {
        printf(CYAN "%s (%d-%d): " RESET, prompt, min, max);
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) != NULL)
        {
            choice = atoi(input);
            if (choice >= min && choice <= max)
            {
                return choice;
            }
        }

        display_error("Invalid input! Please try again.");
    } while (1);
}

void display_error(const char *message)
{
    printf(RED "❌ Error: %s\n" RESET, message);
}

void display_success(const char *message)
{
    printf(GREEN "✅ Success: %s\n" RESET, message);
}

void display_info(const char *message)
{
    printf(BLUE "ℹ️  Info: %s\n" RESET, message);
}

int count_records_in_file(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return -1;

    int count = 0;
    char line[256];

    if (fgets(line, sizeof(line), fp))
    {
        while (fgets(line, sizeof(line), fp))
        {
            count++;
        }
    }

    fclose(fp);
    return count;
}
