// ThreadTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include "ThreadXLib.h"


using namespace std;

class CThread1 : public Thread::CThreadBase
{
public:
	CThread1(const char* strName) : Thread::CThreadBase(strName) { }
	virtual ~CThread1(){ }

protected:
	virtual void run_thread(void)
	{
		while (true)
		{
			cout << get_name() << endl;
			Sleep(1000);
		}
	}
};

class CThread2 : public Thread::CThreadBase
{
public:
	CThread2() : Thread::CThreadBase("Thread2") { }
	virtual ~CThread2(){ }

protected:
	virtual void run_thread(void)
	{
		while (true)
		{
			cout << "I'm thread_2." << endl;
			Sleep(1000);
		}
	}
};

class CThread3 : public Thread::CThreadBase
{
public:
	CThread3() : Thread::CThreadBase("Thread3") { }
	virtual ~CThread3(){ }

protected:
	virtual void run_thread(void)
	{
		while (true)
		{
			cout << "I'm thread_3." << endl;
			Sleep(1000);
		}
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	/*Thread::CThreadBase* pt1 = new CThread1();
	Thread::CThreadBase* pt2 = new CThread2();
	Thread::CThreadBase* pt3 = new CThread3();*/
	CThread1* pt[10000];
	for (int i=0; i<10000; i++)
	{
		ostringstream stream;
		stream << i+1;
		string str = "I'm ";
		str += stream.str();
		pt[i] = new CThread1(str.c_str());
	}
	
	for (int i=0; i<10000; i++)
	{
		pt[i]->start_thread();
	}
	

	Sleep(1000*60*60*60);
	return 0;
}

