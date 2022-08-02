#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <sys/wait.h>

#define MAX_LINE_LENGTH 256
#define MAX_DIR_NAME 256
#define MAX_TOKENS 128
#define EXIT_STR "exit"
#define JSH_DELIMITER " "
#define JSH_PROMPT ">>"
#define BIN_DIR "/usr/bin"
#define JOIN_CHAR "/"

typedef struct {
	char *cwd;
	char *uname;
} jsh_settings;


char* join_strings(char* base, char* fname, char *delim) 
{
	size_t base_size = strlen(base);
	size_t fname_size = strlen(fname);
	size_t delim_size = strlen(delim);
	char* begin = malloc(base_size + delim_size + fname_size + 1);
	char* end = begin;
	memcpy(end, base, base_size);
	end += base_size;
	memcpy(end, delim, delim_size);
	end += delim_size;
	memcpy(end, fname, fname_size);
	end += fname_size;
	*end = '\0';

	return begin;
}


char *get_stdin_line()
{
	char *prompt = JSH_PROMPT;
	char *line = readline(prompt);
	return line;
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


void start_process(char **tokens) 
{
	pid_t pid = fork();
	switch (pid) {
		case -1:    // error
			perror("Fork");
			exit(1);
		case  0:    // child
			char *f_path = join_strings(BIN_DIR, tokens[0], JOIN_CHAR);
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
		tokens = split_line(line);
		printf("Tokens: ");
		for (size_t i = 0; tokens[i] != NULL; ++i) {
			printf("[%s]", tokens[i]);
		}
		printf("\n");

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


jsh_settings *initialize_settings()
{
	jsh_settings *result = malloc(sizeof(jsh_settings));
	char *cwd = malloc(MAX_DIR_NAME);
	char *uname = getlogin();

	getcwd(cwd, MAX_DIR_NAME);

	if (cwd == NULL) {
		perror("getwd");
		exit(1);
	} else if (uname == NULL) {
		perror("getlogin");
		exit(1);
	}

	result->cwd = cwd;
	result->uname = uname;

	return result;
}


void cleanup_settings(jsh_settings *stgs)
{
	free(stgs->cwd);
	free(stgs);
}


int main()
{
	// TODO: parse command line args
	// TODO: read config file here

	jsh_settings *stgs = initialize_settings();
	printf("[%s]%s\n", stgs->uname, stgs->cwd);


	// run main shell loop
	jsh_main_loop_run(stgs);

	// TODO: add any cleanup here
	cleanup_settings(stgs);

	return 0;
}
