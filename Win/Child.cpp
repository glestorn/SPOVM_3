#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <time.h>
#include <string.h>

#define ever ;;

int main(int argc, char* argv[])
{
	char processID[10];
	sprintf_s(processID, argv[1]);
	HANDLE close = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, processID);
	HANDLE print = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Print"); 
	HANDLE write = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Write"); 

	HANDLE conn = OpenFileMapping(FILE_MAP_ALL_ACCESS,
								  FALSE,
								  "FileMap");

	LPVOID buff = MapViewOfFile(conn,
								FILE_MAP_ALL_ACCESS,
								0,
								0,
								40);

	char emptyRow[40];
	memset(emptyRow, '\0', 40);
	
	char outputRow[40];
	sprintf_s(outputRow, "Process %d", atoi(argv[1]));

	for(ever) {

		WaitForSingleObject(write, INFINITE);
		
		if (WaitForSingleObject(close, 0) == WAIT_OBJECT_0) {
			ReleaseSemaphore(write, 1, NULL);
			break;
		}
		// if (WaitForSingleObject(write, 1) == WAIT_OBJECT_0) {
			CopyMemory((PVOID)buff, emptyRow, 40);
			CopyMemory((PVOID)buff, outputRow, 40);
			ReleaseSemaphore(print, 1, NULL);
		// }		
	}

	CloseHandle(close);
	CloseHandle(write);
	CloseHandle(print);
	UnmapViewOfFile(buff);
	CloseHandle(conn);

	return 0;
}

