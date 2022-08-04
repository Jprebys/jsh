#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "commands.h"
#include "constants.h"
#include "utils.h"


void run_cd_cmd(char **tokens, jsh_settings *stgs)
{
	if (tokens[1] == NULL) {
		fprintf(stderr, "Run cd with: 'cd <directory>'\n");
		return;
	}
	int result = chdir(tokens[1]);
	if (result == -1) {
		fprintf(stderr, "Error: %s\n", strerror(errno));
		return;
	}
	free(stgs->cwd);
	char *cwd = malloc(MAX_DIR_NAME);
	getcwd(cwd, MAX_DIR_NAME);
	stgs->cwd = cwd;
	return;
}


int day_of_week_number(int d, int m, int y)
{
	// returns 0 for Sunday, 6 for Saturday
	// formula for determining day of week taken from 
	// https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week#Implementation-dependent_methods
	return (d += m < 3 ? y-- : y - 2, 23*m/9 + d + 4 + y/4- y/100 + y/400)%7;
}

bool is_leap_year(int year)
{
	if (year % 4 == 0) {
		if (year % 100 == 0) {
			if (year % 400 == 0)
				return true;
		} else
			return true;
	}
	return false;
}


void run_cal_cmd()
{
	static const char *months[] = {
	    "January", "February", "March", "April",
        "May", "June", "July", "August",
        "September", "October", "November", "December"
    };
    static const int days_in_month[] = { 
    	31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31 
    };

	time_t rawtime = time(NULL);
	if (rawtime == (time_t) -1) {
		perror("time");
		return;
	}
	struct tm *local_time = localtime(&rawtime);

	int num_of_days = days_in_month[local_time->tm_mon];

	// determine if leap year, then if Feb, adjust day count
	if (local_time->tm_mon == 1 && is_leap_year(local_time->tm_year + 1900))
		num_of_days--;


	printf("\n%18s\n\n", months[local_time->tm_mon]);

	int month_start_day = day_of_week_number(1, local_time->tm_mon + 1, 
		                                     local_time->tm_year + 1900);

	int i = 0;
	for ( ; i < month_start_day; ++i) {
		printf("%4s", "");
	}

	for (int j = 1; j <= num_of_days; ++j) {
		if (j == local_time->tm_mday) {
			// Read about ANSI color codes in this SO answer
			// https://stackoverflow.com/a/2616912/3268228
			// This will print the number in black with a white background
			// preceeded by two spaces
			printf("  \033[30;47m%2d\033[0m", j);
		} else {
			printf("%4d", j);
		}
		if ((i = (i + 1) % 7) == 0)
			printf("\n\n");
	}
	printf("\n\n");
}