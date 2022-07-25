#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 128
#define EXIT_STR "exit\n"

char *get_stdin_line()
{
	char *buffer = NULL;
	size_t n = 0;

	if (getline(&buffer, &n, stdin) == -1) {
		if (feof(stdin))
			return buffer;
		else {
			perror("Error using getline");
			exit(1);
		}
	}
	return buffer;

}


void jsh_main_loop_run()
{
	char *line;
	do {
		line = get_stdin_line();
		printf("%s", line);
	} while (strncmp(line, EXIT_STR, strlen(EXIT_STR)));
	printf("Exiting...\n");
}

int main()
{
	// TODO: parse command line args
	// TODO: read config file here

	// run main shell loop
	jsh_main_loop_run();

	// TODO: add any cleanup here

	return 0;
}