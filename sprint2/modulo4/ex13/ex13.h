#ifndef EX09_H
#define EX09_H

#define SHM_NAME "/shm_ex13"
#define SEM_NAME "/sema_ex13"
#define MAX_TAM_STR 25

#define BUFFER 10
#define MAX_CONSUMPTION 30

typedef struct {
    int number[BUFFER];
    int head;
    int tail;
    int buffer;
    int consumption;
} CircularBuffer;

void produce(CircularBuffer*);
void consume(CircularBuffer*);
void printcb(CircularBuffer*);

#endif
