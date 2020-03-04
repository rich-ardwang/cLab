/***************************************************************************************************
*Copyright(C): 2019, Sumscope
*FileName    : atomic.h
*Author      : lei.wang@sumscope.com
*Version     : 1.0
*Date        : 2019-10-11
*Desc        : atomic operation
*History     : Add by Lei.Wang 2019-10-11
***************************************************************************************************/
#ifndef __LAVA_ATOMIC_H__
#define __LAVA_ATOMIC_H__

#ifndef _WIN32
#ifndef __linux__
#error "Unknown os platform, _WIN32 or __linux__ not defined."
#endif
#endif

namespace lava
{

	int inline Atomic_AndAndFetch(int* addend, int value);
	bool inline Atomic_FetchAndBitSet(int* addend, int bit);
	bool inline Atomic_FetchAndBitReset(int* addend, int bit);
	int inline Atomic_FetchAndAdd(int* addend, int value);
	int inline Atomic_FetchAndSub(int* addend, int value);
	int inline Atomic_FetchAndSet(int *target, int value);
	int inline Atomic_Increment(int* addend);
	int inline Atomic_Decrement(int* addend);
	bool inline Atomic_CmpAndSwap(int* targe, int value, int cmp);

}

#ifdef _WIN32
#include <intrin.h>
#pragma intrinsic(_InterlockedAnd)
#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedExchangeAdd)
#pragma intrinsic(_interlockedbittestandset)
#pragma intrinsic(_interlockedbittestandreset)
#endif

namespace lava
{

	int inline Atomic_AndAndFetch(int* addend, int value)
	{
#ifdef _WIN32
		return _InterlockedAnd((long*)addend, value);
#else
		return __sync_and_and_fetch(addend, value);
#endif
	}

	bool inline Atomic_FetchAndBitSet(int* addend, int bit)
	{
#ifdef _WIN32
		return !(_interlockedbittestandset((long*)addend, bit));
#else
		return !(__sync_fetch_and_or(addend, 1<<bit) & (1<<bit));
#endif
	}

	bool inline Atomic_FetchAndBitReset(int* addend, int bit)
	{
#ifdef _WIN32
		return 0 != (_interlockedbittestandreset((long*)addend, bit));
#else
		return 0 != (__sync_fetch_and_and(addend, ~(1<<bit)) & (1<<bit));
#endif
	}

	int inline Atomic_FetchAndAdd(int* addend, int value)
	{
#ifdef _WIN32
		return (int)_InterlockedExchangeAdd((long*)addend, value);
#else
		return __sync_fetch_and_add(addend, value);
#endif
	}

	int inline Atomic_FetchAndSub(int* addend, int value)
	{
#ifdef _WIN32
		return (int)_InterlockedExchangeAdd((long*)addend, ~value + 1);
#else
		return __sync_fetch_and_sub(addend, value);
#endif
	}

	int inline Atomic_FetchAndSet(int *target, int value)
	{
#ifdef _WIN32
		return (int)_InterlockedExchange((long*)target, value);
#else
		return __sync_lock_test_and_set(target, value);
#endif
	}

	int inline Atomic_Increment(int* addend)
	{
#ifdef _WIN32
		return (long)_InterlockedIncrement((long*)addend);
#else
		return __sync_add_and_fetch(addend, 1);
#endif
	}

	int inline Atomic_Decrement(int* addend)
	{
#ifdef _WIN32
		return (long)_InterlockedDecrement((long*)addend);
#else
		return __sync_sub_and_fetch(addend, 1);
#endif
	}

	bool inline Atomic_CmpAndSwap(int* targe, int oldval, int newval)
	{
#ifdef _WIN32
		return oldval == _InterlockedCompareExchange((long*)targe, newval, oldval);
#else
		return __sync_bool_compare_and_swap(targe, oldval, newval);
#endif
	}

}

#endif	/* __LAVA_ATOMIC_H__ */
