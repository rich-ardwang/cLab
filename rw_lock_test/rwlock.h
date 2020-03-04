/***************************************************************************************************
*Copyright(C): 2019, Sumscope
*FileName    : rwlock.h
*Author      : lei.wang@sumscope.com
*Version     : 1.0
*Date        : 2019-10-11
*Desc        : user state read-write lock based on atomic operation.
*            : [warning]:
*            :      this rw_lock does not check the number of concurrent reads, 
*            :      and if exceeding the maximum number of concurrent reads, 
*            :      maybe undetermined behavior will happen.
*History     : Add by Lei.Wang 2019-10-11
***************************************************************************************************/
#ifndef __LAVA_RWLOCK_H__
#define __LAVA_RWLOCK_H__
#include <chrono>
#include "atomic.h"


namespace lava
{

/*
 * bytes that the rw_lock used to hold the required information.
 * value range is 1-4.
 *
 * value:1£¬data mask:0xff000000£¬r_lock mask:0xfe000000£¬w_lock mask:0x01000000
 * value:2£¬data mask:0xffff0000£¬r_lock mask:0xfffe0000£¬w_lock mask:0x00010000
 * value:3£¬data mask:0xffffff00£¬r_lock mask:0xfffffe00£¬w_lock mask:0x00000100
 * value:4£¬data mask:0xffffffff£¬r_lock mask:0xfffffffe£¬w_lock mask:0x00000001
 *
 * value:1£¬r_lock count:(2^6  - 1 =         63)
 * value:2£¬r_lock count:(2^14 - 1 =      16383)
 * value:3£¬r_lock count:(2^22 - 1 =    4194303)
 * value:4£¬r_lock count:(2^30 - 1 = 1073741823)
 *
 * if exceeding the r_lock count, the running result will be uncertain.
 */

template<unsigned int Bytes>
class rw_lock
{
public:
	//construct function set the bytes that used for rw_lock, 
	//default bytes = 1.
	rw_lock() : n_lock_(0) {}
	~rw_lock() {}

public:
	//static functions
	static bool s_try_lock_r(int* lock, unsigned int retry);
	static bool s_try_lock_w(int* lock, unsigned int retry);
	static void s_lock_r(int* lock);
	static void s_lock_w(int* lock);
	static void s_unlock_r(int* lock);
	static void s_unlock_w(int* lock);

public:
	bool try_lock_r(unsigned int uiRetry);
	bool try_lock_w(unsigned int uiRetry);
	void lock_r();
	void lock_w();
	void unlock_r();
	void unlock_w();

public:
	unsigned int max_read() const { return n_read_max_; };

private:
	bool inline write_flag(bool flag)
	{
		if (flag)
			return Atomic_FetchAndBitSet(&n_lock_, n_write_bit_);
		else
			return Atomic_FetchAndBitReset(&n_lock_, n_write_bit_);
	}

	int inline inc_read()
	{
		return Atomic_FetchAndAdd(&n_lock_, n_read_step_);
	}

	int inline dec_read()
	{
		return Atomic_FetchAndSub(&n_lock_, n_read_step_);
	}

	bool inline reading()
	{
		return 0 != (Atomic_AndAndFetch(&n_lock_, 0xffffffff) & n_mask_read_);
	}

	bool inline writing(int value)
	{
		return 0 != (value & n_mask_write_);
	}

