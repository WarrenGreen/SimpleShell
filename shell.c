/** @file shell.c */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "log.h"

log_t *head = NULL;
log_t *current = NULL;

int systemCall(char *line){
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
		printf("%s: not found\n", line);
		exit(-1);
	}else{
		waitpid(newpid, &exitcode,0);
		printf("Command executed by pid=%d\n", newpid);
	}

	return exitcode;
}

void logAdd(char *line){
	if(head==NULL){
		head = malloc(sizeof(log_t));
		char *newCom = malloc(sizeof(line));
		strcpy(newCom,line);
		head->command = newCom;
		log_init(head);
		current = head;
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
			log_destroy(head);
			exit(0);
		}else if (strncmp(line,"cd ", 3) ==0){
			logAdd(line);
			printf("Command executed by pid=%d\n", pid);
			
			if((chdir(line+3)) == -1)
				printf("%s: No such file or directory.\n", line+3);
		}else if(strncmp(line,"!",1)==0){
			if(strcmp(line,"!#")==0){
				log_t *l = head;
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
	log_destroy(head);
    return 0;
}
