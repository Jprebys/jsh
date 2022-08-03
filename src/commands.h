#ifndef COMMANDS_H_
#define COMMANDS_H_


typedef struct jsh_settings{
	char *cwd;
	char *uname;
} jsh_settings;


void run_cd_cmd(char **tokens, jsh_settings *stgs);

#endif