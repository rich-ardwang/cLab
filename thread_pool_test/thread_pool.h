#pragma once
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace std
{
#define MAX_THREAD_NUM			256
#define DEFAULT_THREAD_NUM		10

/* thread pool
 * we can commit the variable params function or the lambda function to operate,
 * and can get the return value.
 * it not support the class member function, but support the class static
 * member function and support the global function, Opteron() etc.
*/
class threadpool {
	using Task = std::function<void()>;

public:
	inline threadpool(unsigned short size = DEFAULT_THREAD_NUM) : stoped_ { false } {
		idl_thr_num_ = size < 1 ? 1 : size;
		for (size = 0; size < idl_thr_num_; ++size) {
			pool_.emplace_back(
				[this] {
					//worker thread function
					while (!this->stoped_) {
						std::function<void()> task;
						{	//get a task to operate
							//unique_lock is better than lock_guard: it can unlock() and lock() at any time
							std::unique_lock<std::mutex> lock { this->mutex_lock_ };
							this->cv_task_.wait(lock,
								[this] {
									return (this->stoped_.load() || (!this->tasks_.empty()));
								}
							);	//wait until get a task
							if (this->stoped_ && this->tasks_.empty()) {
								return;
							}
							task = std::move(this->tasks_.front());	//get one task
							this->tasks_.pop();
						}
						idl_thr_num_--;
						task();
						idl_thr_num_++;
					}
				}
			);
		}
	}

	inline ~threadpool() {
		stoped_.store(true);
		cv_task_.notify_all();	//notify all thread to operate tasks
		for (std::thread &thread : pool_) {
			//thread.detach();	//let threads emerge or perish of itself
			if (thread.joinable()) {
				thread.join();	//waiting..., threads must complete all tasks.
			}
		}
	}

public:
	/*
	 * commit one task
	 * if call .get() function, geting return value when task complete.
	 * Two functions:
	 *	-one is using bind: .commit(std::bind(&Dog::sayHello, &dog));
	 *	-another is to use mem_fn: .commit(std::mem_fn(&Dog::sayHello), &dog)
	*/
	template<class F, class... Args>
	auto commit(F &&f, Args &&... args)->std::future<decltype(f(args...))> {
		if (stoped_.load()) {	//stop == true ?
			throw std::runtime_error("commiting on thread pool is stopped.");
		}

		using RetType = decltype(f(args...));	//typename std::result_of<F(Args...)>::type is return value of function f
		auto task = std::make_shared<std::packaged_task<RetType()> >(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);
		std::future<RetType> future = task->get_future();
		{	//add tasks to the queue
			//to lock the block, lock_guard is packaging class of mutex stack,
			//when construction is lock(), and when destruction is unlock()
			std::lock_guard<std::mutex> lock{ mutex_lock_ };
			tasks_.emplace(
				[task]() {	//push(Task{...})
					(*task)();
				}
			);
		}
		cv_task_.notify_one(); //notify one thread to operate the task
		return future;
	}

	//return the idle thread count
	int idlCount() { return idl_thr_num_; }

private:
	//thread pool
	std::vector<std::thread> pool_;
	//task queue
	std::queue<Task> tasks_;
	//using for synchronization
	std::mutex mutex_lock_;
	//condition block
	std::condition_variable cv_task_;
	//the flag is to open commit or not
	std::atomic<bool> stoped_;
	//idle thread count
	std::atomic<int> idl_thr_num_;
};

}

#endif
