/*
* =====================================================================================
*
*       Filename:  thread.h
*    Description:  基本线程封装（丁富来：2014-4-30）
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

#ifndef _WIND_THREAD_H_
#define _WIND_THREAD_H_

#ifdef _WIN32
#include <windows.h>
#elif defined __linux__
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#else
#error "Unknown os platform, _WIN32 or __linux__ not defined."
#endif

#ifdef _WIN32
#define THREADID DWORD
#else
#define THREADID pthread_t
#endif

#ifdef _WIN32
typedef DWORD (__stdcall *ThreadEntryFunc) (LPVOID lpParameter);
#else
typedef void* (*ThreadEntryFunc)(void *lpParameter);
#endif

namespace windcom
{
    #define THREAD_STATE_STOPPED 0 // 线程已经停止运行
    #define THREAD_STATE_EXITING 1 // 线程正在停止
    #define THREAD_STATE_RUNNING 2 // 线程正在运行

    class Thread
    {
    public:
        Thread();
        virtual ~Thread();

    private:
        // 禁止拷贝和赋值
        Thread(const Thread& rhs);
        Thread& operator=(const Thread& rhs);

    protected:
        // 线程运行函数，成功返回0，失败返回-1
        virtual int Run() = 0;
        // 线程初始化，成功返回0，失败返回-1
        virtual int Init() { return 0; };
        
    public:
        // 静态函数，创建线程，成功返回0，失败返回错误码
        static int CreateThreadImp(ThreadEntryFunc ThreadEntry, void * param);
#ifdef _WIN32
        static DWORD GetCurrentThreadId() { return (::GetCurrentThreadId()); }
#else
        static pid_t GetCurrentThreadId() { return (syscall(SYS_gettid)); }
#endif

    public:
        // 对外接口
        // 启动线程，成功返回0，失败返回-1
        int Start();
        // 停止线程
        void Stop() { if(THREAD_STATE_RUNNING == State) State = THREAD_STATE_EXITING; }
        bool Stopped() const { return State == THREAD_STATE_STOPPED; }
        bool Exiting() const { return State == THREAD_STATE_EXITING; }
        
        THREADID GetThreadId() const { return ThreadId; }
        int GetPid() const { return Pid; }
        int GetExitCode() const { return RetCode; }
#ifdef _WIN32
        DWORD GetErrCode() const
#else
        int GetErrCode() const
#endif
        { return ErrCode; }
        
    private:
        void Clear();
#ifdef _WIN32
        static DWORD __stdcall ThreadEntry(LPVOID Para);
#else
        static void* ThreadEntry(void* Para);
#endif

    private:
        int State;
        int Pid;
        THREADID ThreadId;
        int RetCode;
#ifdef _WIN32
        HANDLE Handle;
        DWORD ErrCode;
#else
        int ErrCode;
#endif
        // 避免线程对象过小，导致多个线程对象有可能存在同一个Cache行内，例如线程对象数组
        char CacheBuf[1024];
    };
    
}

#endif

