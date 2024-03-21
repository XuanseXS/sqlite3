#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/sem.h>
#include <stdint.h>

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *arry;
};

int32_t SQL_semGet(int32_t semKey, int32_t index)
{
	return semget((key_t)semKey, index + 1, 0);
}

int SQL_semP(uint32_t semKey, int32_t index)
{
	int32_t semID = 0;
	struct sembuf sem_b;
	semID = SQL_semGet(semKey, index);

	if(semID == -1){
		printf("SQL_semP failed!\n");
		return -1;
	}

	sem_b.sem_num = index;
	sem_b.sem_op = -1;
	sem_b.sem_flg = SEM_UNDO;
	if(semop(semID, &sem_b, 1) == -1)
	{
		fprintf(stdout, "[dispatchCenter]:\tSQL_semP:%d failed\n", semID);
		return -1;
	}
	return 0;
}

int SQL_semV(uint32_t semKey, int32_t index)
{
	int32_t semID = 0;
	struct sembuf sem_b;
	semID = SQL_semGet(semKey, index);

	if(semID == -1){
		printf("SQL_semV failed!\n");
		return -1;
	}

	sem_b.sem_num = index;
	sem_b.sem_op = 1;
	sem_b.sem_flg = SEM_UNDO;

	if(semop(semID, &sem_b, 1) == -1)
	{
		fprintf(stdout, "[]:\tSQL_semV:%d failed\n", semID);
		return -1;
	}
	return 0;
}
