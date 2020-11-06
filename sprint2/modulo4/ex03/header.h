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

#define SHM_NAME "/shm_ex03"
#define SEM_NAME "/sem_ex03"
#define NUM_STRS 50
#define NUM_CHARS 80

typedef struct {
	char str[NUM_STRS][NUM_CHARS];
	int written;
} shared_data_type;

#endif
