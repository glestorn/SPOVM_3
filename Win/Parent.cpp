#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <vector>
#include <conio.h>
#define ever ;;

using namespace std;

int main(int argc, char* argv[])
{
	char string[40];
	char choice;

	HANDLE closeParent = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "closeParent");
	HANDLE print = CreateSemaphore(NULL, 0, 1, "Print");
	HANDLE write = CreateSemaphore(NULL, 0, 1, "Write");
	HANDLE conn = CreateFileMapping(INVALID_HANDLE_VALUE,
							 NULL,
							 PAGE_READWRITE,
							 0,
							 40,
							 "FileMap");
	LPVOID buff = MapViewOfFile(conn,
						 FILE_MAP_ALL_ACCESS,
						 0,
						 0,
						 40);

	for(ever) {
		
		ReleaseSemaphore(write, 1, NULL);
		WaitForSingleObject(print, INFINITE);

		memset(string, '\0', 40);
		strncpy(string, (char*)buff, 40);

		cout << endl;
		for (unsigned int i = 0; i < strlen(string); i++) {
			cout << string[i];
			Sleep(100);
		}

		if (WaitForSingleObject(closeParent, 0) == WAIT_OBJECT_0) {
			break;
		}
	}

	CloseHandle(closeParent);
	CloseHandle(print);
	CloseHandle(write);
	UnmapViewOfFile(buff);
	CloseHandle(conn);

	return 0;
}

// void Parent::addProcess(int nextProcessID) 
// {
// 	char newProcessString[10];

// 	sprintf_s(newProcessString, "%d", nextProcessID);
// 	procInfo.push_back(createProcess(newProcessString));

// 	semInfo.push_back(CreateSemaphore(NULL, 0, 1, newProcessString));
// }

// void Parent::removeProcess()
// {
// 	ReleaseSemaphore(semInfo.back(), 1, NULL);

// 	CloseHandle(semInfo.back());
// 	semInfo.pop_back();

// 	CloseHandle(procInfo.back().hProcess);
// 	CloseHandle(procInfo.back().hThread);
// 	procInfo.pop_back();
// }

// PROCESS_INFORMATION Parent::createProcess(char* nextEventID) 
// {
// 	STARTUPINFO si;
// 	PROCESS_INFORMATION pi;

// 	ZeroMemory(&si, sizeof(si));
// 	si.cb = sizeof si;
// 	ZeroMemory(&pi, sizeof(pi));

// 	char cmdLine[30];
// 	sprintf_s(cmdLine, "SPOVM_3 %s", nextEventID);

// 	if(!CreateProcess(NULL,
// 				  	  cmdLine,
// 				 	  NULL,
// 				  	  NULL,
// 				  	  FALSE,
// 				  	  0,
// 				  	  NULL,
// 				  	  NULL,
// 				  	  &si,
// 				  	  &pi)) {
// 		std::cout << "There is an error with creating of proccess" << std::endl;	
// 	}

// 	return pi;
// }