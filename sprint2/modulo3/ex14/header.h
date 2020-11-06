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

#define SHM_NAME "/shm_ex14"
#define BUFFER_SIZE 10

typedef struct {
	int array[BUFFER_SIZE];
	int escrito;
	int lido;
} circ_buffer;

#endif
