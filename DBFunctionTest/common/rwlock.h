/*
 * ========================================================
 *
 * Author:         dingfulai
 * Version:        1.0
 * Created:        2013/12/02
 * Description:    ����ԭ�Ӳ���ʵ�ֵ��û�̬��д����ʹ�ú�
                   RWL_STATISTICS������д����ͳ�ƹ��ܡ�

                   �ö�д������鲢�������������������
                   �����߳���������δȷ������Ϊ��                   

 * History:
 *  <author>    <time>      <version>   <desc>
 *  dingfulai   2013/12/02  1.0         �����ļ�
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
     * ��д��ʹ�ö��ٸ��ֽ�����������Ҫ����Ϣ��ȡֵ��Χ1-4
     *
     * ��ֵΪ1����������Ϊ0xff000000����������Ϊ0xfe000000��д������Ϊ0x01000000
     * ��ֵΪ2����������Ϊ0xffff0000����������Ϊ0xfffe0000��д������Ϊ0x00010000
     * ��ֵΪ3����������Ϊ0xffffff00����������Ϊ0xfffffe00��д������Ϊ0x00000100
     * ��ֵΪ4����������Ϊ0xffffffff����������Ϊ0xfffffffe��д������Ϊ0x00000001
     * 
     * ��ֵΪ1ʱ����������(2^6  - 1 =         63)�������������н����ȷ��
     * ��ֵΪ2ʱ����������(2^14 - 1 =      16383)�������������н����ȷ��
     * ��ֵΪ1ʱ����������(2^22 - 1 =    4194303)�������������н����ȷ��
     * ��ֵΪ2ʱ����������(2^30 - 1 = 1073741823)�������������н����ȷ��
     */

    template<unsigned int Chars>
    class _RWLock
    {
    public:
        // ���캯��ָ����д��ʹ�õ��ֽ�����Ĭ��Ϊ1
        _RWLock();
        ~_RWLock();

    public:
        // ��̬����
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
            // ���Դ���Ϊ0ʱ����ʾһֱ����ֱ���ɹ�Ϊֹ
            if ((uiRetry != 0) && (nRetryCount++ >= uiRetry))
                break;

            // ֱ�Ӷ�����1,�����д�������ڣ�����-1���е���
            nOld = IncRead();
            if (Writing(nOld))
            {
                DecRead();
                MSleep(1);
            }
            // ����Ѿ���������������-1���е���
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
            // ���Դ���Ϊ0ʱ����ʾһֱ����ֱ���ɹ�Ϊֹ
            if ((uiRetry != 0) && (nRetryCount++ >= uiRetry))
                break;

            // ����ռ��д��־λ,����Ѿ���д���ڣ�ѭ���ȴ�
            if (!Write(true))
            {
                MSleep(1);
                continue;
            }

            //�ȴ����еĶ��˳�
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

            //������еĶ���û���˳����ͷ�д��־λ
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

    // һ��������������63�Ķ�д�������û��������Ʋ��������ޣ���������δ������Ϊ
    typedef _RWLock<4> RWLock;
    typedef _RWLock<2> RWLock2;
    typedef _RWLock<3> RWLock3;
    typedef _RWLock<4> RWLock4;
}


#endif // _RWLOCK_H_

