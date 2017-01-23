#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

struct COMMAND{
	char* name;
	char* desc;
	bool (*func)(int argc, char* argv[]);
};

bool cmd_cd(int argc, char* argv[]);
bool cmd_exit(int argc, char* argv[]);
bool cmd_help(int argc, char* argv[]);

struct COMMAND	builtin_cmds[]={
	{"cd", "changed directory", cmd_cd},
	{"exit", "exit this shell", cmd_exit},
	{"quit", "quit this shell", cmd_exit},
	{"help", "show this help", cmd_help},
	{"?", "show this help", cmd_help}
};
bool cmd_cd(int argc, char *argv[])
{
	if(argc == 1)
		chdir(getenv("HOME"));
	else if(argc == 2) {
		if(chdir(argv[1]))
			printf("No directory\n");
	}
	else
		printf("USAGE: cd []dir\n");
	return true;
}
bool cmd_exit(int argc, char* argv[])
{
	return false;
}
bool cmd_help(int argc, char* argv[])		//??
{
	int i;
	for(i = 0; i < sizeof(builtin_cmds) / sizeof(struct COMMAND); i++)
	{
		if(argc == 1 || strcmp(builtin_cmds[i].name, argv[1]) == 0)
			printf("%-10s: %s\n", builtin_cmds[i].name, builtin_cmds[i].desc);
	}
}
int tokenize(char* buf, char* delims, char* tokens[], int maxTokens)
{
	int token_count = 0;
	char* token;
	token = strtok(buf, delims);		//strtok? library?
	while(token != NULL && token_count < maxTokens){
		tokens[token_count] = token;
		token_count++;
		token = strtok(NULL, delims);		//reculsive하게 다음거에 넣는 것 같은데..
	}
	tokens[token_count] = NULL;
	return token_count;
}
bool run(char* line)
{
	char delims[] = "\r\n\t";
	char* tokens[128];
	int token_count;
	int i;
	int status;
	pid_t child;

	token_count = tokenize(line, delims, tokens, sizeof(tokens) / sizeof(char*));
	if(token_count == 0)		//no input?
		return true;		//again input?
	for(i = 0 ; i < sizeof(builtin_cmds) / sizeof(struct COMMAND); i++){		//COMMAND builtin_cmds / COMMAND -> COMMAND number
		if(strcmp(builtin_cmds[i].name, tokens[0]) == 0)			//if same, strcmp returns 0
			return builtin_cmds[i].func(token_count, tokens);
	}
	child = fork();
	if(child == 0){				//child proc
		execvp(tokens[0], tokens);
		printf("No such file\n");
		_exit(0);
	}
	else if(child < 0){
		printf("Failed to fork()!");
	}
	else					//parent proc
		wait(&status);			//what is status value?
	
	return true;
	
}
int main(void)
{
	char line[1024];
	while(-1){		//-1 == 1?
		printf("%s $", get_current_dir_name());
		fgets(line,sizeof(line)-1,stdin);	//null?
		if(run(line) == false)
			break;
	}
	return 0;
}
