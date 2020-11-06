#ifndef EX16_H
#define EX16_H

#define SHM_NAME "/shm_ex16"
#define SEM_NAME "/sema_ex16"
#define MAX_TAM_STR 25
#define NUM_CHILDREN 20

typedef struct {
    int east;
    int west;
} Bridge;

#endif