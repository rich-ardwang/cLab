// UseCalcDll.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <vector>

using namespace std;
typedef int(*lpZhishu)(int, int); //�궨�庯��ָ������

int _tmain(int argc, _TCHAR* argv[])
{
	/*
	HINSTANCE hDll; //DLL���
	lpZhishu zhishu; //����ָ��

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

