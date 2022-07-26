#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 256
#define MAX_TOKENS 128
#define EXIT_STR "exit\n"
#define JSH_DELIMITER " \n"

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

char **split_line(char *line)
{
	char *token;
	char **result = malloc(MAX_TOKENS * sizeof(char*));
	if (result == NULL) {
		perror("Split line malloc");
		exit(1);
	}

	size_t count = 0;
	token = strtok(line, JSH_DELIMITER);
	while(token != NULL) {
		result[count++] = token;
		if (count == MAX_TOKENS) {
			fprintf(stderr, "ERROR: Too many tokens\n");
			exit(1);
		}
		token = strtok(NULL, JSH_DELIMITER);
	}
	result[count] = NULL;

	return result;
}


void jsh_main_loop_run()
{
	char *line, **tokens;
	do {
		line = get_stdin_line();
		tokens = split_line(line);

		printf("Tokens: ");
		for (size_t i = 0; tokens[i] != NULL; ++i) {
			printf("[%s]", tokens[i]);
		}
		printf("\n");
		
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