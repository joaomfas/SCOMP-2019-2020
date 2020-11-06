/**

11. Implement a program similar to 9, but now the child processes send the local maximum using a pipe, shared by all
processes.

"9. Implement a program to determine the biggest element of an array in a parallel/concurrent environment. The
parent process should:
• Create an array of 1000 integers, initializing it with random values between 0 and 1000;
• Create a shared memory area to store an array of 10 integers, each containing the local maximum of 100 values;
• Create 10 new processes;
• Wait until the 10 child processes finish the search for the local maximum;
• Determine the global maximum;
• Eliminate the shared memory area.
Each child process should:
• Calculate the largest element in the 100 positions;
• Write the value found in the position corresponding to the order number (0-9) in the array of local maximum"

**/

#include "header.h"

int main(int argc, char *argv[]){

	enum extremidade {
        LEITURA = 0, ESCRITA = 1
    };

	const int DATA_VETOR_SIZE = sizeof(VetorEstrutura);
	int i, pos, iniSubArray, fimSubArray, processSequenceNumber = 0;

	int vetor[VECTOR_SIZE];

	//para pipe
	int fd[2];

	//para memória partilhada
	int	sharedMemoryArea;

	pid_t generatedPid;

	generateVector(vetor);

	//ABRE O PIPE DE COMUNICAÇÂO
	if (pipe(fd) == -1) {
        perror("Pipe failed!");
        exit(EXIT_FAILURE);
    }

	//O pai cria a área de memória partilhada para registar o objeto que será manipulado/lido

	if ((sharedMemoryArea = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) == -1){
		perror("failed shm_open in parent!!!\n");
		exit(EXIT_FAILURE);
	}

	if (ftruncate(sharedMemoryArea, DATA_VETOR_SIZE) == -1){
		perror("failed ftruncate!!!\n");
		exit(EXIT_FAILURE);
	}

	VetorEstrutura* sharedVetorPai = (VetorEstrutura *)mmap(NULL, DATA_VETOR_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemoryArea, 0);

	if (sharedVetorPai == MAP_FAILED){
		perror("failed mmap!!!\n");
		exit(EXIT_FAILURE);
	}

	//Pai inicia os valores necessários na estrutura registada na área de memória partilhada
	iniciaVetorResultado(sharedVetorPai);

	//o pai cria os processos e distribui os inicios e fins dos subarays para os filhos pesquisarem o maxNUm
	for (pos = 0; pos < NUM_FILHOS; pos++){
        processSequenceNumber = pos;
		if(pos==0){
			 iniSubArray = 0;
			 fimSubArray = 99;
		} else {
			iniSubArray = pos*100;
			fimSubArray = (pos+1)*100-1;
		}

        generatedPid = fork();
        if (generatedPid == -1)
            return (1);
        if (generatedPid == 0)
            break;
    }

	if (generatedPid == 0) {
		//se o pid for igual a zero após o fork, este é o filho

		//os filhos só precisam escrever no pipe, por isso fechamos a leitura
		close(fd[LEITURA]);

		int sharedMemoryArea;

		//Abre a zona de memória criada (previamente) pelo PAI
		if ((sharedMemoryArea = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR)) == -1){
			perror("failed shm_open in child!!!\n");
			exit(EXIT_FAILURE);
		}

		//cria um apontador que recebe o objeto que está registado na área de memória
		VetorEstrutura* sharedVetorProcesso = (VetorEstrutura *)mmap(NULL, DATA_VETOR_SIZE,  PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemoryArea, 0);

		if (sharedVetorProcesso == MAP_FAILED){
			perror("failed mmap!!!\n");
			exit(EXIT_FAILURE);
		}

		//Verifica se o número de sequência para ler/escrever na memória partilhada é igual ao de sequência do processo
		while (sharedVetorProcesso->flagInicio != 1);

		//após o sinal verde do pai, os filhos na sequência irão aceder a memória partilhada
		while (sharedVetorProcesso->seqEscrita != processSequenceNumber);

		//estrutura para a resposta enviada no pipe
		ResponsePipe response;
		response.processSequenceNumber = processSequenceNumber;

		//Procura o max no subvetor
		response.maxNum =  findMaxValue(vetor, iniSubArray, fimSubArray);

		//envia a resposta ao pai;
		write(fd[ESCRITA], &response, sizeof(response));

		//encerra o pipe apos enviar a resposta
		close(fd[ESCRITA]);

		if((processSequenceNumber!=NUM_FILHOS-1)) sharedVetorProcesso->seqEscrita++;

		//ficará neste ciclo até que chegue ao último processo que terá a seqEscrita igual ao processNumber pois não entrou no IF acima
		//A partir daqui os filhos vão encerrando um a um de forma decrescente
		while (sharedVetorProcesso->seqEscrita != processSequenceNumber);
		sharedVetorProcesso->seqEscrita--;

		if (munmap(sharedVetorProcesso, DATA_VETOR_SIZE) == -1){
			perror("failed munmap!!!\n");
			exit(EXIT_FAILURE);
		}

		//apenas o último processo vivo faz o unlink da memória partilhada
		if(processSequenceNumber==0){
			if (shm_unlink(SHM_NAME) == -1){
				printf("\nPSN: %d - UNLINK!",processSequenceNumber);
				perror("failed shm_unlink!\n");
				exit(EXIT_FAILURE);
			}
		}

        exit(EXIT_SUCCESS);
    }

	//se o pid não for igual a zero, passa a executar a partir daqui, este é o pai
	//Código do pai

	//O pai só precisa ler o pipe, então fechamos a escrita
	close(fd[ESCRITA]);

	sharedVetorPai->flagInicio = 1; //permite que os filhos comecem a procurar

	//Criamos um vetor à parte para armazenar as respostas, poderia estar junto na estrutura inicial do vetor dos valores aleatórios também
	int vetorResultadosMaxNum[VECTOR_RESULT_SIZE];

	//Este laço é para o pai ler todos os retornos dos filhos quando enviarem cada um o max num
	for (i = 0; i <  NUM_FILHOS; i++){
            printf("\n");
			ResponsePipe response;
            read(fd[LEITURA], &response, sizeof(response));
			//Prints para teste
			//printf("\nPIPE - Pos: %d Valor: %d ", response.processSequenceNumber, response.maxNum);
			vetorResultadosMaxNum[response.processSequenceNumber] = response.maxNum;
			//printf("\nVETOR %d", vetorResultadosMaxNum[response.processSequenceNumber]);
        }

	//O pai não vai ler mais do pipe, por isso podemos fechar a extremidade de leitura
	close(fd[LEITURA]);

    //Depois de ler todos os valores enviados pelo pipe pelos filhos, o pai procura o maxTotal e imprime o vetor final
	int posVec = 0;
	int foundMaxValueTotal = findMaxValue(vetorResultadosMaxNum, posVec, VECTOR_RESULT_SIZE-1);

	printf("Vetor final:\n");
	for(posVec = 0; posVec < VECTOR_RESULT_SIZE; posVec++){
		printf("\n%d\t%d",posVec+1, vetorResultadosMaxNum[posVec]);
	}
	printf("\n\nMaior valor encontrado: %d\n", foundMaxValueTotal);
	printf("Valores recebidos!\n");


	//Depois de imprimir os resultados, o Pai espera que os filhos terminem
    int status;
    for(i = 0; i < NUM_FILHOS; i++) {
        pid_t p = wait(&status);
        if (p == -1) {
            perror("Wait failed!");
            exit(EXIT_FAILURE);
        }
    }

	//E por fim o pai encerra o espaço de memória partilhado criado inicialmente
	if (munmap(sharedVetorPai, DATA_VETOR_SIZE) == -1){
		perror("failed munmap!!!\n");
		exit(EXIT_FAILURE);
	}
	if (close(sharedMemoryArea) == -1){
		perror("failed munmap!!!\n");
		exit(EXIT_FAILURE);
	}

    return 0;
}
