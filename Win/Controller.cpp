#include <iostream>
#include <windows.h>
#include <vector>
#include <stdlib.h>
#include <conio.h>

std::vector<HANDLE> semInfo;
std::vector<PROCESS_INFORMATION> procInfo;
PROCESS_INFORMATION parentInfo;
HANDLE closeParent = CreateSemaphore(NULL, 0, 1, "closeParent");

PROCESS_INFORMATION addParent();
PROCESS_INFORMATION addProcess(int processID);
void removeProcess();

int main(int argc, char* argv[])
{
	int processID = 0;
	char choice;

	parentInfo = addParent();

	std::cout << "Enter + to add a person" << std::endl;
	std::cout << "Enter - to remove a person" << std::endl;
	std::cout << "Enter q to exit" << std::endl;

	while(TRUE) {
		if (_kbhit()) {

			choice = _getch();

			if (choice == '+') {
				procInfo.push_back(addProcess(++processID));
			}

			if (choice == '-')  {
				if (!procInfo.empty()) {
					removeProcess();
					processID--;
				}
			}

			if (choice == 'q') {
				while(!procInfo.empty()) {
					removeProcess();
				}

				ReleaseSemaphore(closeParent, 1, NULL);
				CloseHandle(closeParent);

				WaitForSingleObject(parentInfo.hProcess, INFINITE);

				CloseHandle(parentInfo.hProcess);
				CloseHandle(parentInfo.hThread);
				return 0;
			}
		}
	}
}

PROCESS_INFORMATION addParent()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(STARTUPINFO);
	ZeroMemory(&pi, sizeof(pi));

	char cmdLine[30];
	sprintf_s(cmdLine, "Parent.exe");

	if(!CreateProcess(NULL,
				  	  cmdLine,
				 	  NULL,
				  	  NULL,
				  	  FALSE,
				  	  0,
				  	  NULL,
				  	  NULL,
				  	  &si,
				  	  &pi)) {
		std::cout << "There is an error with creating of parent proccess" << std::endl;	
	}

	return pi;
}

PROCESS_INFORMATION addProcess(int processID)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof si;
	ZeroMemory(&pi, sizeof(pi));

	char newProcessString[10];
	sprintf_s(newProcessString, "%d", processID);

	char cmdLine[30];
	sprintf_s(cmdLine, "Child.exe %s", newProcessString);

	semInfo.push_back(CreateSemaphore(NULL, 0, 1, newProcessString));

	if(!CreateProcess(NULL,
				  	  cmdLine,
				 	  NULL,
				  	  NULL,
				  	  FALSE,
				  	  0,
				  	  NULL,
				  	  NULL,
				  	  &si,
				  	  &pi)) {
		std::cout << "There is an error with creating of child proccess" << std::endl;	
	}

	return pi;
}

void removeProcess()
{
		ReleaseSemaphore(semInfo.back(), 1, NULL);

		CloseHandle(semInfo.back());
		semInfo.pop_back();

		CloseHandle(procInfo.back().hProcess);
		CloseHandle(procInfo.back().hThread);
		procInfo.pop_back();
}