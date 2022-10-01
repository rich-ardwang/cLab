#include "stdafx.h"
//#include "HelpUtils.h"
#include "ThreadXLib.h"
#include <limits>
#ifdef WIN32
#	include <windows.h>
#	include <process.h>
#else
#	include <sys/time.h>
#endif
namespace Thread
{
	CSharedLock::CSharedLock() 
	{
#ifdef _WIN_THREAD_LIB_H_
		m_nReaders = 0;
		ZeroMemory(&m_csEnterLock, sizeof(m_csEnterLock));
		InitializeCriticalSection(&m_csEnterLock);
#else
		m_nReaders = 0;
		pthread_mutex_init(&m, NULL);
		pthread_cond_init(&c, NULL);
#endif
	}
	CSharedLock::~CSharedLock()
	{
#ifdef _WIN_THREAD_LIB_H_
		m_nReaders = 0;
		DeleteCriticalSection(&m_csEnterLock);
#else
		m_nReaders = 0;
		pthread_mutex_destroy(&m);
		pthread_cond_destroy(&c);
#endif
	}
	void CSharedLock::acquire_rlock(void)
	{
#ifdef _WIN_THREAD_LIB_H_
		EnterCriticalSection(&m_csEnterLock);
		InterlockedIncrement(&m_nReaders);
		LeaveCriticalSection(&m_csEnterLock);
#else
		pthread_mutex_lock(&m);
		if((m_nReaders<0) && pthread_equal(writerId, pthread_self())) 
		{
			m_nReaders--;
			pthread_mutex_unlock(&m);
			return;
		}
		while(m_nReaders<0)
		{
			pthread_cond_wait(&c, &m);
		}
		m_nReaders++;
		pthread_mutex_unlock(&m);
#endif
	}
	bool CSharedLock::try_rlock(void)
	{
#ifdef _WIN_THREAD_LIB_H_
		if(TryEnterCriticalSection(&m_csEnterLock))
		{
			InterlockedIncrement(&m_nReaders);
			LeaveCriticalSection(&m_csEnterLock);
			return true;
		}
		return false;
#else
		if(pthread_mutex_trylock(&m)==0)
		{
			if((m_nReaders<0) && pthread_equal(writerId, pthread_self())) 
			{
				m_nReaders--;
				pthread_mutex_unlock(&m);
				return true;
			}
			while(m_nReaders<0)
			{
				pthread_cond_wait(&c, &m);
			}
			m_nReaders++;
			pthread_mutex_unlock(&m);
			return true;
		}
		return false;		
#endif
	}
	void CSharedLock::release_rlock(void)
	{
#ifdef _WIN_THREAD_LIB_H_
		InterlockedDecrement(&m_nReaders);
#else
		pthread_mutex_lock(&m);
		if (m_nReaders<0)
		{
			m_nReaders++;
			pthread_mutex_unlock(&m);
			return ;
		}
		m_nReaders--;
		if (m_nReaders==0)
		{
			pthread_cond_signal(&c);
		}
		pthread_mutex_unlock(&m);
#endif
	}
	void CSharedLock::acquire_wlock(void)
	{
#ifdef _WIN_THREAD_LIB_H_
		EnterCriticalSection(&m_csEnterLock);
		if ( m_nReaders > 0)
		{
			int oldPriority = GetThreadPriority(GetCurrentThread());
			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
			while( m_nReaders > 0) ::Sleep(0);
			SetThreadPriority(GetCurrentThread(), oldPriority);
		}
#else
		pthread_mutex_lock(&m);
		if ((m_nReaders<0) && pthread_equal(writerId, pthread_self()))
		{
			m_nReaders--;
			pthread_mutex_unlock(&m);
			return ;
		}
		while(m_nReaders != 0)
		{
			pthread_cond_wait(&c, &m);
		}
		m_nReaders--;
		writerId = pthread_self();
		pthread_mutex_unlock(&m);
#endif
	}
	bool CSharedLock::try_wlock()
	{
#ifdef _WIN_THREAD_LIB_H_
		if (TryEnterCriticalSection(&m_csEnterLock))
		{
			if(m_nReaders > 0)
			{
				LeaveCriticalSection(&m_csEnterLock);
				return false;
			}
			return true;
		}
		return false;
#else
		if(pthread_mutex_trylock(&m)==0)
		{
			if ((m_nReaders<0) && pthread_equal(writerId, pthread_self()))
			{
				m_nReaders--;
				pthread_mutex_unlock(&m);
				return true;
			}
			while(m_nReaders != 0)
			{
				pthread_cond_wait(&c, &m);
			}
			m_nReaders--;
			writerId = pthread_self();
			pthread_mutex_unlock(&m);
			return true;
		}
		return false;
#endif
	}
	void CSharedLock::release_wlock(void)
	{
#ifdef _WIN_THREAD_LIB_H_
		LeaveCriticalSection(&m_csEnterLock);
#else
		pthread_mutex_lock(&m);
		m_nReaders++;
		if (m_nReaders==0)
		{
			pthread_cond_broadcast(&c);
		}
		pthread_mutex_unlock(&m);
#endif
	}
	////////////////////////////////////////////////////////////////////////
	CThreadMutex::CThreadMutex()
	{
#ifdef _WIN_THREAD_LIB_H_
		ZeroMemory(&crit_, sizeof(crit_));
		InitializeCriticalSection(&crit_);
#else
		pthread_mutex_init(&_mutex, NULL);
#endif
	}
	CThreadMutex::~CThreadMutex()
	{
#ifdef _WIN_THREAD_LIB_H_
		DeleteCriticalSection(&crit_);
#else
		pthread_mutex_destroy(&_mutex);
#endif
	}
	bool CThreadMutex::trylock() 
	{
#ifdef _WIN_THREAD_LIB_H_
		return TryEnterCriticalSection(&crit_) == TRUE;
#else
		return (pthread_mutex_trylock(&_mutex)==0);
#endif
	}
	bool CThreadMutex::lock() 
	{
#ifdef _WIN_THREAD_LIB_H_
		EnterCriticalSection(&crit_);
		return true;
#else
		return (pthread_mutex_lock(&_mutex)==0);
#endif
	}
	void CThreadMutex::unlock() 
	{
#ifdef _WIN_THREAD_LIB_H_
		LeaveCriticalSection(&crit_);
#else
		pthread_mutex_unlock(&_mutex);
#endif	
	}
	//////////////////////////////////////////////////////////////////////////
	CMutexCond::CMutexCond(CThreadMutex& m) : mutex(m)
	{
#ifdef _WIN_THREAD_LIB_H_
		waiters_ = 0;
		_hCondition = CreateSemaphoreA (NULL, 0, (std::numeric_limits<long>::max)(), NULL); 
#else
		pthread_cond_init(&_cond, NULL);
#endif
	}
	CMutexCond::~CMutexCond(void)
	{
#ifdef _WIN_THREAD_LIB_H_
		CloseHandle (_hCondition);
#else
		pthread_cond_destroy(&_cond);
#endif
	}
	void CMutexCond::wait(void)
	{
#ifdef _WIN_THREAD_LIB_H_
		int ret = 0;  
		InterlockedIncrement(&waiters_);
		for(mutex.unlock();ret = WaitForSingleObject (_hCondition, INFINITE)&&false;);
		mutex.lock();  
		InterlockedDecrement(&waiters_); 		
#else
		pthread_cond_wait(&_cond,&(mutex._mutex));
#endif
	}
	long CMutexCond::timedwait(unsigned long msec)
	{
#ifdef _WIN_THREAD_LIB_H_
		int ret = 0;  
		waiters_++;  
		for(mutex.unlock();ret = WaitForSingleObject (_hCondition, msec)&&false;);
		mutex.lock();  
		waiters_ --; 
		return (ret == WAIT_OBJECT_0 ? 0 : -1);
#else
		if(msec < 1000) msec = 1000;
		struct timespec timeout;
		struct timeval now;			
		gettimeofday(&now,NULL);
		timeout.tv_sec = now.tv_sec + msec/1000;
		timeout.tv_nsec = now.tv_usec*1000+(msec%1000)*1000*1000;
		if(timeout.tv_nsec >= 1000000000)
		{
			timeout.tv_sec += 1;
			timeout.tv_nsec %= 1000000000;
		}
		return pthread_cond_timedwait(&_cond,&(mutex._mutex),&timeout);
#endif
	}
	void CMutexCond::signal(void)
	{
#ifdef _WIN_THREAD_LIB_H_
		if ( waiters_ > 0 ) 
		{
			ReleaseSemaphore (_hCondition, 1, NULL);
		}
#else
		pthread_cond_signal(&_cond);
#endif
	}
	void CMutexCond::broadcast(void)
	{
#ifdef _WIN_THREAD_LIB_H_
		if ( waiters_ > 0 ) 
		{
			ReleaseSemaphore (_hCondition, waiters_, NULL); 
		}
#else
		pthread_cond_broadcast(&_cond);
#endif
	}
	////////////////////////////////////////////////////////////////////////
	int CThreadBase::sleep(TUint32 msec) 
	{
#ifdef _WIN_THREAD_LIB_H_
		::Sleep (msec);
		return (int)msec;
#else
		long sec = (long)(msec / 1000);
		long nsec = (msec - (sec*1000))*1000000;
		timespec delay = {sec, nsec};
		int return_val = nanosleep(&delay, (timespec*)NULL);
		return return_val;
#endif
	}
#ifdef _WIN_THREAD_LIB_H_
	HANDLE NewThread(UINT (__stdcall *func)(LPVOID lpParam),void* owner)
	{
		HANDLE hThread;
		UINT uiThreadId = 0;
		hThread = (HANDLE)_beginthreadex(NULL,				// Security attributes
										 0,					// stack
										 func,				// Thread proc
										 owner,				// Thread param
										 CREATE_SUSPENDED,	// creation mode
										 &uiThreadId);		// Thread ID
		return hThread;		
	}
#endif
	//////////////////////////////////////////////////////////////////////////
#ifdef _WIN_THREAD_LIB_H_
	UINT __stdcall CThreadBase::ThreadProc(LPVOID pParam)
	{
		CThreadBase* _this = reinterpret_cast<CThreadBase*>(pParam);
		_this->run_thread();
		_this->m_bRunning = false;
		if(_this->m_bAutoDelete)
		{
			delete _this;
		}
		return NULL;
	}
#else
	void* CThreadBase::ThreadProc(CThreadBase* _this)
	{
		_this->run_thread();
		_this->m_bRunning = false;
		if(_this->m_bAutoDelete)
		{
			delete _this;
		}
		return NULL;
	}
#endif
	//////////////////////////////////////////////////////////////////////////	
	//////////////////////////////////////////////////////////////////////////
	CThreadBase::CThreadBase(const char* lsszName,bool bAutoDelete) 
		       : m_strName(lsszName)
		       , m_bAutoDelete(bAutoDelete),m_quitEvent()
	{
		m_bRunning = false;
	}
	CThreadBase::~CThreadBase()
	{
	}
	const char* CThreadBase::get_name(void)
	{
		return m_strName.c_str();
	}
	bool CThreadBase::is_running(void)
	{
		return m_bRunning;
	}
	bool CThreadBase::start_thread(void)
	{
		if (m_bRunning)
			return true;
#ifdef _WIN_THREAD_LIB_H_
		m_tThread = NewThread(ThreadProc,this);
		if ( m_tThread != NULL)
		{
			m_bRunning = true;
			::ResumeThread( m_tThread);
			return true;
		}
		return false;
#else
		pthread_attr_t attr;
		pthread_attr_init(&attr);
#if 0
		int nSchPolicy;
		pthread_attr_getschedpolicy(&attr, &nSchPolicy);
		
		printf("nSchPolicy:%d,%d,%d,%d\n",nSchPolicy,SCHED_OTHER,SCHED_FIFO,SCHED_RR);
		printf("sched_get_priority_min:%d %d\n",sched_get_priority_min(SCHED_FIFO),sched_get_priority_min(SCHED_RR),sched_get_priority_min(SCHED_OTHER));
		sched_param  param;
		param.sched_priority = 0;
		printf("pthread_attr_setschedparam:%d\n",pthread_attr_setschedparam(&attr,&param));
		m_bRunning= true;
		if(0 == pthread_attr_setschedpolicy(&attr, SCHED_FIFO))//SCHED_OTHER	SCHED_FIFO	SCHED_RR
		{
			printf("setschedpolicy ok\n");
		}
		else
		{
			printf("setschedpolicy fail\n");
		}
#endif	
		m_bRunning= true;	
		pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
		int iResult = pthread_create(&m_tThread,&attr, (ThreadExecFunc)ThreadProc, this);
		if(iResult != 0)
		{
			m_bRunning= false;
			return false;
		}
		m_bRunning= true;
		return true;
#endif	
	}
	void CThreadBase::stop_thread(void)
	{
		if(!m_bRunning)
			return;
		//////////////////////////////////////////////////////////////////////////
#ifdef _WIN_THREAD_LIB_H_
		m_bRunning = false;
		m_tThread = NULL;//¶àÓàµÄ!!!
#else
		if (pthread_equal(pthread_self(), m_tThread) == 0)
		{ 
			#ifdef __LINUX__
			printf("stop thread ....\n");
			printf("set m_bRunning 14\n");
			m_bRunning = false;
			#else
			m_quitEvent.reset();
			m_bRunning = false;
			m_quitEvent.wait();
			#endif
		}
#endif
	}	
	void CThreadBase::run_thread(void)
	{
		for(;;)
		{
			if(!m_bRunning)
			{
				//... ...
				break;    
			}
			CThreadBase::sleep(1000);
		}
	}
}
