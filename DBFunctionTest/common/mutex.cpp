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

#include "mutex.h"

namespace windcom
{

    Mutex::Mutex()
    {
#ifdef _WIN32
        InitializeCriticalSection(&_Mutex);
#elif defined( __linux__ )
        pthread_mutex_init(&_Mutex, NULL);
#endif
    }

    Mutex::~Mutex()
    {
#ifdef _WIN32
        DeleteCriticalSection(&_Mutex);
#elif defined( __linux__ )
        pthread_mutex_destroy(&_Mutex);
#endif
    }

    void Mutex::Lock()
    {
#ifdef _WIN32
        EnterCriticalSection(&_Mutex);
#elif defined( __linux__ )
        pthread_mutex_lock(&_Mutex);
#endif
    }

    bool Mutex::TryLock()
    {
#ifdef _WIN32
        return (0 != TryEnterCriticalSection(&_Mutex));
#else
        return (0 == pthread_mutex_trylock(&_Mutex));
#endif
    }

    void Mutex::UnLock()
    {
#ifdef _WIN32
        LeaveCriticalSection(&_Mutex);
#elif defined( __linux__ )
        pthread_mutex_unlock(&_Mutex);
#endif
    }

}

