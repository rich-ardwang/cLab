
// HashCalculator.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CHashCalculatorApp: 
// �йش����ʵ�֣������ HashCalculator.cpp
//

class CHashCalculatorApp : public CWinApp
{
public:
	CHashCalculatorApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CHashCalculatorApp theApp;