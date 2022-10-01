/*
 * ========================================================
 *
 * Author:         dingfulai
 * Version:        1.0
 * Created:        2013/12/02
 * Description:    基于原子操作实现的用户态读写锁，使用宏
                   RWL_STATISTICS开启读写锁的统计功能。

                   该读写锁不检查并发读的数量，超过最大并
                   发读线程数将导致未确定的行为。                   

 * History:
 *  <author>    <time>      <version>   <desc>
 *  dingfulai   2013/12/02  1.0         创建文件
 *
 * ========================================================
 *
 */

#ifndef _WIND_RWLOCK_H_
#define _WIND_RWLOCK_H_

#include "common/atomic.h"
#include "common/mytime.h"

namespace windcom
{
    /*
     * 读写锁使用多少个字节来保存所需要的信息，取值范围1-4
     *
     * 此值为1，数据掩码为0xff000000，读锁掩码为0xfe000000，写锁掩码为0x01000000
     * 此值为2，数据掩码为0xffff0000，读锁掩码为0xfffe0000，写锁掩码为0x00010000
     * 此值为3，数据掩码为0xffffff00，读锁掩码为0xfffffe00，写锁掩码为0x00000100
     * 此值为4，数据掩码为0xffffffff，读锁掩码为0xfffffffe，写锁掩码为0x00000001
     * 
     * 此值为1时，读锁数量(2^6  - 1 =         63)个，超过后运行结果不确定
     * 此值为2时，读锁数量(2^14 - 1 =      16383)个，超过后运行结果不确定
     * 此值为1时，读锁数量(2^22 - 1 =    4194303)个，超过后运行结果不确定
     * 此值为2时，读锁数量(2^30 - 1 = 1073741823)个，超过后运行结果不确定
     */

    template<unsigned int Chars>
    class _RWLock
    {
    public:
        // 构造函数指定读写锁使用的字节数，默认为1
        _RWLock();
        ~_RWLock();

    public:
        // 静态函数
        static bool STryLockR(int* lock, unsigned int retry);
        static bool STryLockW(int* lock, unsigned int retry);
        static void SLockR(int* lock);
        static void SLockW(int* lock);
        static void SUnLockR(int* lock);
        static void SUnLockW(int* lock);

    public:
        bool TryLockR(unsigned int uiRetry);
        bool TryLockW(unsigned int uiRetry);
        void LockR();
        void LockW();
        void UnLockR();
        void UnLockW();

    public:
        unsigned int MaxRead() const { return iReadMax; };

    private:
        bool inline Write(bool flag)
        {
            if(flag)
                return Atomic_FetchAndBitSet(&m_iLock, iWriteBit);
            else
                return Atomic_FetchAndBitReset(&m_iLock, iWriteBit);
        }
        
        int inline IncRead()
        {
            return Atomic_FetchAndAdd(&m_iLock, iReadStep);
        }
        
        int inline DecRead()
        {
            return Atomic_FetchAndSub(&m_iLock, iReadStep);
        }
        
        bool inline Reading()
        {
            return 0 != (Atomic_AndAndFetch(&m_iLock, 0xffffffff) & iMaskRead);
        }
        
        bool inline Writing(int value)
        {
            return 0 != (value & iMaskWrite);
        }
        
        int inline ReadCount(int value)
        {
            return (value & iMaskRead) >> (iWriteBit + 1);
        }

    private:
        int m_iLock;

        static const unsigned int iWriteBit  = (4 - Chars) << 3;
        static const unsigned int iMaskLock  = 0xFFFFFFFF << iWriteBit;
        static const unsigned int iMaskWrite = ~iMaskLock + 1;
        static const unsigned int iMaskRead  = iMaskLock - iMaskWrite;
        static const unsigned int iReadStep  = iMaskWrite << 1;
        static const unsigned int iReadMax   = (unsigned int)(1 << ((Chars << 3) - 1)) - 1;
    };


    template<unsigned int Chars>
    _RWLock<Chars>::_RWLock():m_iLock(0)
    {
    }

    template<unsigned int Chars>
    _RWLock<Chars>::~_RWLock()
    {
    }

