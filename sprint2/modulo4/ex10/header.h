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

int inserir();
int consultar();
int consultar_todos();

#define SHM_NAME "/shm_ex10"
#define SEM_NAME "/sem_ex10"
#define SEM_APP_NAME "/sem_app_ex10"
#define NUMBER_RECORDS 100
#define DATA_SIZE sizeof(shared_data_type)

typedef struct{
	char nome[255];
	char morada[255];
	int num;
} Record;

typedef struct {
	Record records[NUMBER_RECORDS];
	int saved_records;
} shared_data_type;

#endif
