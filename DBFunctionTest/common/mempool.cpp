/* =====================================================================================
*
*       Filename:  mempool.cpp
*
*    Description:
*	这是一个简单的内存池,用于解决linux下,频繁申请和释放大块内存时系统不能及时回收,server长时间运行会产生回收不了的内存碎片的问题
*
*        Version:  1.0
*        Created:  2012-7-10
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  chenwanming
*		  Modify:  bianjun 集成进common库，沿用Common库的命名规则（2014-10-30）
*        Company:  wind, Inc.
*
* =====================================================================================
*/

#include <algorithm>
#include "common/mempool.h"
#include "logInterface/windlog.h"

#ifndef _WIN32
#include <string.h>
#endif

namespace windcom
{
	CMemMgr *CMemMgr::m_myMemMgr = NULL;

	CMemPool::CMemPool()
	{
		m_pMemCache = NULL;
		m_CacheLevel = 0;
	}

	CMemPool::~CMemPool()
	{
		if (m_pMemCache != NULL)
			delete [] m_pMemCache;
	}

	int CMemPool::Init(int nPieceSize, int nMaxLen, int nLevel)
	{
		m_pMemCache = new char[nMaxLen * 1024];
		memset(m_pMemCache, 0, nMaxLen * 1024);
		if (m_pMemCache == NULL)
		{
			//LOG4CXX_ERROR(GETLOGGER("Init"), "Init malloc memory too large:  " << nMaxLen);
			return -1;
		}
		if (nMaxLen < 0 || nPieceSize < 0 || nLevel < 0 || nLevel > MEMPOOLNUM)
		{
			//LOG4CXX_ERROR(GETLOGGER("Init"), "nMaxlen error,nBlockSzie error! ");
			return -1;
		}
		m_nPieceSize = nPieceSize - sizeof(m_CacheLevel);
		m_nTotalSize = nMaxLen;
		m_CacheLevel = nLevel;
		long long nBlockNum = nMaxLen * 1024 / nPieceSize;
		for (long long ii = 0; ii < nBlockNum; ii++)
		{
			char *pbuf = m_pMemCache + ii * nPieceSize;
			memcpy(pbuf, &m_CacheLevel, sizeof(m_CacheLevel));
			char *pRealBuf = pbuf + sizeof(m_CacheLevel);
			m_freeList.push(pRealBuf);
		}
		return 1;
	}

	char* CMemPool::GetMem(int nLen)
	{
		if (nLen != m_nPieceSize)
		{
			//LOG4CXX_DEBUG(GETLOGGER("GetMem"), "len is not right:  " << nLen);
			return NULL;
		}
		//LOG4CXX_DEBUG(GETLOGGER("GetMem"),"len: " << nLen);
		char *p = NULL;
		//MutexLock lock(&m_mutex);
		if (!m_freeList.empty())
		{
			p = m_freeList.front();
			m_freeList.pop();
			m_useList.insert(p);
			//LOG4CXX_DEBUG(GETLOGGER("GetMem"), "Pool freeList size: " << m_freeList.size() << " level: " <<	(int)m_CacheLevel);
		}
		else
		{
			//LOG4CXX_DEBUG(GETLOGGER("GetMem"), "Pool freeList is empty. len: " << nLen << " level: " <<	(int)m_CacheLevel);
		}
		return p;
	}

	int CMemPool::FreeMem(char *pChar)
	{
		if (!pChar)
		{
			return -1;
		}
		//MutexLock lock(&m_mutex);
		set<char *>::iterator setItr = m_useList.find(pChar);
		if (setItr != m_useList.end())
		{
			m_freeList.push(pChar);
			m_useList.erase(setItr);
			//LOG4CXX_DEBUG(GETLOGGER("FreeMem"), "free memory to pool, level: " << (int)m_CacheLevel);
		}
		else
		{
			//LOG4CXX_ERROR(GETLOGGER("FreeMem"), "free memory error, the ptr is not in uselist");
		}
		return 1;
	}

	unsigned char CMemPool::GetLevel()
	{
		return m_CacheLevel;
	}

	int CMemPool::GetPieceSize()
	{
		return m_nPieceSize;
	}


	CMemMgr::CMemMgr()
	{

	}

	CMemMgr::~CMemMgr()
	{
		for (int i = 0; i < MEMPOOLNUM; i++)
		{
			if (m_MemMgrArray[i])
			{
				delete m_MemMgrArray[i];
			}
		}
	}

