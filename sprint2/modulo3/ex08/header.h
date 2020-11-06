#ifndef HEADER_H
#define HEADER_H

#define SHM_NAME "/shm_ex8"
#define INIT_VALUE 100
#define OPERATIONS 1000000
#define NUM_CHILDREN 1

typedef struct {
	int Integer;
	int novo;
} StoredInt;

#endif