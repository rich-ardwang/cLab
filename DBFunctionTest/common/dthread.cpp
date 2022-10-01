/*
 * =====================================================================================
 *
 *       Filename:  thread.cpp
 *    Description:  基本线程实现（丁富来：2014-4-30）
 *        Version:  1.0
 *        Created:  2014-4-30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  dingfulai
 *        Company:  wind, Inc.
 *
 * =====================================================================================
 */

#include "dthread.h"

namespace windcom
{
    Thread::Thread():State(THREAD_STATE_STOPPED),Pid(0),ThreadId(0),RetCode(0),ErrCode(0)
    {
#ifdef _WIN32
        Handle = NULL;
#endif
    }

    Thread::~Thread()
    {
#ifdef _WIN32
        if(Handle) CloseHandle(Handle);
#endif
    }

    int Thread::Start()
    {
        // 调用前先清理资源
        Clear();

        // 初始化失败
        if(0 != Init())
            return -1;
        
        if(State == THREAD_STATE_RUNNING)
            return 0;
        
        if(State == THREAD_STATE_EXITING)
            return -1;

        State = THREAD_STATE_RUNNING;
        
#ifdef _WIN32
        Handle = CreateThread(NULL, NULL, ThreadEntry, this, 0, &ThreadId);
        if(Handle == NULL)
        {
            ErrCode = GetLastError();
            State = THREAD_STATE_STOPPED;
            return -1;
        }
#else
        ErrCode = pthread_create(&ThreadId, NULL, ThreadEntry, (void*)this);
        if(ErrCode != 0)
        {
            State = THREAD_STATE_STOPPED;
            return -1;
        }
#endif
        return 0;
    }

    void Thread::Clear()
    {
        if(State != THREAD_STATE_STOPPED)
            return;

        ErrCode = 0;
        ThreadId = 0;
        Pid = 0;
#ifdef _WIN32
        if(Handle)
        {
            CloseHandle(Handle);
            Handle = NULL;
        }
#endif
    }

    
#ifdef _WIN32
    DWORD __stdcall Thread::ThreadEntry(LPVOID Para)
#else
    void* Thread::ThreadEntry(void* Para)
#endif
    {
        Thread* This = (Thread*)Para;
#ifdef _WIN32
        This->Pid = GetCurrentProcessId();
#else
        This->Pid = getpid();
#endif
        This->RetCode = This->Run();
        //This->Clear();

        This->State = THREAD_STATE_STOPPED;

#ifdef _WIN32
        return (DWORD)This->RetCode;
#else
        return (void*)This->RetCode;
#endif
    }


    int Thread::CreateThreadImp(ThreadEntryFunc ThreadEntry, void * param)
    {
        THREADID ThreadId;
#ifdef _WIN32
        if(NULL == CreateThread(NULL, NULL, ThreadEntry, param, 0, &ThreadId))
            return GetLastError();
        return 0;
#else
        return pthread_create(&ThreadId, NULL, ThreadEntry, (void*)param);  
#endif
    }
}
