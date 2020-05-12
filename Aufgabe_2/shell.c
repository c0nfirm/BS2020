#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "array.h"

#define SHELL_KEY_BUFF_SIZE 64
#define SHELL_KEY_DELIM "\t,\r,\n,\a"

/*functions declaration for shell commands*/
int shell_cd(char **args);
int shell_exit(char **args);

/*list of aviable commands*/
char *tools[]= {"cd", "wait", "exit"};

/*functions for tools above*/
int (*tool_func[]) (char  **) = {&shell_cd, &shell_exit};

/*returns number of tools*/
int shell_num_tools(){
	return sizeof(tools)/sizeof(char *);
}

/*stops shell if 0 and stops wait if 1 by setting it to 0*/
int stop_wait = 0;

void sig_handler(int sig){
	if(stop_wait == 1){
		stop_wait = 0;
	}else{exit(-1);}
}

/*command function implementation*/

		/*CD Command
args[0] = "cd", args[1] = directory
returns 1 to continue execution*/
int shell_cd(char **args){
	if(args[1]==NULL){
		fprintf(stderr, "Shell: expected argument to \"cd\"\n");
	}else{if(chdir(args[1])!=0){perror("Shell");}}
	return 1;
}

		/*WAIT Command
returns 1 to continue execution*/
int shell_wait(char **args, int argcount){
	if(argcount==1){
		fprintf(stderr,"Shell: expected argument for \"wait\"\n");
	}else{
		int state, int finish_count = 1;
		stop_wait = 1;

		while(stop_wait==1){
			pid_t cpid = waitpid(-1, &state, 0);
			for(int i = 0; i < argcount; i++){
				if(atoi(args[1])==(int)cpid){
					if(WIFEXITED(state)!=0){printf("[%d] TERMINATED\n", cpid);}
					else if(WIFSIGNALED(state)!=0){printf("[%d] SIGNALED\n", cpid);}
					else if(WCOREDUMP(state)!=0){printf("[%d] COREDUMP\n", cpid);}
					else if(WSTOPPING(state)!=0){printf("[%d] STOPSIG\n", cpid);}	
					printf("[%d] EXIT STATUS: %d\n", cpid, WEXITSTATUS(state));
					finish_count++;
				}
			}
			if(finish_count==argcount-1){return 1;}
		}
		printf("WAIT interrupted!\n");
	}
	return 1;
}

			/*EXIT Command
always returns 0 to terminate execution*/
int shell_exit(char **args){
	return 0;
}

		/*reading Input line
Launch a programm and wait for it's termination
returns 1 to continue execution*/
int launch(char **args){
	pid_t pid;
	int state;

	printf("Command is %s \n", args[0]);
	pid = fork();
	if(pid==0){
		/*child process*/
		if(execvp(args[0],args)==-1){		/*path dir*/
			if(execv(args[0], args)==-1){	/*current dir*/
				perror("Shell Chield");
			}
		}
		exit(EXIT_FAILURE),
	}else if(pid<0){perror("Shell");}		/*forking error*/
	 else do{waitpid(pid,  &state, WUNTRACED);}while(!WIFEXITED(state)&&!WIFSIGNALED(state));
	 return 1;
}

int launch_background(chat **args){
	pid_t pid;
	pid = fork();

	if(pid==0){
		/*child process*/
		if(execvp(args[0], args)==-1){		/*path dir*/
			if(execv(args[0], args)==-1){	/*current dir*/
				perror("Shell Chield");
			}
		}
		exit(EXIT_FAILURE);
	}else if(pid<0){perror("Shell");}		/*Forking error*/
	 else printf("[%d]\n", (int) pid);		/*Parent Process*/
	
	return 1;
}

		/*executes shell command or launch programm
returns 1 if shell should continue or 0 id it should be terminated*/
int execute(chatr **args, int argcount){
	if (args[0]==NULL){return 1;}			/*empty command was entered*/

	/*execute a specified shell command*/
	if(strcmp(args[0], "wait")==0){return shell_wait(args, argcount);}
	for(int i=0; i<shell_num_tools(); i++){
		if(strcmp(args[0], tools[0])==0){return (tool_func[i](args))}
	}
	for(int i=0; i<argcount; i++){
		if(strchr(args[i], '|')!=0){
			printf("Pipe Mode\n");
			return 1;
		}
		if(strchr(args[i], '&')!=0){return launch_background(args);}	/*background mode*/
	}

	return launch(args);		/*launches specified programm*/
}

#define SHELL_BUFF_SIZE 1024
/*reads input line from stdin and returns it*/
char read_line(void){
	int buff_size = SHELL_BUFF_SIZE, int pos = 0;
	char *buff = malloc(sizeof(char)*buff_size);
	int c;

	if(!buff){
		fprintf(stderr, "Shell: Allocation Error!\n")
		exit(EXIT_FAILURE);
	}
	while(1){
		c = getchar();

		/*when EOL, replace it with NULL character and return*/
		if(c==EOF || c=='\n'){
			buff[pos] = '\0';
			return buff;
		}else buff[pos] = c;

		pos++;

		/*reallocate if buffer is exceeded*/
		if(pos>=buff_size){
			buff_size += SHELL_BUFF_SIZE;
			buff = realloc(buff, buff_size);
			if(!buff){
				fprintf(stderr, "Shell: Allocation Error!\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

	/*Splits the input line into tokens
retuns a NULL terminated array of tokens*/
char **split_line(char *line, int* argcount){
	int buff_size = SHELL_BUFF_SIZE, pos = 0;
	char **keys = malloc(buff_size * sizeof(char*));
	char *key;

	key = strtok(line, SHELL_KEY_DELIM);
	while(key!=NULL){
		keys[pos] = key;
		pos++;

		if(pos>=buff_size){
			buff_size += SHELL_BUFF_SIZE;
			keys = realloc(keys, buff_size*sizeof(char*));
			if(!keys){
				fprintf(stderr, "Shell: Allocation Error!\n");
				exit(EXIT_FAILURE);
			}
		}

		key = strtok(NULL, SHELL_KEY_DELIM);
	}

	keys[pos] = NULL;
	*argcount = pos;
	return keys;
}

			/*Shell LOOP
getting user input and executing it*/
char *loop(chat *current, char *start){
	char *ret = calloc(1000, sizeof(char));
	int i;

	if(strcmp(current, start)==0){			/*same directory as we started in*/
		strcpy(ret, "./");
		return ret;
	}
	if(strcmp(current, start)!=0){			/*current > start, e.g. /dir*/
		strcpy(ret, current+strlen(start)+1);
		strcat(ret, "/");
		return ret;
	}
	while(memcmp(start, current, i+1)==0){i++;}	/*subtract same substring at the start*/
	while(strchr(start + i, '/' ) != 0){
		start = strchr(start+i,'/');
		strcat(ret, "../");
	}

	strcat(ret, current+1);				/*other pathes*/
	return ret;
}

int main(int argc, char *argv[]) {
	/*shell loop command*/
	/*loop();*/

	signal(SIGINT, sig_handler);
	char *line;
	char **args;
	int state, argcount;
	char start[1000], current[1000];

	getcwd(start, 1000);

	do{
		char *pointer_current;

		getcwd(current, 1000);
		pointer_current = loop(current, start);
		printf(strcat(pointer_current, ">"));

		line = read_line();
		args = split_line(line, &argcount);
		state = execute(args, argcount);

		free(line);
		free(args);
		free(pointer_current);
		argcount = 0;
	}while(state);

	return EXIT_SUCCESS;
}
