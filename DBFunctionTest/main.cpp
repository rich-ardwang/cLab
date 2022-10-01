// DBFunctionTest.cpp : �������̨Ӧ�ó������ڵ㡣
//
#ifndef _WIND_TIME_H_
#define _WIND_TIME_H_

#include "logInterface/windlog.h"
#include "common/mytime.h"
#include "dbInterface/dbmgr.h"
#include "xmlParser/IMConfig.h"
#include <iostream>


using namespace std;
using namespace wind;
using namespace windcom;
using namespace windcom::db;


int main(int argc, char* argv[])
{
	//��ʼ����־���
	WINDLOG_INIT_DEFALUT();
	WINDLOG_INFO(("main"), "DBFunctionTest start...");

	//��ʼ����������
	CDBManager *pDBMgr = CDBManager::GetInstance();
	if(pDBMgr->init(IMCONFIG.confDB) != Success)
	{
		WINDLOG_WARN(__FUNCTION__, "db connection init failed!");
		return -1;
	}

	//����test
	CDBAdapter objDbAdapter;
	try
	{
		const string strSQL = "select * from jpword_tbl";
		if(objDbAdapter.ExecWrite(strSQL) != Success)
		{
			WINDLOG_ERROR(("main"), "Create table false!");
			return -1;
		}
		if (!objDbAdapter.IsEnd())
		{
			string jpword;
			string cx;
			string jiaming;
			string enword;
			string ciyi;
			string bookname;
			int ceci;
			int keci;
			objDbAdapter >> jpword;
			objDbAdapter >> cx;
			objDbAdapter >> jiaming;
			objDbAdapter >> enword;
			objDbAdapter >> ciyi;
			objDbAdapter >> bookname;
			objDbAdapter >> ceci;
			objDbAdapter >> keci;
			cout << jpword << ", " << cx << ", " << jiaming << ", " << enword << ", " << ciyi << ", " << bookname << ", " << ceci << ", " << keci << ".";
		}
	}
	catch(CWindDBException &p)
	{
		objDbAdapter.RollBack();
		WINDLOG_ERROR(("main"), "Create table false! " << p.msg);
		return Failure;
	}

	unsigned int nTick = 0;
	while (true)
	{
		if (++nTick % 300 == 0)
		{
			WINDLOG_INFO(("main"), "DBFunctionTest running..."); 
		}
		SSleep(60*60);
	}

	WINDLOG_INFO(("Main"), "DBFunctionTest end.");

	return 0;
}

#endif