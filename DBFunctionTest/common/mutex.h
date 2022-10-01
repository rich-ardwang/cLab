/*
 * =====================================================================================
 *
 *       Filename:  mutex.h
 *    Description:  互斥量mutex封装（丁富来：2014-4-30）
 *        Version:  1.0
 *        Created:  2014-4-30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  dingfulai
 *        Company:  wind, Inc.
 *
 *        History:  chenwanming  created @2012-7-10
 * =====================================================================================
 */

#ifndef  _WIND_MUTEX_H_
#define _WIND_MUTEX_H_

#ifdef _WIN32
#include <windows.h>
#elif defined __linux__
#include <pthread.h>
#else
#error "Unknown os platform, _WIN32 or __linux__ not defined."
#endif

namespace windcom
{
    class Mutex
    {
    public:
        Mutex();
        ~Mutex();

    private:
        Mutex(const Mutex&);
        Mutex& operator=(const Mutex&);

    public:
        void Lock();
        bool TryLock();
        void UnLock();

    private:
#ifdef _WIN32
        CRITICAL_SECTION _Mutex;
#elif defined( __linux__ )
        pthread_mutex_t  _Mutex;
#endif
    };

    class MutexLock
    {
    public:
        MutexLock(Mutex* pMutex):_pMutex(pMutex)
        {
            _pMutex->Lock();
        }

        MutexLock(Mutex& mutex):_pMutex(&mutex)
        {
            _pMutex->Lock();
        }

        ~MutexLock()
        {
            _pMutex->UnLock();
        }

    private:
        MutexLock(const MutexLock&);
        MutexLock& operator=(const MutexLock&);

    private:
        Mutex *_pMutex;
    };
}

#endif

