#ifndef HEADER_H
#define HEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>

int cliente();
int vendedor();

#define SHM_NAME "/shm_ex17"
#define SEM_NAME "/sem_ex17_"
#define DATA_SIZE sizeof(shared_data_type)

#define MAX_LUGARES 300

#define NUM_SEMS 4

typedef struct {
	int vipEspera;
	int especialEspera;
} shared_data_type;


// ONLY FOR THOSE WHO CREATE THE SEMAFORO
void __semaforo__CREATE_AND_OPEN(sem_t *semaforo, char* fileName, int initialValue) {
    if ((semaforo = sem_open(fileName, O_CREAT | O_EXCL, 0644, initialValue)) == SEM_FAILED) {
        perror("Error at sem_open()!\n");
        exit(EXIT_FAILURE);
    }
}

// ONLY FOR THOSE WHO OPEN THE SEMAFORO
// no caso dos filhos, so quererem abrir o semaforo temos q fazer apenas -> sem_open(fileName, 0)
void __semaforo__OPEN(sem_t *semaforo, char* fileName) {
    if ((semaforo = sem_open(fileName, 0)) == SEM_FAILED) {
        perror("Error at sem_open()!\n");
        exit(EXIT_FAILURE);
    }
}

void __semaforo__CLOSE(sem_t *semaforo) {
    if (sem_close(semaforo) < 0) {
        perror("Error at sem_close()!\n");
        exit(EXIT_FAILURE);
    }
}

void __semaforo__UNLINK(char* fileName) {
    if (sem_unlink(fileName) != 0) {
        int err = errno;
        printf("Error: %s", strerror(err));
        printf("Error at shm_unlink() of semaforo!\n");
        exit(EXIT_FAILURE);
    }
}

// Shared Memory Utils
//===============================================================================================================

// int oflag = O_RDWR | O_EXCL | O_CREAT

shared_data_type *__shmOPEN(char *fileName, int *fd, int oflag) {
    // 1. Creates and opens a shared memory area
    *fd = shm_open(fileName, oflag, S_IRUSR | S_IWUSR);
    if (*fd == -1) {
        perror("Error at shm_open()!\n");
        exit(EXIT_FAILURE);
    }

    // 2. Defines the size
    ftruncate(*fd, sizeof(shared_data_type));

    // 3. Get a pointer to the data
    shared_data_type *sharedData = (shared_data_type *)mmap(NULL, sizeof(shared_data_type), PROT_READ | PROT_WRITE, MAP_SHARED, *fd, 0);

    return sharedData;
}

void __shmCLOSE(shared_data_type *sharedData, int fd) {
    // Undo mapping
    if (munmap((void *)sharedData, sizeof(shared_data_type)) < 0) {
        perror("Error at munmap()!\n");
        exit(EXIT_FAILURE);
    }
    // Close file descriptor
    if (close(fd) < 0) {
        perror("Error at close()!\n");
        exit(EXIT_FAILURE);
    }
}

void __shmDELETE(char *fileName) {
    // Remove file from system
    if (shm_unlink(fileName) < 0) {
        perror("Error at shm_unlink()!\n");
        exit(EXIT_FAILURE);
    }
}

#endif
