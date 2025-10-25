
#include <stdio.h>
#include <unistd.h>
#define RESET_COLORS "\033[00m"
#define RED_ON_BLACK "\033[1;31m"
#define GREEN_ON_BLACK "\033[1;32m"
#define CYAN_ON_BLACK "\033[1;36m"
void typewrite(const char[], unsigned int);
void clearscreen(void);
void showmainmenu(void);
