// rw_lock_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <string>
#include <map>
#include "rwlock.h"


std::mutex						g_mutex_;
std::map<int, int*>				g_map;
lava::RWLock					g_map_lock;
#define		COUNT			10000

void print_data(std::string tid, int data) {
	g_mutex_.lock();
	std::cout << "tid:" << tid << ", data:" << data << std::endl;
	g_mutex_.unlock();
}

void writer1() {
	std::string tid("writer1");
	std::map<int, int*>::iterator it;
	for (int i = 0; i < COUNT; i++) {
		g_map_lock.lock_r();
		it = g_map.find(i);
		if (it == g_map.end())
		{
			g_map_lock.unlock_r();
			int *p = new int(i);
			g_map_lock.lock_w();
			g_map.insert(std::pair<int, int*>(i, p));
			print_data(tid, i);
			g_map_lock.unlock_w();
		}
		else {
			g_map_lock.unlock_r();
		}
		//std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

void writer2() {
	std::string tid("writer2");
	std::map<int, int*>::iterator it;
	for (int i = 0; i < COUNT; i++) {
		g_map_lock.lock_r();
		it = g_map.find(i);
		if (it != g_map.end())
		{
			g_map_lock.unlock_r();
			g_map_lock.lock_w();
			if (it->second) {
				delete it->second;
				it->second = nullptr;
			}
			it = g_map.erase(it);
			print_data(tid, i);
			g_map_lock.unlock_w();
		}
		else {
			g_map_lock.unlock_r();
		}
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void reader1() {
	std::string tid("reader1");
	std::map<int, int*>::iterator it;
	while (true) {
		for (int i = 0; i < COUNT; i++) {
			g_map_lock.lock_r();
			it = g_map.find(i);
			if (it != g_map.end()) {
				g_map_lock.unlock_r();
				if (it->second)
					print_data(tid, *it->second);
			}
			else {
				g_map_lock.unlock_r();
			}
		}
	}
}

void reader2() {
	std::string tid("reader2");
	std::map<int, int*>::iterator it;
	while (true) {
		for (int i = 0; i < COUNT; i++) {
			g_map_lock.lock_r();
			it = g_map.find(i);
			if (it != g_map.end()) {
				g_map_lock.unlock_r();
				if (it->second)
					print_data(tid, *it->second);
			}
			else {
				g_map_lock.unlock_r();
			}
		}
	}
}

void reader3() {
	std::string tid("reader3");
	std::map<int, int*>::iterator it;
	while (true) {
		for (int i = 0; i < COUNT; i++) {
			g_map_lock.lock_r();
			it = g_map.find(i);
			if (it != g_map.end()) {
				g_map_lock.unlock_r();
				if (it->second)
					print_data(tid, *it->second);
			}
			else {
				g_map_lock.unlock_r();
			}
		}
	}
}

void reader4() {
	std::string tid("reader4");
	std::map<int, int*>::iterator it;
	while (true) {
		for (int i = 0; i < COUNT; i++) {
			g_map_lock.lock_r();
			it = g_map.find(i);
			if (it != g_map.end()) {
				g_map_lock.unlock_r();
				if (it->second)
					print_data(tid, *it->second);
			}
			else {
				g_map_lock.unlock_r();
			}
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	std::thread w1(writer1);
	std::thread w2(writer2);
	std::thread r1(reader1);
	std::thread r2(reader2);
	std::thread r3(reader3);
	std::thread r4(reader4);

	w1.join();
	w2.join();
	std::cout << "w1 and w2 end.-----------------------------------" << std::endl;
	print_data("**********************", g_map.size());

	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}

	return 0;
}

