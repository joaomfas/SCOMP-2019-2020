#ifndef HEADER_H
#define HEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>

int cliente();
int vendedor();

#define SHM_NAME "/shm_ex12"
#define SEM_NAME "/sem_ex12_"
#define NUMBER_TICKETS 2
#define DATA_SIZE sizeof(shared_data_type)

typedef struct {
	int bilhetes[NUMBER_TICKETS];
	int bilhetes_vendidos;
} shared_data_type;

#endif
