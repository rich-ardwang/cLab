// processB.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <iostream>
#include <Windows.h>

HANDLE H_Mutex = NULL;
HANDLE H_Event = NULL;

struct token_info {
    char       u_name[65];
    char       token[65];
};

int _tmain(int argc, _TCHAR* argv[])
{
    //create share file handle
    HANDLE shared_file = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,        //access mode: r/w
        FALSE,
        L"TokenNotify"              //share memory name
    );
    if (shared_file == NULL) {
        std::cout << "Could not open file mapping object..." << std::endl;
        return 1;
    }

    //map view of file and get the share memory pointer
    LPVOID lpBUF = MapViewOfFile(
        shared_file,            //file map handle
        FILE_MAP_ALL_ACCESS,    //access mode: r/w
        0,                      //file offset high 32 bit
        0,                      //file offset low 32 bit
        //map view size, 0:from offset to the end of file map.
        //because process don't know the size， set 0
        0
    );
    if (lpBUF == NULL) {
        std::cout << "Could not create file mapping object..." << std::endl;
        CloseHandle(shared_file);
        return 1;
    }

    //open the created mutex
    H_Mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"sm_mutex");
    if (H_Mutex == NULL) {
        std::cout << "open mutex failed..." << std::endl;
        return 1;
    }

    //open the created event handle
    H_Event = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"sm_event");
    if (H_Event == NULL) {
        std::cout << "open mutex failed..." << std::endl;
        return 1;
    }

    //operate share mmemory
    //char Buffer[65];
    while (true) {
        std::cout << "waiting receive data from process A." << std::endl;
        WaitForSingleObject(H_Event, INFINITE);
        WaitForSingleObject(H_Mutex, INFINITE); //acquire mutex lock
        //memcpy(Buffer, lpBUF, strlen((char *)lpBUF) + 1);
        token_info tk_tmp;
        memset(&tk_tmp, 0, sizeof(token_info));
        memcpy(&tk_tmp, lpBUF, sizeof(token_info));
        ReleaseMutex(H_Mutex);                  //unlock
        std::cout << "recv token: " << tk_tmp.token << std::endl;
        std::cout << "recv user name: " << tk_tmp.u_name << std::endl;
    }

    CloseHandle(H_Event);
    CloseHandle(H_Mutex);

    //release map view and close handle
    UnmapViewOfFile(lpBUF);
    CloseHandle(shared_file);

    std::cout << "enter to quit." << std::endl;
    std::cin.get();
	return 0;
}

