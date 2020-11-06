#ifndef EX14_H
#define EX14_H

#define SHM_NAME "/shm_ex14"
#define SEM_NAME "/sema_ex14"
#define MAX_TAM_STR 25
#define MAX_WRITES 1000
#define MAX_LINHA 200

typedef struct {
    int readcount;
    int writecount;
    char linha[MAX_WRITES][MAX_LINHA];
    int cur;
} Dados;

#endif
