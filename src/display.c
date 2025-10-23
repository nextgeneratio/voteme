#include "display.h"

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

void clearinputbuff(void){
	int c;
	while((c = getchar())!='\n' && c!=EOF)
		;
}

void showbanner(void){
	char buff[1000] = 
	"\n" 
	" /$$    /$$            /$$               /$$      /$$\n"
	"| $$   | $$           | $$              | $$$    /$$$\n"
	"| $$   | $$ /$$$$$$  /$$$$$$    /$$$$$$ | $$$$  /$$$$  /$$$$$$\n"
	"|  $$ / $$//$$__  $$|_  $$_/   /$$__  $$| $$ $$/$$ $$ /$$__  $$\n"
	" \\  $$ $$/| $$  \\ $$  | $$    | $$$$$$$$| $$  $$$| $$| $$$$$$$$\n"
	"  \\  $$$/ | $$  | $$  | $$ /$$| $$_____/| $$\\  $ | $$| $$_____/\n"
	"   \\  $/  |  $$$$$$/  |  $$$$/|  $$$$$$$| $$ \\/  | $$|  $$$$$$$\n"
	"    \\_/    \\______/    \\___/   \\_______/|__/     |__/ \\_______/\n"
	"\n";

	typewrite(buff, 500);
}

void showabout(void){
	char abouttext[1024];
	clearscreen();
	sprintf(abouttext,
		"\n" CYAN_ON_BLACK "Developers: \n"
		"\tTG/2024/2061 : M.H.M.Amjad\n"
		"\tTG/2024/2098 : K.C.Nimash\n"
		"\tTG/2024/2139 : W.K.C.Kalhara\n"
		"\tTG/2021/2140 : A.J.C.Wickramasooriya\n"
		RESET_COLORS
	);
	typewrite(abouttext, 1000);
	printf("\n\n" RED_ON_BLACK "...PRESS ENTER TO GET BACK TO THE MAIN MENU...\n" RESET_COLORS);
	fflush(stdout);
	clearinputbuff();
	getchar();
}

void showmainmenu(void){
	clearscreen();
	printf(RED_ON_BLACK);
	showbanner();
	char menu[1024];
	sprintf(menu,
		"\n\t" CYAN_ON_BLACK "1" RESET_COLORS ") " CYAN_ON_BLACK "Admin" RESET_COLORS "\n"
		"\t" CYAN_ON_BLACK "2" RESET_COLORS ") " CYAN_ON_BLACK "Candidate" RESET_COLORS "\n"
		"\t" CYAN_ON_BLACK "3" RESET_COLORS ") " CYAN_ON_BLACK "Voter" RESET_COLORS "\n"
		"\t" CYAN_ON_BLACK "4" RESET_COLORS ") " CYAN_ON_BLACK "About" RESET_COLORS "\n"
		"\t" CYAN_ON_BLACK "0" RESET_COLORS ") " CYAN_ON_BLACK "Exit" RESET_COLORS "\n"
        ); 
	typewrite(menu, 800);
	char option;
	for(;;){
		printf("\n" RESET_COLORS "[" GREEN_ON_BLACK "*" RESET_COLORS "]" GREEN_ON_BLACK);
		typewrite(" Option : ", 1000);
		scanf(" %c", &option);
		switch(option){
			case '1':
				/* Not implemented yet */
				break;
			case '2':
				/* Not implemented yet */
				break;
			case '3':
				/* Not implemented yet */
				break;
			case '4':
				showabout();
				showmainmenu();
				break;
			case '0':
				printf("\n" CYAN_ON_BLACK "Have a nice day (^_^)" RESET_COLORS "\n");
				return;
				break;
			default:
				printf("\n" RESET_COLORS "[" RED_ON_BLACK "-" RESET_COLORS "] " RED_ON_BLACK "Unknown Option!" RESET_COLORS "\n");
				continue;
		}
		return;
	}
	return;
}

