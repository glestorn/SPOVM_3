#include <iostream>
#include <time.h>
#include <string>
#include <semaphore.h>
#include <fcntl.h>
#include <cstring>
#include <sys/sem.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

const char* SEMAPHORE_NAME = "/my_sema";
const char* SEMAPHORE_NAME_2 = "/my_sema_2";
const int MESS_SIZE = 30;

int main(int argc, char* argv[])
{
	int processID = atoi(argv[0]);
	int pipeWr = atoi(argv[1]);
	char outputRow[MESS_SIZE];

	sprintf(outputRow, "Process%d", processID);

	// struct sigaction closesem;
	// closesem.sa_handler = closeProcess;
	// sigaction(SIGUSR1, &closesem, NULL);
	sem_t* closeProcess;
	if ((closeProcess = sem_open(argv[0], 0)) == SEM_FAILED) {
		return 1;
	}

	sem_t* print;
	if ((print = sem_open(SEMAPHORE_NAME, 0)) == SEM_FAILED) {
		return 1;
	}

	sem_t* writePipe;
	if ((writePipe = sem_open(SEMAPHORE_NAME_2, 0)) == SEM_FAILED) {
		return 1;
	}

	int value;

	while (true) {
			sem_wait(writePipe);
			write(pipeWr, outputRow, MESS_SIZE);
			sem_post(print);

			sem_getvalue(closeProcess, &value);
			if (value > 0) {
				close(pipeWr);
				break;
			}
	}

	return 0;
}