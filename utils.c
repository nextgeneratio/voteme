#include "utils.h"

void clearscreen(void){
	printf("\033[H\033[2J\033[3J");
}

void typewrite(const char s[], useconds_t delay){
	for(int i = 0; s[i] != '\0'; i++){
		putchar(s[i]);
		fflush(stdout);
		usleep(delay);
	}
}

