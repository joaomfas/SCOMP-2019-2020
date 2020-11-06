#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include <errno.h>


#define SHM_NAME "/shm_ex_13"
#define FILE_NUMBERS_NAME "arqs/Numbers.txt"
#define FILE_OUTPUT_NAME "arqs/Output.txt"

#define QTD_NUMBERS_TO_READ 200
#define QTD_NUMBERS_TO_WRITE 200
#define NUM_FILHOS 8

#define MIN_NUM 0
#define MAX_NUM 1000

#define SEM_01_NAME "mutex_01"
#define SEM_02_NAME "mutex_02"

void preencheFicheiroNumbers();
void readFile(char [], int [], int, int);
void preencheFicheiroOutput(int []);
int randomNumber(int x);
void milliSleep(int time);

#endif
