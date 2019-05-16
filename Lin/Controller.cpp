#include <iostream>
#include <vector>
#include <unistd.h>
#include <semaphore.h>
#include <termios.h>
#include <string>
#include <fcntl.h>
#include <signal.h>

std::vector<sem_t*> semInfo;
std::vector<pid_t> procInfo;
sem_t* parentSemClose;
const char* SEMAPHORE_NAME = "/my_sema";
const char* SEMAPHORE_NAME_2 = "/my_sema_2";
const int MESS_SIZE = 30;
pid_t parentPID;


void addParent(int pipeRd);
void addProcess(int processID, int pipeWr);
void removeProcess();
int getch();


int main(int argc, char* argv[])
{
	sem_t* print;
	if ((print = sem_open(SEMAPHORE_NAME, O_CREAT, 0777, 0)) == SEM_FAILED) {
		return 1;
	}

    sem_t* write;
 	if ((write = sem_open(SEMAPHORE_NAME_2, O_CREAT, 0777, 0)) == SEM_FAILED ) {
        return 1;
    }

    int pipeIndexes[2];
    pipe(pipeIndexes);
    int pipeRd = pipeIndexes[0];
    int pipeWr = pipeIndexes[1];

	int processID = 0;
	char choice;

	addParent(pipeRd);

	std::cout << "Enter + to add a person" << std::endl;
	std::cout << "Enter - to remove a person" << std::endl;
	std::cout << "Enter q to exit" << std::endl;

	while(true) {
		
		choice = getch();

		if (choice == '+') {
			if (procInfo.empty()) {
				sem_post(write);
			}
			addProcess(++processID, pipeWr);	
		}

		if (choice == '-')  {
			if (!procInfo.empty()) {
				removeProcess();
				processID--;
				if (procInfo.empty()) {
					sem_wait(print);
				}
			}
		}

		if (choice == 'q') {
			while(!procInfo.empty()) {
				removeProcess();
			}
			
			sem_post(parentSemClose);
			usleep(100);
			return 0;
		}
	}
}

void addParent(int pipeRd)
{
	pid_t pid = fork();
	parentPID = pid;

	const char* parentString = "Parent";
	if ((parentSemClose = sem_open(parentString, O_CREAT, 0777, 0)) == SEM_FAILED ) {
        return;
    }

	if (pid < 0) {
		std::cout << "!!! Parent wasn't created !!!" << std::endl;
	}

	if (pid == 0) {
		char pipeData[10];
		sprintf(pipeData, "%d", pipeRd);
		if (execlp("./parent", pipeData, NULL) == -1) {
			std::cout << "Error in creating of parent" << std::endl;	
		}	
	}
}

void addProcess(int processID, int pipeWr)
{
	sem_t* newSem;
	char semName[15];
	sprintf(semName,"%d",processID);
	if ((newSem = sem_open(semName, O_CREAT, 0777, 0)) == SEM_FAILED ) {
        return;
    }
    semInfo.push_back(newSem);
    
	procInfo.push_back(fork());

	if (procInfo.back() < 0) {
		std::cout << "!!! Error in creating of process!!!" << std::endl;
	}

	if (procInfo.back() == 0) {
		char id[10];
		char pipeData[10];
		sprintf(id, "%d", processID);
		sprintf(pipeData, "%d", pipeWr);
		if (execlp("./child", id, pipeData, NULL) == -1) {
			procInfo.pop_back();
			std::cout << "There is an error in creating of Child" << std::endl;
		}
	}
}

void removeProcess()
{
	sem_post(semInfo.back());
	semInfo.pop_back();
	procInfo.pop_back();
}

int getch() 
{
   struct termios oldattr, newattr;
   int ch;
   tcgetattr(STDIN_FILENO, &oldattr);
   newattr = oldattr;
   newattr.c_lflag &= ~(ICANON | ECHO);
   tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
   ch = getchar();
   tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
   return ch;
}