/** @file shell.c */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "log.h"

log_t *log = NULL;
log_t *current = NULL;

int systemCall(char *line){
	int pid = getpid();
	int exitcode;
	char *delim = " ";
	int count = 0;
	int i;
	for(i=0;i<(int)sizeof(line);i++)
		if(line[i]==(char)" ") count++;

	char *token = strtok(line, delim);
	char *array[count+1];
	for(count = 0; token!=NULL;count++){
		strcat(token,"\0");
		array[count] = token;
		token = strtok(NULL, delim);
	}
	array[count] = NULL;

	pid_t newpid = fork();
	if(newpid == 0){
		execv(line, array);
	}else{
		waitpid(newpid, &exitcode,0);
		printf("Command executed by pid=%d\n", newpid);
	}

	return exitcode;
}

void logAdd(char *line){
	if(log==NULL){
		log = malloc(sizeof(log_t));
		char *newCom = malloc(sizeof(line));
		strcpy(newCom,line);
		log->command = newCom;
		log_init(log);
		current = log;
	}else{
		current = log_push(current,line);
	}
}

void simple_shell(){
	char *line = NULL;
	size_t size;
	char cwd[1024];
	int pid = getpid();
	
	while(1){
		if(getcwd(cwd,sizeof(cwd))!=NULL)
			printf("(pid=%d)%s$ ",pid,cwd);
		else
			printf("(pid=%d)$ ",getpid());
		fflush(stdout);
		getline(&line, &size,stdin);

		line[ strlen(line)-1] = '\0';

		if(strcmp(line,"exit")==0){
			logAdd(line);
			printf("Command executed by pid=%d\n", pid);
			free(line);
			exit(0);
		}else if (strncmp(line,"cd ", 3) ==0){
			logAdd(line);
			printf("Command executed by pid=%d\n", pid);
			chdir(line+3);
		}else if(strncmp(line,"!",1)==0){
			if(strcmp(line,"!#")==0){
				log_t *l = log;
				while(l!=NULL){
					printf("%s\n", l->command);
					l = l->next;
				}
				printf("Command executed by pid=%d\n", pid);
			}else{
				char *command = log_search(current, line);
				printf("%s matches %s\n",line, command);
				logAdd(command);
				systemCall(command);
			}
		}else{
			logAdd(line);
			systemCall(line);

		}
		

		free(line);
		line = NULL;
	}

	free(line);
}

/**
 * Starting point for shell.
 */
int main() {
	simple_shell();
    return 0;
}