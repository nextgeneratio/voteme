// Minimal Candidate Registration CLI (placeholder)
#include <stdio.h>

static void press_enter_to_exit(void) {
	int c;
	printf("\nPress ENTER to return to main menu...\n");
	while ((c = getchar()) != '\n' && c != EOF) {}
}

int main(void) {
	printf("\n===================================\n");
	printf("  Candidate Registration (CLI)    \n");
	printf("===================================\n\n");
	printf("This placeholder confirms routing works.\n");
	printf("Use the Admin tool to manage candidates in this build.\n");
	press_enter_to_exit();
	return 0;
}

