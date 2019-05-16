#include <iostream>
#include <unistd.h>
#include <sys/types.h>
 #include <stdlib.h>
#include <string>
#include <cstring>
#include <vector>
#include <termios.h>
#include <semaphore.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <fcntl.h>

const char* SEMAPHORE_NAME = "/my_sema";
const char* SEMAPHORE_NAME_2 = "/my_sema_2";
const int MESS_SIZE = 30;
// int Close = 0;

// void endProcess(int sig)
// {
// 	Close = 1;
// }

int main(int argc, char* argv[])
{
	sem_t* closeParent;
	const char* parentString = "Parent";
	if ((closeParent = sem_open(parentString, 0)) == SEM_FAILED) {
		return 1;
	}

	int pipeRd = atoi(argv[0]);

	sem_t* print;
	if ((print = sem_open(SEMAPHORE_NAME, 0)) == SEM_FAILED) {
		return 1;
	}

    sem_t* write;
 	if ((write = sem_open(SEMAPHORE_NAME_2, 0)) == SEM_FAILED ) {
        return 1;
    }

	char outputRow[MESS_SIZE];
	
	int value;

	while (true) {
		sem_wait(print);
		memset(outputRow, '\0', MESS_SIZE);
		read(pipeRd, &outputRow, MESS_SIZE);
		for (int i = 0; i < strlen(outputRow); i++) {
			std::cout << outputRow[i] << std::endl;
			usleep(100000);
		}
		std::cout << std::endl;
		sem_post(write);

		sem_getvalue(closeParent, &value);
		if (value > 0) {
			close(pipeRd);
			break;
		}
	}
	return 0;
}