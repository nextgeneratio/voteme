#include "utils.h"

void showbanner(void){
	FILE *fp = fopen("banner.txt", "r");
	char buff[256];
	while(fgets(buff, sizeof(buff), fp)){
		typewrite(buff, 500);
	}
	putchar('\n');
	fclose(fp);
}

void showabout(void){
	char abouttext[1024];
	clearscreen();
	sprintf(abouttext,
		"\n%sAuthors: \n"
		"\tTG/2024/2098 : K.C.Nimash\n"
		/* Other group members details */
		"\nThis is a voting system\n"
		/* Other details */
		"%s"
		, CYAN_ON_BLACK, RESET_COLORS	
	);
	typewrite(abouttext, 1000);
	printf("\n\n%s...PRESS ENTER TO GET BACK TO THE MAIN MENU...\n%s", RED_ON_BLACK, RESET_COLORS);
	fflush(stdin);
	getchar();
}

void showmainmenu(void){
	clearscreen();
	printf(RED_ON_BLACK);
	showbanner();
	char menu[1024];
	sprintf(menu, "\n\t%s1%s) %sLogin as a voter%s\n"
	       "\t%s2%s) %sLogin as a candidate%s\n"
	       "\t%s3%s) %sRegister a voter%s\n"
	       "\t%s4%s) %sRegister a party%s\n"
	       "\t%s5%s) %sRegister a candidate%s\n"
	       "\t%s6%s) %sView votes%s\n"
	       "\t%s7%s) %sAbout%s\n"
	       "\t%s0%s) %sExit%s\n"
	       , CYAN_ON_BLACK, RESET_COLORS, CYAN_ON_BLACK, RESET_COLORS,
	       CYAN_ON_BLACK, RESET_COLORS, CYAN_ON_BLACK, RESET_COLORS,
	       CYAN_ON_BLACK, RESET_COLORS, CYAN_ON_BLACK, RESET_COLORS,
	       CYAN_ON_BLACK, RESET_COLORS, CYAN_ON_BLACK, RESET_COLORS,
	       CYAN_ON_BLACK, RESET_COLORS, CYAN_ON_BLACK, RESET_COLORS,
	       CYAN_ON_BLACK, RESET_COLORS, CYAN_ON_BLACK, RESET_COLORS,
	       CYAN_ON_BLACK, RESET_COLORS, CYAN_ON_BLACK, RESET_COLORS,
	       CYAN_ON_BLACK, RESET_COLORS, CYAN_ON_BLACK, RESET_COLORS

        ); 
	typewrite(menu, 100);
	char option;
	for(;;){
		printf("\n%s[%s*%s] %s", RESET_COLORS, GREEN_ON_BLACK, RESET_COLORS, GREEN_ON_BLACK);
		typewrite("Option : ", 100000);
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
				/* Not implemented yet */
				break;
			case '5':
				/* Not implemented yet */
				break;
			case '6':
				/* Not implemented yet */
				break;
			case '7':
				showabout();
				showmainmenu();
				break;
			case '0':
				printf("\n%sHave a nice day (^_^)%s\n", CYAN_ON_BLACK, RESET_COLORS);
				return;
				break;
			default:
				printf("\n%s[%s-%s] %sUnknown Option!%s\n", RESET_COLORS, RED_ON_BLACK, RESET_COLORS, RED_ON_BLACK, RESET_COLORS);
				continue;
		}
		return;
	}
	return;
}

int main(void){
	showmainmenu();
	return 0;
}
