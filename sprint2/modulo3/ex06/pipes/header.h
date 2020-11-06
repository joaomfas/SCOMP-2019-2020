#ifndef HEADER_H
#define HEADER_H

#define SHM_NAME "/shm_ex06"
#define ARRAY_SIZE 1000000

typedef struct {
	char array[ARRAY_SIZE];
	int novo;
} Valores;

#endif