	int inline read_count(int value)
	{
		return (value & n_mask_read_) >> (n_write_bit_ + 1);
	}

private:
	int n_lock_;
	static const unsigned int n_write_bit_ = (4 - Bytes) << 3;
	static const unsigned int n_mask_lock_ = 0xFFFFFFFF << n_write_bit_;
	static const unsigned int n_mask_write_ = ~n_mask_lock_ + 1;
	static const unsigned int n_mask_read_ = n_mask_lock_ - n_mask_write_;
	static const unsigned int n_read_step_ = n_mask_write_ << 1;
	static const unsigned int n_read_max_ = (unsigned int)(1 << ((Bytes << 3) - 1)) - 1;
};

template<unsigned int Bytes>
bool rw_lock<Bytes>::s_try_lock_r(int* lock, unsigned int retry /* = 3 */)
{
	rw_lock<Bytes> *oLock = (rw_lock<Bytes> *)lock;
	return oLock->try_lock_r(retry);
}

template<unsigned int Bytes>
bool rw_lock<Bytes>::s_try_lock_w(int *lock, unsigned int retry /* = 3 */)
{
	rw_lock<Bytes> *oLock = (rw_lock<Bytes> *)lock;
	return oLock->try_lock_w(retry);
}

template<unsigned int Bytes>
void rw_lock<Bytes>::s_lock_r(int* lock)
{
	rw_lock<Bytes> *oLock = (rw_lock<Bytes> *)lock;
	return oLock->lock_r();
}

template<unsigned int Bytes>
void rw_lock<Bytes>::s_lock_w(int *lock)
{
	rw_lock<Bytes> *oLock = (rw_lock<Bytes> *)lock;
	return oLock->lock_w();
}

template<unsigned int Bytes>
void rw_lock<Bytes>::s_unlock_r(int *lock)
{
	rw_lock<Bytes> *oLock = (rw_lock<Bytes> *)lock;
	return oLock->unlock_r();
}

template<unsigned int Bytes>
void rw_lock<Bytes>::s_unlock_w(int *lock)
{
	rw_lock<Bytes> *oLock = (rw_lock<Bytes> *)lock;
	return oLock->unlock_w();
}

template<unsigned int Bytes>
bool rw_lock<Bytes>::try_lock_r(unsigned int uiRetry)
{
	unsigned int nRetryCount = 0;
	int nOld;

	while (true)
	{
		//When the number of retries is 0, 
		//it means to keep retrying until success.
		if ((uiRetry != 0) && (nRetryCount++ >= uiRetry))
			break;

		//read lock count +1 derictly, 
		//if there is writing operation, 
		//read lock count -1.
		nOld = inc_read();
		if (writing(nOld))
		{
			dec_read();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		//if exceeding the max read lock count, 
		//read lock count -1.
		else if (read_count(nOld) >= n_read_max_)
		{
			dec_read();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		else
		{
			return true;
		}
	}
	return false;
}

template<unsigned int Bytes>
bool rw_lock<Bytes>::try_lock_w(unsigned int uiRetry)
{
	unsigned int nRetryCount = 0;

	while (true)
	{
		//When the number of retries is 0, 
		//it means to keep retrying until success.
		if ((uiRetry != 0) && (nRetryCount++ >= uiRetry))
			break;

		//first occupy the write flag bit, 
		//if there is writing operation, loop waiting.
		if (!write_flag(true))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			continue;
		}

		//wait until all reading operations quit.
		while (true)
		{
			if ((uiRetry != 0) && (nRetryCount++ >= uiRetry))
				break;

			if (reading())
			{
#ifdef _WIN32
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
#else
				std::this_thread::sleep_for(std::chrono::microseconds(50));
#endif
				continue;
			}
			else
			{
				return true;
			}
		}

		//if all reading operations do not exiting, 
		//release write flag.
		write_flag(false);

		return false;
	}

	return false;
}

template<unsigned int Bytes>
void rw_lock<Bytes>::lock_r()
{
	rw_lock<Bytes>::try_lock_r((unsigned int)0);
}

template<unsigned int Bytes>
void rw_lock<Bytes>::lock_w()
{
	rw_lock<Bytes>::try_lock_w((unsigned int)0);
}

template<unsigned int Bytes>
void rw_lock<Bytes>::unlock_r()
{
	(void)dec_read();
}

template<unsigned int Bytes>
void rw_lock<Bytes>::unlock_w()
{
	write_flag(false);
}

//user must manage the limiting concurrent read count, 
//otherwise undefined behavior will occur
typedef rw_lock<1> RWLock;
typedef rw_lock<2> RWLock2;
typedef rw_lock<3> RWLock3;
typedef rw_lock<4> RWLock4;

}

#endif	/* __LAVA_RWLOCK_H__ */
