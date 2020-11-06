#ifndef HEADER_H
#define HEADER_H

#define SHM_NAME "/shm_ex03"
#define ARRAY_SIZE 100000
#define BUFFER_SIZE 100

typedef struct {
	int number;
	char strg[BUFFER_SIZE];
} S2;

typedef struct {
	S2 s2[ARRAY_SIZE];
	int novo;
} S1;

#endif
