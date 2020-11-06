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
	int* i;
	printf("I'm...\n");
	pid_t p = fork();
	if(p == 0){
		printf("I'll never join you!\n");
		exit(0);
	}
	wait(i);
	printf("the...\n");
	p = fork();
	if(p == 0){
		printf("I'll never join you!\n");
		exit(0);
	}
	wait(i);
	printf("father!\n");
	p = fork();
	if(p == 0){
		printf("I'll never join you!\n");
		exit(0);
	}

	return 0;
}
