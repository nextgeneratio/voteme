#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <stddef.h>

// Color codes for better user interface
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define WHITE "\033[0;37m"
#define RESET "\033[0m"
#define BOLD "\033[1m"

// Basic UI helpers
void display_banner(void);
void pause_for_user(void);
void clear_screen(void);

// Input helpers
void get_user_input(const char *prompt, char *buffer, size_t size);
int get_user_choice(const char *prompt, int min, int max);

// Messaging helpers
void display_error(const char *message);
void display_success(const char *message);
void display_info(const char *message);

// File helpers
int count_records_in_file(const char *filename);

#endif // UI_UTILS_H
