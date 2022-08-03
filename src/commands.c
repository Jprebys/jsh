#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

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