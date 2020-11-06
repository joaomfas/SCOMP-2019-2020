#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h> 
#include <Windows.h> 

#define MAX_NUM 200
#define MIN_NUM 0

void generateVector(int[]);
int firstOcccurrence(int[], int );
int randomNumber();

int newSeed = 0;
int VECTOR_SIZE = 2000;
int NUM_FILHOS = 10;

int main(int argc, char *argv[] ) {

    //verifica existencia de argumentos e faz atribuicao
    if(argc > 1) {
        for(int i=0; i< argc; i++){
            if(i==1) VECTOR_SIZE = atoi(argv[i]);
            if(i==2) NUM_FILHOS = atoi(argv[i]);
        }
    }

    //define as dimensoes dos vetores
    int vector[VECTOR_SIZE],
    numberToFind;
    
    pid_t generatedPid;
    
    generateVector(vector);
    numberToFind = randomNumber(0);
    
    for (int i=0; i < NUM_FILHOS; ++i) {
        generatedPid = fork();
        if (generatedPid == -1) return (1);
        if (generatedPid==0) break;
    }

    if (generatedPid == 0){    
        int foundValue = 255;
        foundValue = firstOcccurrence(vector, numberToFind);
        exit(foundValue);
    }

    if(generatedPid >0){
        for (int i=0; i < NUM_FILHOS; i++) {
            int ret1_pidChild, status;
            ret1_pidChild = wait(&status);
            if ((status >> 8) == 255){
                printf("\n>>>>  PID filho -> %d   -  Não encontrei o valor %d", ret1_pidChild, numberToFind);
            }else{
                printf("\n\t\t\t\t\t\t\t>>>> PID filho -> %d   -  Encontrei o valor %d na posição %d", ret1_pidChild, numberToFind , (status >> 8)+1);
            }
        }
    }

    return 0;
};

void generateVector(int vector[]){
    for(int i=0; i<VECTOR_SIZE; i++){
        vector[i] = randomNumber(i+time(0));
    }
};

int firstOcccurrence(int vector[], int numberToFind ){
    for(int i=0; i<VECTOR_SIZE; i++){
        if (vector[i] == numberToFind) return i;
    }
    return 255;
};

int randomNumber(int x){
    srand(time(0)+newSeed+x);
    newSeed = MIN_NUM + rand() / (RAND_MAX / (MAX_NUM - MIN_NUM) + 1);
    return newSeed;
}
