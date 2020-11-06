#include "header.h"

void preencheFicheiroOutput(int vetorWrite[]){
    FILE *file;
    char* filePath = FILE_OUTPUT_NAME;
    file = fopen(filePath, "ab+");

    if (file == NULL){
        printf("Não foi possível abrir o arq.\n");
        exit(EXIT_FAILURE);
    }
   
    for(int i=0; i<QTD_NUMBERS_TO_WRITE; i++){
        int num = vetorWrite[i];
        fprintf(file, "%d\n", num);
    }
 
    fclose(file);
}


void readFile(char* fileName, int vetorRead[],int startPos, int qtdLeituras){
    FILE *file;
    char* filePath = fileName;
    file = fopen(filePath, "rb");

    if (file == NULL){
        printf("\nNão foi possível abrir o arq.\n");
        exit(EXIT_FAILURE);
    }

    int num = 0;
    int count=0;
    int posVetor = 0;

    //!feof (file)
     while (count < qtdLeituras){ 
        fscanf (file, "%d", &num);
        if(count >= startPos) {
            vetorRead[posVetor] = num;   
            posVetor++;
        }
        count++;
    }
    fclose (file);
    
    /**
     * Outra forma
    while ( (num = getw(file)) != EOF || count< QTD_NUMBERS_TO_READ ) {
        vetorRead[count] = num;  
        fflush(stdout);   
        milliSleep(100); 
        printf("\rNUM: %d - Indice: %d",num, count);
        count++;
    }
    **/
}


void preencheFicheiroNumbers(){

    FILE *file;

    //elimina ficheiro de output
    remove(FILE_OUTPUT_NAME);

    char* filePath = FILE_NUMBERS_NAME;
    file = fopen(filePath, "wb+");

    if (file == NULL){
        printf("\nNão foi possível abrir o arq.\n");
        exit(EXIT_FAILURE);
    }
    
    int seed = time(0);
    for(int i=0; i<QTD_NUMBERS_TO_WRITE*NUM_FILHOS; i++){
        int randomNumb = randomNumber(seed*time(0));
        //Outra forma
        //putw(randomNumb, file);
        fprintf(file, "%d\n", randomNumb);
        fflush(stdout);
        seed = randomNumb;
    }
    fclose(file);
}

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