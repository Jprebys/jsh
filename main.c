#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <sys/wait.h>

#define MAX_LINE_LENGTH 256
#define MAX_TOKENS 128
#define EXIT_STR "exit"
#define JSH_DELIMITER " "
#define JSH_PROMPT ">>"
#define BIN_DIR "/usr/bin"
#define JOIN_CHAR "/"
#define JOIN_CHAR_LEN (sizeof(JOIN_CHAR) - 1)


char* join_paths(char* base, char* fname) 
{
	size_t base_size = strlen(base);
	size_t fname_size = strlen(fname);
	char* begin = malloc(base_size + JOIN_CHAR_LEN + fname_size + 1);
	char* end = begin;
	memcpy(end, base, base_size);
	end += base_size;
	memcpy(end, JOIN_CHAR, JOIN_CHAR_LEN);
	end += JOIN_CHAR_LEN;
	memcpy(end, fname, fname_size);
	end += fname_size;
	*end = '\0';

	return begin;
}


char *get_stdin_line()
{
	char *prompt = JSH_PROMPT;
	return readline(prompt);
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

void start_process(char **tokens) {
	pid_t pid = fork();
	switch (pid) {
		case -1:    // error
			perror("Fork");
			exit(1);
		case  0:    // child
			char *f_path = join_paths(BIN_DIR, tokens[0]);
			int result = execve(f_path, tokens, NULL);
			free(f_path);
			if (result == -1) {
				perror(tokens[0]);
				exit(1);
			} else
				exit(0);
		default:    // parent
			int status;
			waitpid(pid, &status, 0);
			if (WIFEXITED(status) && (WEXITSTATUS(status))) {
				fprintf(stderr, "%c\n", WEXITSTATUS(status));
			}
	}
}


void jsh_main_loop_run()
{
	char *line, **tokens;
	bool exit_received = false;
	do {
		line = get_stdin_line();
		if (line == NULL) {
			free(line);
			break;
		}
		printf("here%s\n", line);		
		tokens = split_line(line);
		printf("Tokens: ");
		for (size_t i = 0; tokens[i] != NULL; ++i) {
			printf("[%s]", tokens[i]);
		}
		printf("%s\n", line);

		if (!strncmp(line, EXIT_STR, strlen(EXIT_STR)))
			exit_received = true;
		else if (tokens[0] != NULL)
			start_process(tokens);
		else
			printf("No args\n");

		free(line);
		free(tokens);
		
	} while (!exit_received);
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