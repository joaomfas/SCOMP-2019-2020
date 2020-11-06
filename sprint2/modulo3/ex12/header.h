#ifndef HEADER_H
#define HEADER_H

#define SHM_NAME "/shm_ex12"
#define STR_SIZE 502
#define NR_DISC 103
#define NUM_CHILDREN 1

struct aluno{
    int numero;
    char nome[STR_SIZE];
    int disciplinas[NR_DISC];
    int nrDisciplinas;
    int novo;
};

#endif