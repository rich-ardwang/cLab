// threadPoolTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "thread_pool.h"
#include <iostream>
#include <string>
#include "vld.h"

std::mutex      g_mutex_lock_;

void fun1(int slp) {
	printf(" hello, fun1 ! %d\n", std::this_thread::get_id());
	if (slp > 0) {
		printf(" ======= fun1 sleep %d ========= %d\n", slp, std::this_thread::get_id());
		std::this_thread::sleep_for(std::chrono::milliseconds(slp));
	}
}

struct gfun {
	int operator()(int n) {
		printf("%d hello, gfun ! %d\n", n, std::this_thread::get_id());
		return 42;
	}
};

void test_msg_order(int number) {
    std::lock_guard<std::mutex> lock(g_mutex_lock_);
    std::cout << "thread id : " << std::this_thread::get_id()
        << "msg number : " << number << std::endl;
}

class A {
public:
	static int Afun(int n = 0) {  //函数必须是 static 的才能直接使用线程池
		std::cout << n << " hello, Afun ! " << std::this_thread::get_id() << std::endl;
		return n;
	}

	static std::string Bfun(int n, std::string str, char c) {
		std::cout << n << " hello, Bfun ! " << str.c_str() << " " << (int)c << " " << std::this_thread::get_id() << std::endl;
		return str;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	/*
	try {
		std::threadpool executor{ 100 };
		A a;
		std::future<void> ff = executor.commit(fun1, 0);
		std::future<int> fg = executor.commit(gfun{}, 0);
		std::future<int> gg = executor.commit(a.Afun, 9999); //IDE提示错误,但可以编译运行
		std::future<std::string> gh = executor.commit(A::Bfun, 9998, "mult args", 123);
		std::future<std::string> fh = executor.commit([]()->std::string { std::cout << "hello, fh ! " << std::this_thread::get_id() << std::endl; return "hello,fh ret !"; });

		std::cout << " ======= sleep ========= " << std::this_thread::get_id() << std::endl;
		std::this_thread::sleep_for(std::chrono::microseconds(900));

		for (int i = 0; i < 50; i++) {
			executor.commit(fun1, i * 100);
		}
		std::cout << " ======= commit all ========= " << std::this_thread::get_id() << " idlsize=" << executor.idlCount() << std::endl;

		std::cout << " ======= sleep ========= " << std::this_thread::get_id() << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(3));

		ff.get(); //调用.get()获取返回值会等待线程执行完,获取返回值
		std::cout << fg.get() << " " << fh.get().c_str() << " " << std::this_thread::get_id() << std::endl;

		std::cout << " ======= sleep ========= " << std::this_thread::get_id() << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(3));

		std::cout << " ======= fun1,55 ========= " << std::this_thread::get_id() << std::endl;
		executor.commit(fun1, 55).get();  //调用.get()获取返回值会等待线程执行完

		std::cout << "end... " << std::this_thread::get_id() << std::endl;


		std::threadpool pool(4);
		std::vector< std::future<int> > results;

		for (int i = 0; i < 8; ++i) {
			results.emplace_back(
				pool.commit([i] {
				std::cout << "hello " << i << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(1));
				std::cout << "world " << i << std::endl;
				return i*i;
				})
			);
		}
		std::cout << " ======= commit all2 ========= " << std::this_thread::get_id() << std::endl;
		for (auto && result : results) {
			std::cout << result.get() << ' ';
		}
		std::cout << std::endl;
	}
	catch (std::exception& e) {
		std::cout << "something unhappy happened... " << std::this_thread::get_id() << e.what() << std::endl;
	}
	*/

    /*
	std::string *ps = new std::string("hello world.");
	std::string ss = "123456_";
	std::cout << ss << std::endl;
	std::cout << ps << std::endl;
	std::cout << (*ps) << std::endl;
	ss += reinterpret_cast<long>(ps);
	std::cout << reinterpret_cast<long>(ps) << std::endl;
	std::cout << ps << std::endl;
    */

    //test message order
    std::threadpool executor{ 10 }; //use 10 threads in the pool
    for (int i = 0; i < 100; i++) {
        executor.commit(test_msg_order, i);
    }

    while (true) {
        if (10 == executor.idlCount())
            break;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

	std::cout << "input enter key to exit..." << std::endl;
	std::cin.get();
	return 0;
}

