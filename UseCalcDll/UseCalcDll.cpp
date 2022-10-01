// UseCalcDll.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <vector>

using namespace std;
typedef int(*lpZhishu)(int, int); //宏定义函数指针类型

int _tmain(int argc, _TCHAR* argv[])
{
	/*
	HINSTANCE hDll; //DLL句柄
	lpZhishu zhishu; //函数指针

	hDll = LoadLibrary(L"./CalcFunction.dll");
	if (hDll != NULL)
	{
		zhishu = (lpZhishu)GetProcAddress(hDll, "zhishu");
		int a = 0;
		int b = 0;
		while (8888 != b)
		{
			cout << "Input a:";
			cin >> a;
			cout << "Input b:";
			cin >> b;
			cout << "a ^ b = " << zhishu(a, b) << endl;
			cin.get();
		}
		FreeLibrary(hDll);
	}
	*/
	vector<int> vecTmp;
	for (int i=0; i<20; i++)
	{
		vecTmp.push_back(i);
	}

	vector<int>::iterator iter = vecTmp.begin();
	for (; iter!=vecTmp.end(); ++iter)
	{
		if ((1==(*iter)) || (3==(*iter)) || (5==(*iter)) || (7==(*iter)))
		{
			iter = vecTmp.erase(iter);
		}
	}


	system("Pause");
	return 0;
}

