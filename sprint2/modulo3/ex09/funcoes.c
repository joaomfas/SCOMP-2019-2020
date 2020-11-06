#include "header.h"

clock_t t1, t2;

void iniciaVetorResultado(VetorEstrutura* estrutura){
    estrutura->flagInicio=0;
	estrutura->seqEscrita=0;
	for(int count=0; count<NUM_FILHOS; count++){
		estrutura->vector[count] = 0;
	}
}


void generateVector(int vector[]){ 
    printf("\n\n*** GERANDO VETOR INICIAL ***\n\n");
    int count = 0;
    int seed = time(0);
    for(int i=0; i<VECTOR_SIZE; i++){
        if(i>0) seed = vector[i-1];
        vector[i] = randomNumber(seed*time(0));
        char *loadBar =  defineLoadPos(count);
        milliSleep(10);
        printf("\rRandom number: %d\t%s\tCiclo: %d\t\t\r", vector[i], loadBar, i);
        fflush(stdout);
        if(count<40){
            count++;
        }else{
            count=0;
        } 
    }
};


int findMaxValue(int vector[], int inicio, int fim){
    int maxValue = -1;
    for(int i=inicio; i < fim; i++){
        if (vector[i] > maxValue){
            maxValue = vector[i];
        } 
    }
    return maxValue;
};

int randomNumber(int x){
    srand(x);
    int randomNum = MIN_NUM + rand() / (RAND_MAX / (MAX_NUM - MIN_NUM) + 1);
    return randomNum;
};

void milliSleep(int time){
    struct timeval tv;
    tv.tv_sec  = time / 1000;
    tv.tv_usec = (time % 1000) * 1000;
    select (0, NULL, NULL, NULL, &tv);
};

char* defineLoadPos(int count){
    char *posicao = "|";
    switch (count){
        case 0 ... 9:
            posicao = "|";
            break;
        case 10 ... 19:
            posicao = "/";
            break;
        case 20 ... 29:
            posicao = "-";
            break;
        case 30 ... 39:
            posicao = "\\";
            break;
        default:
            break;
    };
    return posicao;
};
