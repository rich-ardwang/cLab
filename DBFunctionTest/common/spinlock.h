/*
 * =====================================================================================
 *
 *       Filename:  spinlock.h
 *    Description:  �û�̬��������װ����������2014-4-30��
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

#ifndef _WIND_SPINLOCK_H_
#define _WIND_SPINLOCK_H_

namespace windcom
{

    // ������
    class SpinLock
    {
    public:
        SpinLock();
        ~SpinLock();

    public:
        void Lock();
        void UnLock();
        bool TryLock();

    private:
        int lock;
    };

    // �������Թ�����
    class AutoSpinLock
    {
    public:
        AutoSpinLock(const SpinLock& lock);
        ~AutoSpinLock();

    private:
        AutoSpinLock(const AutoSpinLock&);
        AutoSpinLock& operator = (const AutoSpinLock&);

    private:
        SpinLock _lock;
    };

}

#endif