	CMemMgr* CMemMgr::GetInstance()
	{
		if (m_myMemMgr == NULL)
		{
			m_myMemMgr = new CMemMgr();
		}
		return m_myMemMgr;
	}

	int CMemMgr::Init(map<int, int> &mapMemConf)
	{
		if (mapMemConf.size() == 0)
		{
			return -1;
		}
		int i = 0;
		map<int, int>::iterator it = mapMemConf.begin();
		for (; it != mapMemConf.end(); it++)
		{
			if (it->first < 0 || it->second < 0)
			{
				return -1;
			}
			CMemPool *pMemPool = new CMemPool();
			if (!pMemPool)
			{
				return -1;
			}
			if (pMemPool->Init(it->first, it->second, i) != 1)
			{
				return -1;
			}
			m_MemMgrArray[i] = pMemPool;
			++i;
		}
		m_nRealLevel = i;

		m_directNewList.clear();
		return 1;
	}

	char* CMemMgr::GetMem(int nLen,int& realLen)
	{
		MutexLock lock(&m_mutex);
		//alloc mem from level pool first;
		for (int i = 0; i < m_nRealLevel; i++)
		{
			int nPieceSize = m_MemMgrArray[i]->GetPieceSize();
			if (nLen <= nPieceSize)
			{
				//LOG4CXX_DEBUG(GETLOGGER("GetMem"), "len: " << nLen  << " level: " << i);
				char *pRet = m_MemMgrArray[i]->GetMem(nPieceSize);
				if (pRet == NULL)
				{
					break;
				}
				else
				{
					realLen = nPieceSize;
					return pRet;
				}
			}
		}
		//if the len is too big; alloc mem from m_directNewList;

		unsigned char Level = MEMPOOLNUM ;
		char *p = new char[nLen + sizeof(Level)];
		memset(p, 0, nLen);
		memcpy(p, &Level, sizeof(Level));
		p += sizeof(Level);
		//LOG4CXX_DEBUG(GETLOGGER("GetMem"), "len: " << nLen  << " level: " << (int)Level);
		m_directNewList.insert(p);
		realLen = nLen;
		return p;
	}

	char* CMemMgr::GetMem(int nLen)
	{
		//sGetMem++;
		//LOG4CXX_DEBUG(GETLOGGER("GetMem"),"getmemnum:" << sGetMem);
		if (nLen <= 0)
		{
			return NULL;
		}
		MutexLock lock(&m_mutex);
		//alloc mem from level pool first;
		for (int i = 0; i < m_nRealLevel; i++)
		{
			int nPieceSize = m_MemMgrArray[i]->GetPieceSize();
			if (nLen <= nPieceSize)
			{
				//LOG4CXX_DEBUG(GETLOGGER("GetMem"), "len: " << nLen  << " level: " << i);
				char *pRet = m_MemMgrArray[i]->GetMem(nPieceSize);
				if (pRet == NULL)
				{
					break;
				}
				else
				{
					return pRet;
				}
			}
		}
		//if the len is too big; alloc mem from m_directNewList;

		unsigned char Level = MEMPOOLNUM ;
		char *p = new char[nLen + sizeof(Level)];
		memset(p, 0, nLen);
		memcpy(p, &Level, sizeof(Level));
		p += sizeof(Level);
		//LOG4CXX_DEBUG(GETLOGGER("GetMem"), "len: " << nLen  << " level: " << (int)Level);
		m_directNewList.insert(p);
		return p;
	}

	int CMemMgr::FreeMem(char *pChar)
	{
		//sFreeMem++;
		//LOG4CXX_DEBUG(GETLOGGER("GetMem"),"FreeMem:" << sFreeMem);
		if (!pChar)
		{
			return -1;
		}
		MutexLock lock(&m_mutex);
		unsigned char Level = 100;
		memcpy(&Level, pChar - sizeof(Level), sizeof(Level));
		if (Level < 0 || Level > MEMPOOLNUM)
		{
			//LOG4CXX_DEBUG(GETLOGGER("FreeMem"), "level is error:  " << Level);
			return -1;
		}
		if (Level == MEMPOOLNUM)
		{
			set<char *>::iterator setItr = m_directNewList.find(pChar);
			if (setItr != m_directNewList.end())
			{
				char *p = *setItr;
				if (!p)
				{
					p -= sizeof(Level);
					delete [] p;
					//LOG4CXX_DEBUG(GETLOGGER("FreeMem"), "free memory to pool,level: " << (int)Level);
				}
				m_directNewList.erase(setItr);
				return 1;
			}
		}
		else
		{
			return m_MemMgrArray[Level]->FreeMem(pChar);
		}
		return 1;
	}






