#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <sys/wait.h>
#include "constants.h"
#include "utils.h"
#include "commands.h"


char *get_stdin_line(jsh_settings *stgs)
{
	size_t prompt_len = strlen(stgs->uname) 
					    + strlen(stgs->cwd) 
					    + JSH_PROMPT_LEN
					    + 2 + 1;
	char *prompt = malloc(prompt_len);
	snprintf(prompt, prompt_len, "[%s]%s%s\n", 
		     stgs->uname, stgs->cwd, JSH_PROMPT);
	char *line = readline(prompt);
	free(prompt);
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


void start_process(char **tokens, jsh_settings *stgs) 
{
	if (!strncmp(tokens[0], CD_CMD, CD_CMD_LEN)) {
		run_cd_cmd(tokens, stgs);
		return;
	} else if (!strncmp(tokens[0], CAL_CMD, CAL_CMD_LEN)) {
		// cal can be invoked with 'cal' or 'cal yyyy-mm-dd'
		run_cal_cmd(tokens[1]);
		return;
	}

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


void jsh_main_loop_run(jsh_settings *stgs)
{
	char *line, **tokens;
	bool exit_received = false;
	do {
		line = get_stdin_line(stgs);
		if (line == NULL) {
			free(line);
			break;
		}
		tokens = split_line(line);

		if (!strncmp(line, EXIT_STR, strlen(EXIT_STR)))
			exit_received = true;
		else if (tokens[0] != NULL)
			start_process(tokens, stgs);
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

	// run main shell loop
	jsh_main_loop_run(stgs);

	// TODO: add any cleanup here
	cleanup_settings(stgs);

	return 0;
}
