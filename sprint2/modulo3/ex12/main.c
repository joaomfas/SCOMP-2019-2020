
// 12.Implement a program that allows the exchange of data concerning a student between
// two processes(number, name and grades of a set of classes). The data to be exchanged 
// are represented in the following struct aluno.
//  #define STR_SIZE 502
//  #define NR_DISC 103
//  struct aluno{
//      int numero;
//      char nome[STR_SIZE];
//      int disciplinas[NR_DISC];
//  };
// The parent process should:
//      •Create a shared memory area for data exchange. Check the need to add one or more variables to synchronize the writing and reading of data operations;
//      •Create a new process;
//      •Fill the shared memory area in accordance with user-entered information;
//      •Wait until the child process ends.
//      •Eliminate the shared memory area.
// The child process should:
//      •Wait for the student data
//      •Calculates the highest, the lowest and the average grade;
//      •Print the information on the screen.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include "header.h"

int cria_filhos(int n) {
    pid_t pid;
    int i;
    for (i = 0; i < n; i++) {
        pid = fork();
        if (pid < 0) {
            return -1;
        } else if (pid == 0) {
            return i + 1;
        }
    }
    return 0;
}

int main() {

    int data_size = sizeof(struct aluno);

    /* Cria e abre a zona de memoria partilhada */
    int fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if(fd == -1) {
        perror("failed shm_open!");
        exit(EXIT_FAILURE);
    }

    /* Define o tamanho da zona de memória e inicializa-a com zero */
	if(ftruncate(fd, data_size) == -1) {
		perror("failed ftruncate!");
		exit(EXIT_FAILURE);
	}

    /* Mapeia a zona de memória, offset a zero indica que se vai usar toda a memória desde o início */
    struct aluno* shared_data = (struct aluno*) mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(shared_data == MAP_FAILED) {
        perror("failed mmap!");
        exit(EXIT_FAILURE);
    }
    
    int id = cria_filhos(1);
    
    if (id == -1) {
        perror("Fork failed!");
        exit(EXIT_FAILURE);
    }

    if(id == 0) { //Código do pai
        int nrDisc = 0;

        //•Fill the shared memory area in accordance with user-entered information;
        printf("(pai) Numero : ");
        scanf("%d", &shared_data->numero);
        printf("(pai) Nome : ");
        scanf("%s", shared_data->nome);
        printf("(pai) Nr disciplinas : ");
        scanf("%d", &nrDisc);
        int i;
        for(i=0; i<nrDisc; i++) {
            printf("(pai) disciplina %d : ", (i+1));
            scanf("%d", &shared_data->disciplinas[i]);
        }

        shared_data->nrDisciplinas = nrDisc;
        shared_data->novo = 1;

    } else { //Código dos filhos
        while(shared_data->novo != 1);

        //•Calculates the highest, the lowest and the average grade;
        int i, highest, lowest, total;
        for(i=0; i<shared_data->nrDisciplinas; i++) {
            if(i==0) {
                highest = shared_data->disciplinas[i];
                lowest = shared_data->disciplinas[i];
                total = shared_data->disciplinas[i];
            } else {
                highest = (shared_data->disciplinas[i] > highest) ? shared_data->disciplinas[i] : highest;
                lowest = (shared_data->disciplinas[i] < lowest) ? shared_data->disciplinas[i] : lowest;
                total += shared_data->disciplinas[i];
            }
        }

		printf("(filho) aluno nome  x-> %s\n, nota max: %d, nota min: %d, média: %d\n", shared_data->nome, highest, lowest, (total/shared_data->nrDisciplinas));

    }


    if (munmap(shared_data, data_size) == -1) {
		perror("failed munmap!");
		exit(EXIT_FAILURE);
	}

	if(id > 0) {
		if(close(fd) == -1) {
			perror("failed close!");
			exit(EXIT_FAILURE);
		}
	} else {
		if(shm_unlink(SHM_NAME) == -1) {
			perror("failed shm_unlink!");
			exit(EXIT_FAILURE);
		}
	} 


    return 0;
}