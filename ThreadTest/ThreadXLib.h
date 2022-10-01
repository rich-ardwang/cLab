#pragma once
//////////////////////////////////////////////////////////////////////////
#include "TypeUtils.h"
//////////////////////////////////////////////////////////////////////////
#include <string>
#ifdef _WIN_THREAD_LIB_H_
#	include <Windows.h>
#else
#	include "pthread.h"
#	include "sched.h"
#	include "semaphore.h"
#endif
//////////////////////////////////////////////////////////////////////////
#define LOCK_GUARD(LOCK)  Thread::CLockGuard      __guard_(LOCK)
#define READ_GUARD(LOCK)  Thread::ReaderLockGuard r_guard_(LOCK)
#define WRITE_GUARD(LOCK) Thread::WriterLockGuard w_guard_(LOCK)
//////////////////////////////////////////////////////////////////////////
namespace Thread
{
	using namespace stdtype;
	//////////////////////////////////////////////////////////////////////////
	class CThreadMutex 
	{
	public:
		CThreadMutex();
		~CThreadMutex();

		friend class CMutexCond;
	public:
		bool trylock(void);
		bool lock(void);
		void unlock(void);

	private:
#	ifdef _WIN_THREAD_LIB_H_
		CRITICAL_SECTION crit_;
#	else
		pthread_mutex_t _mutex;
#	endif
	};
	class CLockGuard
	{
		CThreadMutex& mutex;
	public:
		inline CLockGuard(CThreadMutex& m) : mutex(m) { mutex.lock(); }
		inline ~CLockGuard(void) { mutex.unlock(); }
	private:
		CLockGuard(const CLockGuard&);
		CLockGuard& operator=(const CLockGuard&);
	};
	//////////////////////////////////////////////////////////////////////////
	class CMutexCond
	{
		CThreadMutex& mutex;
	public:
		CMutexCond(CThreadMutex& m);
		~CMutexCond(void);

		void wait(void);	
		long timedwait(unsigned long msec);
		void signal(void);
		void broadcast(void);

	private:
		CMutexCond(const CMutexCond&);
		CMutexCond& operator=(const CMutexCond&);

	private:
#ifdef _WIN_THREAD_LIB_H_
		volatile long waiters_;
		HANDLE _hCondition;  
#else
		pthread_cond_t _cond;
#endif
	};
	//////////////////////////////////////////////////////////////////////////  	
	//////////////////////////////////////////////////////////////////////////
	class CSharedLock 
	{
	public:
		CSharedLock();
		virtual ~CSharedLock();

	public:
		void acquire_rlock(void);
		bool try_rlock(void);
		void release_rlock(void);

		void acquire_wlock(void);
		bool try_wlock(void);
		void release_wlock(void);

	private:
#ifdef _WIN_THREAD_LIB_H_
		volatile long    m_nReaders;
		CRITICAL_SECTION m_csEnterLock;
#else
		pthread_mutex_t m;
		pthread_cond_t c;
		pthread_t writerId;
		volatile long m_nReaders;
#endif
	};
	//////////////////////////////////////////////////////////////////////////
	class ReaderLockGuard
	{
	public:
		ReaderLockGuard(CSharedLock& lock_) : rwlock(lock_) { rwlock.acquire_rlock(); }
		~ReaderLockGuard(void) { rwlock.release_rlock(); }

	private:
		CSharedLock& rwlock;
	};
	class WriterLockGuard
	{
	public:
		WriterLockGuard(CSharedLock& l) : rwlock(l) { rwlock.acquire_wlock(); }
		~WriterLockGuard(void) { rwlock.release_wlock(); }
	private:
		CSharedLock& rwlock;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef void* (*ThreadExecFunc)(void*);
	//////////////////////////////////////////////////////////////////////////
	class CThreadBase
	{
	public:
		CThreadBase(const char* lsszName,bool bAutoDelete = false);
		virtual ~CThreadBase();
		//////////////////////////////////////////////////////////////////////////
		class CExitEvent
		{
		public:
			CExitEvent() : m_bQuitFlag(false) { }
			~CExitEvent() { }

		public:
			void reset(void) { m_bQuitFlag = false; }		
			void signal(void){ m_bQuitFlag = true;  }			
			void wait() { while(!m_bQuitFlag) { CThreadBase::sleep(10); } }
		protected:
			volatile bool m_bQuitFlag;
		};
		//////////////////////////////////////////////////////////////////////////
#ifdef _WIN_THREAD_LIB_H_
		static UINT __stdcall ThreadProc(LPVOID pParam);
#else
		static void* ThreadProc(CThreadBase* _this);
#endif
		static int sleep(TUint32 msec);
		//////////////////////////////////////////////////////////////////////////		
	public:
		const char* get_name(void);
		bool is_running(void);

		virtual bool start_thread(void);
		virtual void stop_thread(void);

	protected:
		virtual void run_thread(void);

	protected: 
		bool          m_bAutoDelete;		
		volatile bool m_bRunning;
		CExitEvent    m_quitEvent;
#ifdef _WIN_THREAD_LIB_H_
		HANDLE        m_tThread;
#else
		pthread_t     m_tThread;
#endif
	private:
		std::string   m_strName;
	};
}
//////////////////////////////////////////////////////////////////////////
typedef Thread::CLockGuard   IILockGuard;
typedef Thread::CThreadMutex IIThreadMutex;
typedef Thread::CMutexCond   IIMutexCond;
typedef Thread::CThreadBase  IIThreadBase;
typedef Thread::CSharedLock  IISharedLock;
//////////////////////////////////////////////////////////////////////////
