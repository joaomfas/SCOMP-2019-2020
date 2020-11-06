#ifndef EX15_H
#define EX15_H

#define SHM_NAME "/shm_ex15"
#define SEM_NAME "/sema_ex15"
#define MAX_TAM_STR 25
#define NUM_CHILDREN 17
#define MAX_SEATS 5
#define SHOW_TIME 2
#define SHOW_INTERVAL 1

typedef struct {
    int seats;
    int fila;
    int terminou;
} Show;

#endif