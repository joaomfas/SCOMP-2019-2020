/*
 * ex02.c
 *
 *  Created on: 20/02/2020
 *      Author: isep
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(void){
	pid_t pid[2];
	int s;
	int i;

	for(i = 0; i<2; i++){
		pid[i] = fork();
		if(pid[i] == 0){
			break;
		}
	}

	if(pid[i] == 0){
		sleep(i+1);
		exit(i+1);
	}

	for(i = 0; i<2; i++){
		waitpid(pid[i], &s, 0);
		if(WIFEXITED(s)){
			printf("Filho %d: %d\n", i+1, WEXITSTATUS(s));
		}
	}

	return 0;
}
