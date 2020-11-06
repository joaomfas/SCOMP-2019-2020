#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

int cria_filhos(int n){
	pid_t pid;
	
	int i;
	for(i = 0; i < n; i++){
		pid = fork();
		if(pid < 0){
			return -1;
		}else if(pid == 0){
			return i+1;
		}
	}
	return 0;
}

int main(void){
	int fd[2];
	enum extremidade{LEITURA=0, ESCRITA=1};
	int pid_p = 0;
	
	if(pipe(fd) == -1){
		perror("Pipe failed!");
		exit(EXIT_FAILURE);
	}
	
	int id = cria_filhos(1);
	if(id == -1){
		perror("Fork failed!");
		exit(EXIT_FAILURE);
	}
	
	if(id == 0){ //Código do pai
		close(fd[LEITURA]);
		pid_p = getpid();
		printf("Pai: O meu PID é %d\n", pid_p);
		if(write(fd[ESCRITA], &pid_p, sizeof(pid_p)) == -1){
			perror("Write failed!");
		}
		close(fd[ESCRITA]);
	}else{ //Código do filho
		close(fd[ESCRITA]);
		if(read(fd[LEITURA], &pid_p, sizeof(pid_p)) == -1){
			perror("Read failed!");
			exit(EXIT_FAILURE);
		}
		close(fd[LEITURA]);
		printf("Filho: O PID do processo pai é %d\n", pid_p);
		exit(0);
	}
	
	int status;
	pid_t p = wait(&status);
	if(p == -1){
		perror("Wait failed!");
		exit(EXIT_FAILURE);
	}
	
	return 0;
}