	/*
	* =====================================================================================
	*
	*       Filename:  mempool.h
	*    Description:  内存池，一种是分配block编号，一种直接分配地址，两者都支持动态增长
	*        Version:  1.0
	*        Created:  2016-02-16
	*       Revision:  none
	*       Compiler:  gcc
	*         Author:  jbian
	*        Company:  wind, Inc.
	*
	* =====================================================================================
	*/
	void* memalloc::malloc()
	{
		void *p = m_free.front();
		m_free.pop();
		return p;
	}

	void memalloc::free(void* ptr)
	{
		m_free.push(ptr);
	}

	void* memalloc::borrow(size_t size)
	{
		size_t bsize = 1<<m_bits;
		size_t count = size/bsize;

		void *p = ::malloc(bsize*count);
		if (!p) {
			WINDLOG_WARN(__FUNCTION__, "not enough memory");
			return 0;
		}

		for (size_t i=0; i<count; ++i)
			m_free.push((void*)((char*)p+i*bsize));

		return p;
	}

	mempool* mempool::m_pinst = 0;

	mempool* mempool::inst()
	{
		if (m_pinst == 0) {
			inst_lock().Lock();
			if (m_pinst == 0) {
				m_pinst = new mempool();
				m_pinst->init();
			}
			inst_lock().UnLock();
		}
		return m_pinst;
	}

	void* mempool::malloc(size_t size)
	{
		//超过1M的数据直接向操作系统要
		if (size > 1024*1024) {
			MicroSecsTimer ms;
			ms.Start();
			//WINDLOG_DEBUG(__FUNCTION__, "malloc from system");
			void *p = ::malloc(size);
			WINDLOG_INFO(__FUNCTION__, "sys malloc(" << size << ")cost:" << ms.Stop() << "us");
			return p;
		}

		//小于4Bytes的数据按照4Bytes申请
		if (size < 4) size = 4;
		uint idx = size2bits(size);

		m_blocks[idx].lock();
		if (m_blocks[idx].space() == 0)
		{
			long long cost1 = 0, cost2 = 0;
			MicroSecsTimer ms;
			ms.Start();
			//没有空间了就去申请，一次申请4M
			void *p = m_blocks[idx].borrow(4*1024*1024);
			cost1 = ms.Stop();
			ms.Reset(); ms.Start();
			if (p == 0) {
				m_blocks[idx].unlock();
				return 0;
			}
			else {
				mptr mp(idx, p);
				add_sort(mp);
			}
			cost2 = ms.Stop();
			WINDLOG_INFO(__FUNCTION__, "malloc(" << size << ")cost1:" << cost1 << "us,cost2:" << cost2 << "us");
		}
		void *p = m_blocks[idx].malloc();
		m_blocks[idx].unlock();
		return p;
	}

	void mempool::free(void *ptr)
	{
		if (!ptr) return;
		int idx = find_ptr(ptr);
		if (idx == -1) {
			//WINDLOG_DEBUG(__FUNCTION__, "free to system");
			return ::free(ptr);
		}
		m_blocks[idx].lock();
		m_blocks[idx].free(ptr);
		m_blocks[idx].unlock();
	}

	size_t mempool::chunk()
	{
		size_t s = 0;
		m_lockptr.LockW();
		s = m_ptrs.size();
		m_lockptr.UnLockW();
		return s;
	}

	void mempool::init()
	{
		for (int i=0; i<=MEM_POOL_LEVEL; ++i)
			m_blocks[i].init(i);
	}

	void mempool::add_sort(const mptr &p)
	{
		m_lockptr.LockW();
		m_ptrs.push_back(p);
		std::sort(m_ptrs.begin(), m_ptrs.end(), mempool::compare_ptr);
		m_lockptr.UnLockW();
	}

	int mempool::begin_ptr(void *p)
	{
		int first = 0;
		int end = m_ptrs.size();

		while (first < end-1)
		{
			int middle = (first + end) >> 1;
			if (m_ptrs[middle].m_ptr < p)
				first = middle;
			else if (m_ptrs[middle].m_ptr > p)
				end = middle;
			else
				return middle;
		}

		return (p<m_ptrs[first].m_ptr)?-1:first;
	}

