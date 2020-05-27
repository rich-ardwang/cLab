//
//  timer.h
//
//  Created by Colin on 2019-08-05.
//  Copyright (c) 2019 Sumscope. All rights reserved.
//
#pragma once
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include "lava_base.h"

namespace lava
{
	namespace utils
	{
		class timer
		{
		public:
			timer() : m_expired(true), m_try_to_expire(false) {}

			timer(const timer& t) 
			{
				m_expired = t.m_expired.load();
				m_try_to_expire = t.m_try_to_expire.load();
			}

			~timer() 
			{
				expire();
			}

			void start(int interval, std::function<void()> task) 
			{
				if ( m_expired == false )
					return;

				m_expired = false;
				std::thread([this, interval, task]()
				{
					while ( !m_try_to_expire ) 
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(interval));
						task();
					}
					{
						std::lock_guard<std::mutex> locker(m_mutex);
						m_expired = true;
						m_expired_cond.notify_one();
					}
				}).detach();
			}

			void expire()
			{
				if ( m_expired || m_try_to_expire )
					return;

				m_try_to_expire = true;
				{
					std::unique_lock<std::mutex> locker(m_mutex);
					m_expired_cond.wait(locker, [this] { return m_expired == true; });
					m_try_to_expire = false;
				}
			}

			template<typename callable, class... arguments>
			void sync_wait(int after, callable&& f, arguments&& ... args) 
			{
				std::function<typename std::result_of<callable(arguments...)>::type()>
				task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
				std::this_thread::sleep_for(std::chrono::milliseconds(after));
				task();
			}

			template<typename callable, typename... arguments>
			void async_wait(int after, callable&& f, arguments&& ... args) 
			{
				std::function<typename std::result_of<callable(arguments...)>::type()> 
				task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

				std::thread([after, task]() 
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(after));
					task();
				}).detach();
			}

		private:
			std::atomic<bool>		m_expired;
			std::atomic<bool>		m_try_to_expire;
			std::mutex				m_mutex;
			std::condition_variable m_expired_cond;
		};
	}
}