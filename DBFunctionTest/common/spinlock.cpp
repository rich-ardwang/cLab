#include "common/spinlock.h"

#include "common/atomic.h"

namespace windcom
{

SpinLock::SpinLock():lock(0)
{
}

SpinLock::~SpinLock()
{
}

void SpinLock::Lock()
{
    while(!Atomic_CmpAndSwap(&lock, 0, 1))
    {
        long long n = 20000;
        n--;
    }
}

void SpinLock::UnLock()
{
    lock = 0;
}

bool SpinLock::TryLock()
{
    return Atomic_CmpAndSwap(&lock, 0, 1);
}


AutoSpinLock::AutoSpinLock(const SpinLock& lock):_lock(lock)
{
    _lock.Lock();
}

AutoSpinLock::~AutoSpinLock()
{
    _lock.UnLock();
}

}