    template<unsigned int Chars>
    bool _RWLock<Chars>::STryLockR(int* lock, unsigned int retry /* = 3 */)
    {
        _RWLock<Chars> *oLock = (_RWLock<Chars> *)lock;
        return oLock->TryLockR(retry);
    }

    template<unsigned int Chars>
    bool _RWLock<Chars>::STryLockW(int *lock, unsigned int retry /* = 3 */)
    {
        _RWLock<Chars> *oLock = (_RWLock<Chars> *)lock;
        return oLock->TryLockW(retry);
    }

    template<unsigned int Chars>
    void _RWLock<Chars>::SLockR(int* lock)
    {
        _RWLock<Chars> *oLock = (_RWLock<Chars> *)lock;
        return oLock->LockR();
    }

    template<unsigned int Chars>
    void _RWLock<Chars>::SLockW(int *lock)
    {
        _RWLock<Chars> *oLock = (_RWLock<Chars> *)lock;
        return oLock->LockW();
    }

    template<unsigned int Chars>
    void _RWLock<Chars>::SUnLockR(int *lock)
    {
        _RWLock<Chars> *oLock = (_RWLock<Chars> *)lock;
        return oLock->UnLockR();
    }

    template<unsigned int Chars>
    void _RWLock<Chars>::SUnLockW(int *lock)
    {
        _RWLock<Chars> *oLock = (_RWLock<Chars> *)lock;
        return oLock->UnLockW();
    }

    template<unsigned int Chars>
    bool _RWLock<Chars>::TryLockR(unsigned int uiRetry)
    {
        unsigned int nRetryCount = 0;
        int nOld;

        while (1)
        {
            // 重试次数为0时，表示一直重试直到成功为止
            if ((uiRetry != 0) && (nRetryCount++ >= uiRetry))
                break;

            // 直接读锁加1,如果有写操作存在，读锁-1进行调整
            nOld = IncRead();
            if (Writing(nOld))
            {
                DecRead();
                MSleep(1);
            }
            // 如果已经超出读锁限制再-1进行调整
            else if (ReadCount(nOld) >= iReadMax)
            {
                DecRead();
                MSleep(1);
            }
            else
            {
                return true;
            }
        }
        return false;
    }

    template<unsigned int Chars>
    bool _RWLock<Chars>::TryLockW(unsigned int uiRetry)
    {
        unsigned int nRetryCount = 0;

        while (1)
        {
            // 重试次数为0时，表示一直重试直到成功为止
            if ((uiRetry != 0) && (nRetryCount++ >= uiRetry))
                break;

            // 首先占据写标志位,如果已经有写存在，循环等待
            if (!Write(true))
            {
                MSleep(1);
                continue;
            }

            //等待所有的读退出
            while (1)
            {
                if ((uiRetry != 0) && (nRetryCount++ >= uiRetry))
                    break;

                if (Reading())
                {
#ifdef _WIN32
                    MSleep(1);
#else
                    USleep(50);
#endif
                    continue;
                }
                else
                {
                    return true;
                }
            }

            //如果所有的读都没有退出，释放写标志位
            Write(false);

            return false;
        }

        return false;
    }

    template<unsigned int Chars>
    void _RWLock<Chars>::LockR()
    {
        _RWLock<Chars>::TryLockR((unsigned int)0);
    }

    template<unsigned int Chars>
    void _RWLock<Chars>::LockW()
    {
        _RWLock<Chars>::TryLockW((unsigned int)0);
    }

    template<unsigned int Chars>
    void _RWLock<Chars>::UnLockR()
    {
        (void)DecRead();
    }

    template<unsigned int Chars>
    void _RWLock<Chars>::UnLockW()
    {
        Write(false);
    }

    // 一个并发读上限是63的读写锁，由用户负责限制并发读上限，否则会出现未定义行为
    typedef _RWLock<4> RWLock;
    typedef _RWLock<2> RWLock2;
    typedef _RWLock<3> RWLock3;
    typedef _RWLock<4> RWLock4;
}


#endif // _RWLOCK_H_

