/* =====================================================================================
*
*       Filename:  mempool.h
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

#ifndef _WIND_MEM_POOL_H_
#define _WIND_MEM_POOL_H_

#include <set>
#include <list>
#include <queue>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <cstdlib>
#include "common/mytypes.h"
#include "common/myerror.h"
#include "common/mutex.h"
#include "common/rwlock.h"

using namespace std;

namespace windcom
{
#define MYNEW(len) CMemMgr::GetInstance()->GetMem(len)

#define MYDELETE(p) CMemMgr::GetInstance()->FreeMem(p)

#define IMNEW(len,realLen) CMemMgr::GetInstance()->GetMem(len,realLen)

#define IMDELETE(p) CMemMgr::GetInstance()->FreeMem(p)

#define MEMPOOLNUM 10

    //static int sGetMem = 0;
    //static int sFreeMem = 0;
    //内存池
    class CMemPool
    {
    public:
        CMemPool();
        ~CMemPool();

        int Init(int nPieceSize, int nMaxLen, int nLevel);
        char *GetMem(int nLen);
        int FreeMem(char *pChar);
        unsigned char GetLevel();
        int GetPieceSize();

    private:
        char *m_pMemCache;
        int  m_nPieceSize;
        int m_nTotalSize;
        unsigned char m_CacheLevel;
        set<char *> m_useList;
        queue<char *> m_freeList;
		windcom::Mutex m_mutex;
    };

    //各种内存池的管理类,支持不定长的内存申请,不过不做维护,交还后立即释放
    class CMemMgr
    {
    public:
        static CMemMgr *GetInstance();
        int Init(map<int, int> &mapMemConf);
		char* GetMem(int nLen,int& realLen);
        char *GetMem(int nLen);
        int FreeMem(char *pChar);

    private:
        CMemMgr();
        ~CMemMgr();

        static CMemMgr *m_myMemMgr;
        set<char *> m_directNewList;
        CMemPool *m_MemMgrArray[MEMPOOLNUM];
        int m_nRealLevel;
		windcom::Mutex m_mutex;
    };




	/*
	* =====================================================================================
	*       Filename:  mempool.h
	*    Description:  可以动态增长的内存池，每次分配都是4M
	*        Version:  1.0
	*        Created:  2016-02-16
	*       Revision:  none
	*       Compiler:  gcc
	*         Author:  jbian
	*        Company:  wind, Inc.
	* =====================================================================================
	*/
	#define MEM_POOL_LEVEL	20UL	//一共20级，最大支持2^20=1M，超过1M的向操作系统分配
	#define BLK_POOL_LEVEL	20UL	//一共20级，最大支持2^20=1M，超过1M的向操作系统分配

	struct mptr {
		mptr():m_blk(0),m_ptr(0) {
		}

		mptr(uint block, void *ptr):
		m_blk(block),m_ptr(ptr) {
		}

		mptr(const mptr &rhs) {
			m_blk = rhs.m_blk;
			m_ptr = rhs.m_ptr;
		}

		mptr& operator =(const mptr &rhs) {
			if (this != &rhs) { 
				m_blk = rhs.m_blk;
				m_ptr = rhs.m_ptr;
			} 
			return *this; 
		}

		uint				m_blk;
		void				*m_ptr;
	};

	class memalloc
	{
	public:
		memalloc():m_bits(0){}
		~memalloc(){}

	public:
		void init(uint blockbits) { m_bits = blockbits; }
		void* malloc();
		void free(void* ptr);
		void* borrow(size_t size);
		inline void lock() { m_lock.LockW(); }
		inline void unlock() { m_lock.UnLockW(); }
		inline size_t space() const { return m_free.size(); }

	private:
		uint			m_bits;
		RWLock			m_lock;
		queue<void*>	m_free;
	};

	class mempool
	{
	public:
		static mempool* inst();
		~mempool(){}

	public:
		void* malloc(size_t size);
		void free(void* ptr);
		size_t chunk();

	private:
		void init();
		void add_sort(const mptr &p);
		int begin_ptr(void *p);
		int find_ptr(void *p);
		static Mutex& inst_lock();
		static bool compare_ptr(mptr mp1, mptr mp2) {
			return mp1.m_ptr < mp2.m_ptr;
		}

	private:
		mempool(){}
		mempool(const mempool &);
		mempool& operator =(const mempool &);
		static mempool *m_pinst;
		memalloc m_blocks[MEM_POOL_LEVEL+1];
		vector<mptr> m_ptrs;
		RWLock m_lockptr;
	};


	class blkalloc
	{
	public:
		blkalloc():m_bits(0),m_blks(0){}
		~blkalloc(){}

	public:
		void init(uint bits) { m_bits = bits; }
		int malloc();//失败返回-1
		void free(uint block);
		void* ask(uint block);
		size_t chunk();

	private:
		bool borrow(size_t size);
		int begin_blk(uint block);

	private:
		uint				m_bits;
		uint				m_blks;
		RWLock				m_lock;
		queue<uint>			m_free;
		vector<mptr>		m_ptrs;
	};


	class blkpool
	{
	public:
		static blkpool* inst();
		~blkpool(){}

	public:
		uint malloc(size_t size);
		void free(uint blk);
		void* ask(uint blk);
		uint size(uint blk);
		size_t chunk();

	private:
		void init();

	private:
		blkpool(){}
		blkpool(const blkpool &);
		blkpool& operator =(const blkpool &);
		static blkpool *m_pinst;
		static Mutex m_lock;
		blkalloc m_blocks[BLK_POOL_LEVEL+1];
	};

	static int size2bits(size_t size)
	{
		static const uchar log2[256] = {
			0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
			6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
			7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
			7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
			8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
			8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
			8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
			8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
		};

		int b = -1;
		if ((size & (size-1)) != 0) ++b;
		while (size >= 256) { b += 8; size >>= 8; }
		return b + log2[size];
	}

	class StdMem
	{
	public:
		static char* Malloc(size_t size) {
			//return (char*)malloc(size);
			return (char*)mempool::inst()->malloc(size);
		}

		static void Free(const char *p) {
			//free((void*)p);
			mempool::inst()->free((void*)p);
		}
	};
}

#define WMEMPOOL windcom::mempool::inst()
#define WBLKPOOL windcom::blkpool::inst()

#endif
