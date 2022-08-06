#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ncurses.h>

#define __USE_XOPEN 1
#include <time.h>

#include "commands.h"
#include "constants.h"
#include "utils.h"


// cd command

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


// cal command

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


void run_cal_cmd(char *date_str)
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
    static const char* days[] = {
    	"Su", "M", "T", "W", "Th", "F", "Sa"
    };

    struct tm lt;
    struct tm *local_time = &lt;
    if (date_str == NULL) {
		time_t rawtime = time(NULL);
		if (rawtime == (time_t) -1) {
			perror("time");
			return;
		}
		local_time = localtime(&rawtime);
	} else {
		strptime(date_str, "%F", local_time);
	}
	int num_of_days = days_in_month[local_time->tm_mon];

	// determine if leap year, then if Feb, adjust day count
	if (local_time->tm_mon == 1 && is_leap_year(local_time->tm_year + 1900))
		num_of_days++;


	printf("\n%18s\n\n", months[local_time->tm_mon]);

	for (size_t i = 0; i < sizeof(days) / sizeof(char*); ++i) 
		printf("%4s", days[i]);
	printf("\n\n");

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


// movie command
#define GRAVITY 10
#define FPS 10
#define ONE_SECOND 1000000

typedef struct Ball {
	int r;
	double x, y, vx, vy;
} Ball;


Ball init_ball(int max_y, int max_x) 
{
	(void) max_y;
	Ball ball = {
		.x  = rand() % max_x,
		.vx = rand() % 20,
		.vy = rand() % 20,
		.r  = (rand() % 4) + 1
	};
	ball.y = ball.r;
	return ball;
}


void draw_ball(Ball ball, int max_y)
{	
	for (int i = -ball.r; i <= ball.r; ++i) {
		for (int j = -ball.r; j <= ball.r; ++j) {
			if (i*i + j*j <= ball.r * ball.r + ball.r * 0.8f)
				mvaddch(max_y - (ball.y + i), ball.x + j, '#');
		}
	}
}


void update_ball(Ball *ball, int max_x)
{
	ball->x += ball->vx / FPS;
	ball->y += ball->vy / FPS;

	if (ball->y - ball->r <= 0)
		ball->vy = -ball->vy;
	else
		ball->vy -= GRAVITY / FPS;

	if (ball->x + ball->r >= max_x || ball->x - ball->r <= 0)
		ball->vx = -ball->vx;
	
}


void run_movie_cmd(char *token)
{
	int rows, cols, n_balls;

	// seed RNG
	time_t t;
	srand((unsigned) time(&t));

	if (token == NULL)
		n_balls = (rand() % 8) + 2; // 2-10 balls
	else 
		n_balls = atoi(token);

	Ball balls[n_balls];

	WINDOW *stdscr = initscr();
	timeout(0);
	curs_set(0);
	getmaxyx(stdscr, rows, cols);

	int max_y = rows - 2;

	for (int i = 0; i < n_balls; ++i) 
		balls[i] = init_ball(rows, cols);
	

	while (true) {
		erase();
		box(stdscr, 0, 0);

		for (int i = 0; i < n_balls; ++i) {
			draw_ball(balls[i], max_y);
			update_ball(&balls[i], cols);
		}

		refresh();

		if (getch() == ERR)
			usleep(ONE_SECOND / FPS);
		else
			break;
	}


	getch();



	endwin();	
}