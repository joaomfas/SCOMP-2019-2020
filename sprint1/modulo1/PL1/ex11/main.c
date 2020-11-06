/**
Write a program that initializes an array numbers with 1000 random integers in the range [0,255]. The program
should:
a) create 5 child processes that will concurrently find the maximum value of 1/5 of the array;
b) after computing the maximum value of the entire array, the parent process should create one child process
to perform the following calculation result[i]=((int) numbers[i]/max_value)*100 on half of the
array and print the result;
c) the parent process should perform the same calculation on the other half of the array;
d) both child and parent process must perform the computation concurrently, but the output must be sorted by
the array indexes. 
**/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h> 
#include <Windows.h>
#include <pthread.h>

#define MAX_NUM 255
#define MIN_NUM 0
#define NUMBER_OF_RESULT_ARRAYS 2

int newSeed = 0;
int VECTOR_SIZE = 1000;
int NUM_FILHOS = 5;
int* vectorResult[1000];

void generateVector(int[]);
int randomNumber();
int findMaxValue(int [], int inicio, int fim);
int recreateVectorParent(int [], int inicio, int fim, int maxValue);
int recreateVectorChild(int [], int [], int inicio, int fim, int maxValue);


int main(int argc, char *argv[] ) {

    int vector[VECTOR_SIZE];
    int subArraySize =  VECTOR_SIZE/NUM_FILHOS;
    int iniSubArray = 0;
    int fimSubArray = subArraySize;
    pid_t generatedPid;

    generateVector(vector);
    
    for (int i=0; i < NUM_FILHOS; ++i) {
        generatedPid = fork();

        if (generatedPid == -1) return (1);
        if (generatedPid==0) break;
        
        iniSubArray = fimSubArray;
        fimSubArray += subArraySize;
    }

    if (generatedPid == 0){
        int foundValue = findMaxValue(vector, iniSubArray, fimSubArray);
        exit(foundValue);
    }

    int maxValueReturned = -1;

    if(generatedPid >0){

        for (int i=0; i < NUM_FILHOS; i++) {
            int ret1_pidChild, status;
            ret1_pidChild = wait(&status);
            if ((status >> 8) == -1){
                printf("\n>>>>  PID filho -> %d   -  Retornei com erro %d", ret1_pidChild, (status >> 8));
            }else{
                if(maxValueReturned < (status >> 8)) maxValueReturned = (status >> 8);
                printf("\n\t\t\t\t>>>>Filho %d PID -> %d -> terminei",i+1,  ret1_pidChild);
            }
        }

        int resultSubArraySize =  VECTOR_SIZE/NUMBER_OF_RESULT_ARRAYS;
        iniSubArray = 0;
        fimSubArray = resultSubArraySize;
        int fd[2];
        pipe(fd);
        
        generatedPid = fork();

        if (generatedPid == -1  ) return (1);

        if (generatedPid == 0){

            int exitValue = -1;
            int vectorCreateByChild[VECTOR_SIZE];
            
            printf("\n ini: %d  final: %d max: %d",iniSubArray,fimSubArray, maxValueReturned);
            exitValue = recreateVectorChild(vectorCreateByChild, vector, iniSubArray, fimSubArray, maxValueReturned);

            close(fd[0]);  
            close(1);     
            dup(fd[1]);
            write(1, vectorCreateByChild, sizeof(vectorCreateByChild)); 

            exit(exitValue);
        }

        if(generatedPid >0){

            printf("\n\t\t\t\t>>>>Max num encontrado %d", maxValueReturned);

            iniSubArray = resultSubArraySize;
            fimSubArray += resultSubArraySize;

            recreateVectorParent(vector, iniSubArray, fimSubArray, maxValueReturned);

            int ret1_pidChild, status;
            ret1_pidChild = wait(&status);
            close(fd[1]);  
            dup(fd[0]);  
            
            int vectorFromChild[VECTOR_SIZE]; 
            int n = read(fd[0], vectorFromChild, sizeof(vectorFromChild));
            
            if ((status >> 8) == -1){
                printf("\n>>>>  PID filho -> %d   -  Retornei com erro %d", ret1_pidChild, (status >> 8));
            }else{
                printf("\n\t\t\t\t>>>>Filho Final PID filho -> %d   -  retornei com cod %d", ret1_pidChild , (status >> 8));
                for(int i=0; i<VECTOR_SIZE; i++){
                    if(i < (n/8)) vector[i] = vectorFromChild[i];
                    printf("\nPos %d  value -> %d",i, vector[i]);
                }
            }
        }
    }

    return 0;
};


void generateVector(int vector[]){
    for(int i=0; i<VECTOR_SIZE; i++){
        vector[i] = randomNumber((time(0)));
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

int recreateVectorChild(int vectorChild[], int vector[], int inicio, int fim, int maxValue){
    for(int i=inicio; i < fim; i++){
        float vec = vector[i];
        float max = maxValue;
        int result =(vec/max)*100;
        vectorChild[i] = result;
    }
    return 1;
};

int recreateVectorParent(int vector[], int inicio, int fim, int maxValue){
    for(int i=inicio; i < fim; i++){
        float vec = vector[i];
        float max = maxValue;
        int result =(vec/max)*100;
        vector[i] = result;
    }
    return 1;
};

int randomNumber(int x){
    srand(time(0)+newSeed+x);
    newSeed = MIN_NUM + rand() / (RAND_MAX / (MAX_NUM - MIN_NUM) + 1);
    return newSeed;
}
