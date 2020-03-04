// win_share_memory_test.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <iostream>
#include <Windows.h>

#define MAP_VIEW_BUF_SIZE 130
HANDLE H_Mutex = NULL;
HANDLE H_Event = NULL;

struct token_info {
    char       u_name[65];
    char       token[65];
};

int _tmain(int argc, _TCHAR* argv[])
{
    //create share file handle
    HANDLE shared_file = CreateFileMapping(
        INVALID_HANDLE_VALUE,   //physical file handle
        NULL,                   //default security level
        PAGE_READWRITE,         //PAGE_READWRITE:r/w，PAGE_READONLY:r，PAGE_WRITECOPY:w
        0,                      //file size high bit
        MAP_VIEW_BUF_SIZE,      //file size low bit
        L"TokenNotify"          //share memory name
    );
    if (shared_file == NULL) {
        std::cout << "Could not create file mapping object..." << std::endl;
        return 1;
    }

    //map view of file and get the share memory pointer
    LPVOID lpBUF = MapViewOfFile(
        shared_file,            //file map handle
        FILE_MAP_ALL_ACCESS,    //access mode: r/w
        0,                      //file offset high 32 bit
        0,                      //file offset low 32 bit
        MAP_VIEW_BUF_SIZE       //map view size
    );
    if (lpBUF == NULL) {
        std::cout << "Could not create file mapping object..." << std::endl;
        CloseHandle(shared_file);
        return 1;
    }

    //create a mutex
    H_Mutex = CreateMutex(NULL, FALSE, L"sm_mutex");
    H_Event = CreateEvent(NULL, //security control
        //auto setting or manual setting，TRUE:manual，FALSE:auto.
        //if auto setting， it will call ResetEvent() after calling WaitForSingleObject()
        FALSE,
        FALSE,      //event ini status，TRUE:triggered
        L"sm_event" //event name. if no event name setting NULL
    );

    //operate share memory
    char Buffer[65];
    while (true) {
        token_info tk_info;
        memset(&tk_info, 0, sizeof(token_info));
        std::cout << "Please input the token to the process B:" << std::endl;
        std::cin.getline(Buffer, 65);
        memcpy(&tk_info.token, Buffer, strlen(Buffer));
        std::cout << "content: " << Buffer << std::endl;
        memset(&Buffer, 0, 65);
        std::cout << "Please input the user name to the process B:" << std::endl;
        std::cin.getline(Buffer, 65);
        memcpy(&tk_info.u_name, Buffer, strlen(Buffer));
        std::cout << "user name: " << Buffer << std::endl;
        memset(&Buffer, 0, 65);
        WaitForSingleObject(H_Mutex, INFINITE); //acquire mutex lock
        memcpy(lpBUF, &tk_info, sizeof(token_info));
        ReleaseMutex(H_Mutex);                  //unlock
        SetEvent(H_Event); //notify the waiting thread
    }

    CloseHandle(H_Mutex);
    CloseHandle(H_Event);

    //release map view and close handle
    UnmapViewOfFile(lpBUF);
    CloseHandle(shared_file);

    std::cout << "enter to quit." << std::endl;
    std::cin.get();
	return 0;
}