	int mempool::find_ptr(void *p)
	{
		m_lockptr.LockR();
		int begin = begin_ptr(p);
		if (begin == -1) {
			m_lockptr.UnLockR();
			return -1;
		}
		//该指针和基地址不会超过4M
		if ((ulong64)p-(ulong64)m_ptrs[begin].m_ptr >= 4*1024*1024) {
			m_lockptr.UnLockR();
			return -1;
		}
		int idx = m_ptrs[begin].m_blk;
		m_lockptr.UnLockR();
		return idx;
	}

	Mutex& mempool::inst_lock() {
		static Mutex lock;  
		return lock; 
	}




	int blkalloc::malloc()
	{
		m_lock.LockW();
		if (m_free.size() == 0 && !borrow(4*1024*1024)) {
			m_lock.UnLockW();
			return -1;
		}
		uint nfree = m_free.front();
		m_free.pop();
		m_lock.UnLockW();
		return (int)nfree;
	}

	void blkalloc::free(uint block)
	{
		m_lock.LockW();
		m_free.push(block);
		m_lock.UnLockW();
	}

	void* blkalloc::ask(uint block)
	{
		m_lock.LockR();
		int idx = begin_blk(block);
		if (idx == -1) {
			m_lock.UnLockR();
			return 0;
		}
		size_t offset = block - m_ptrs[idx].m_blk;
		offset <<= m_bits;
		char *p = (char*)m_ptrs[idx].m_ptr+offset;
		m_lock.UnLockR();
		return (void*)p;
	}

	size_t blkalloc::chunk()
	{
		size_t s = 0;
		m_lock.LockW();
		s = m_ptrs.size();
		m_lock.UnLockW();
		return s;
	}

	bool blkalloc::borrow(size_t size)
	{
		size_t bsize = 1<<m_bits;
		size_t count = size/bsize;

		void *p = ::malloc(bsize*count);
		if (!p) {
			WINDLOG_WARN(__FUNCTION__, "not enough memory");
			return false;
		}

		mptr mb(m_blks, p);
		m_ptrs.push_back(mb);

		for (size_t i=0; i<count; ++i)
			m_free.push(m_blks++);

		return true;
	}

	int blkalloc::begin_blk(uint block)
	{
		int first = 0;
		int end = m_ptrs.size();

		while (first < end-1)
		{
			int middle = (first + end) >> 1;
			if (m_ptrs[middle].m_blk < block)
				first = middle;
			else if (m_ptrs[middle].m_blk > block)
				end = middle;
			else
				return middle;
		}

		return (block<m_ptrs[first].m_blk)?-1:first;
	}



	blkpool* blkpool::m_pinst = 0;
	Mutex blkpool::m_lock;

	blkpool* blkpool::inst()
	{
		if (m_pinst == 0) {
			m_lock.Lock();
			if (m_pinst == 0) {
				m_pinst = new blkpool();
				m_pinst->init();
			}
			m_lock.UnLock();
		}
		return m_pinst;
	}

	uint blkpool::malloc(size_t size)
	{
		//小于4Bytes的数据按照4Bytes申请
		if (size < 4) size = 4;
		uint idx = size2bits(size);
		int block = m_blocks[idx].malloc();
		if (block == -1) return 0;
		return (idx<<27)+block;
	}

	void blkpool::free(uint blk)
	{
		if (blk == 0) return;
		uint idx = blk>>27;
		m_blocks[idx].free(blk&0x7ffffff);
	}

	void* blkpool::ask(uint blk)
	{
		if (blk == 0) return 0;
		uint idx = blk>>27;
		return m_blocks[idx].ask(blk&0x7ffffff);
	}

	size_t blkpool::chunk()
	{
		size_t s = 0;
		for (int i=0; i<=BLK_POOL_LEVEL; ++i)
			s += m_blocks[i].chunk();
		return s;
	}

	uint blkpool::size(uint blk)
	{
		if (blk == 0) return 0;
		uint idx = blk>>27;
		return 1<<idx;
	}

	void blkpool::init()
	{
		for (int i=0; i<=BLK_POOL_LEVEL; ++i)
			m_blocks[i].init(i);
	}
}

