/*
* =====================================================================================
*
*       Filename:  DBManager.cpp
*    Description:  数据库管理
*        Version:  1.0
*        Created:  2014-07-08
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangzheng
*        Company:  wind,Inc.
*
* =====================================================================================
*/
#include "dbInterface/dbmgr.h"
#include <algorithm>
#include <iostream>

#ifndef _WIN32
#include <dlfcn.h>
#include "odbcinstext.h"
#endif

#ifdef _WIN32
#pragma warning(push )
#pragma warning(disable: 4819)
#pragma warning( pop )
#endif

using namespace windcom;
using namespace windcom::db;
using namespace std;

/************************************************************************/
/*                           DBConnectionPool                           */
/************************************************************************/

CDBManager* CDBManager::m_pDBMgr = NULL;

windcom::Mutex CDBManager::m_mutex;

CDBAutoReconnect g_DBAutoReconnectThread;

typedef unsigned int (*GetCredentialFun)(char *domain, char *userId, char *buf, unsigned int bufLen);        //定义函数指针

#define  ODBC_ADD_DSN     1               // add a user DSN
#define  ODBC_ADD_SYS_DSN 4               // add a system DSN
#ifdef _WIN32
extern "C" BOOL __stdcall SQLConfigDataSource(HWND hwndParent, WORD fRequest, LPCSTR lpszDriver, LPCSTR lpszAttributes);
#endif

CWindDBException::CWindDBException(const unsigned char *message, const unsigned char *sqlst, const int ec, const bool dc)
{
	memset(msg, 0, sizeof(msg));
	memcpy(msg, message, strlen((char *)message));
	memset(sqlstate, 0, sizeof(sqlstate));
	memcpy(sqlstate, sqlst, strlen((char *)sqlst));
	code = ec;
	disconnect = dc;
}

/**
* @brief                                 初始化
* @param size                            初始化连接数量
* @retval                                <0 失败  =0成功
*/
int CDBManager::init(const TDBParamater &dbPara)
{
	m_nReadType = dbPara.nReadType;
	m_nWriteType = dbPara.nWriteType;
	m_nConnectCount = dbPara.nConnNum;

	if(m_nReadType & DB_MYSQL || m_nWriteType & DB_MYSQL)
	{
		mysql_otl_connect::mysql_otl_initialize(1);

		if(initMYSQL(dbPara.strDsnInfoMysql, dbPara.strUserNameMysql, dbPara.strDomainMysql, dbPara.nDataSourceTypeMysql, dbPara.strPwdMysql, dbPara.strNameMysql, dbPara.strDriverName) == Failure)
		{
			return Failure;
		}

		if(createMYSQLConnect() == Failure)
		{
			return Failure;
		}

		LOG4CXX_INFO(GETLOGGER("CDBManager::init"), "init MYSQL ok");
	}

	g_DBAutoReconnectThread.Start();

	return Success;
}

int CDBManager::initMYSQL(const string& dbDsnInfo, const string& dbUser, const string& dbSrvInfo, int DataSrcType, const string& dbPwd, const string& dbName, const string& driverName)
{
	//需要配置ODBC信息
	if (SetOdbcDsn(dbSrvInfo, dbName, dbDsnInfo, driverName.c_str()) != Success)
	{
		//无法配置数据库连接
		LOG4CXX_ERROR(GETLOGGER("CDBManager::initSQLServer"), "can not set dsn = " << dbDsnInfo << " srvinfo = " << dbSrvInfo);
		return Failure;
	}

	//Mysql链接信息
	m_strMYSQLConnect = dbUser + "/" + dbPwd + "@" + dbDsnInfo;

	return Success;
}

int CDBManager::SetOdbcDsn(const string &strDBServer, const string &strDBName, const string &strDSN, const char *szDirverString)
{
	int iRet = Success;

#ifdef _WIN32
	string dsn = "DSN="+strDSN+"\\DESCRIPTION=MySQL"+"\\SERVER="+strDBServer+"\\DATABASE="+strDBName+"\\";
	std::replace(dsn.begin(), dsn.end(), '\\', '\0');
	std::replace(dsn.begin(), dsn.end(), '/', '\\');

	iRet = SQLConfigDataSource(NULL, ODBC_ADD_DSN, szDirverString, dsn.c_str());
	if (1 == iRet)
	{
		iRet = Success;
	}
	else
	{
		iRet = Failure;
	}
#endif

	return iRet;
}

int CDBManager::getMYSQLConnection(TMYSQL_CONN_PTR & conn)
{
	if(g_DBAutoReconnectThread.getReconnect())
	{
		return Failure;
	}

	if (!m_queMYSQLConn.empty())
	{
		windcom::MutexLock lock(&m_mutex);
		conn = m_queMYSQLConn.front();
		m_queMYSQLConn.pop();
	}

	if(!conn.get())
	{
		LOG4CXX_ERROR(GETLOGGER("CDBManager::getMYSQLConnection"), "db pool is empty" );
		return Failure;
	}

	return Success;
}

/// 释放连接
void CDBManager::releaseMYSQLConnection(TMYSQL_CONN_PTR &pCon)
{
	windcom::MutexLock lock(&m_mutex);
	m_queMYSQLConn.push(pCon);
}

int CDBManager::createMYSQLConnect()
{
	for (int i=0; i<m_nConnectCount; i++)
	{
		TMYSQL_CONN_PTR pCon(new TMYSQLConnection());
		pCon->m_number = i;

		try
		{
			pCon->m_otlConnect.rlogon(m_strMYSQLConnect.c_str());
			pCon->m_otlConnect.auto_commit_off();
		}
		catch(mysql_otl_exception& e)
		{
			LOG4CXX_ERROR(GETLOGGER("CDBManager::createMYSQLConnect"), "mysql otl exception : msg = " << e.msg << " errorcode = " << e.code);
			return Failure;
		}

		MutexLock lock(&m_mutex);
		m_queMYSQLConn.push(pCon);
	}

	return Success;
}

int CDBManager::reconnect()
{
	MutexLock lock(&m_mutex);

	if((m_nReadType & DB_MYSQL ||  m_nWriteType & DB_MYSQL) && (int)m_queMYSQLConn.size() < m_nConnectCount)
	{
		return Failure;
	}

	if(m_nReadType & DB_MYSQL || m_nWriteType & DB_MYSQL)
	{
		try
		{
			for(int i=0; i<m_nConnectCount; i++)
			{
				TMYSQL_CONN_PTR pCon = m_queMYSQLConn.front();
				try
				{
					pCon->m_otlConnect.logoff();
				}
				catch(mysql_otl_exception& e)
				{
					LOG4CXX_ERROR(GETLOGGER("CDBManager::reconnect"), "mysql otl exception : msg = " << e.msg << " errorcode = " << e.code);
				}
				pCon->m_otlConnect.rlogon(m_strMYSQLConnect.c_str());
				pCon->m_otlConnect.auto_commit_off();
				m_queMYSQLConn.pop();
				m_queMYSQLConn.push(pCon);
			}
		}
		catch(mysql_otl_exception& e)
		{
			LOG4CXX_ERROR(GETLOGGER("CDBManager::reconnect"), "mysql otl exception : msg = " << e.msg << " errorcode = " << e.code);
			return Failure;
		}
	}

	LOG4CXX_INFO(GETLOGGER("CDBManager::reconnect"), "reconnect to db ok");

	return Success;
}


CDBAdapter::CDBAdapter()
{

}

CDBAdapter::~CDBAdapter()
{
	if(m_pMYSQLConnection.get())
	{
		//m_otlMYSQLCur.close();
		CDBManager::GetInstance()->releaseMYSQLConnection(m_pMYSQLConnection);
	}
}

int CDBAdapter::RollBack()
{
	try
	{
		if((m_dbOperationType & DB_MYSQL) && m_pMYSQLConnection.get())
		{
			m_pMYSQLConnection->m_otlConnect.direct_exec("rollback;");
		}

		return Success;
	}
	DB_CATCH_EXCEPTION("CDBAdapter::RollBack", false);

	return Failure;
}

int CDBAdapter::Commit()
{
	try
	{
		if((m_dbOperationType & DB_MYSQL) && m_pMYSQLConnection.get())
		{
			m_pMYSQLConnection->m_otlConnect.direct_exec("commit;");
		}

		return Success;
	}
	DB_CATCH_EXCEPTION("CDBAdapter::Commit", true);

	return Failure;
}

bool CDBAdapter::IsEnd()
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			return m_otlMYSQLCur.eof()==0 ? false : true;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::IsEnd", true);

	return false;
}

void CDBAdapter::skip_to_end_of_row()
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			m_otlMYSQLCur.skip_to_end_of_row();
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::skip_to_end_of_row", true);
}

void CDBAdapter::close()
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			m_otlMYSQLCur.close();
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::close", true);
}

void CDBAdapter::clean()
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			m_otlMYSQLCur.clean();
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::clean", true);
}

void CDBAdapter::set_flush(const bool flush_flag/* =false */)
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			m_otlMYSQLCur.set_flush(flush_flag);
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::set_flush", true);
}

void CDBAdapter::set_commit(int auto_commit/* =0 */)
{
	try
	{
		if(auto_commit)
		{
			if(m_dbOperationType & DB_MYSQL)
			{
				m_otlMYSQLCur.set_commit(auto_commit);
//				m_pMYSQLConnection->m_otlConnect.auto_commit_on();
			}
		}
		else
		{
			if(m_dbOperationType & DB_MYSQL)
			{
				m_otlMYSQLCur.set_commit(auto_commit);
//				m_pMYSQLConnection->m_otlConnect.auto_commit_off();
				m_pMYSQLConnection->m_otlConnect.direct_exec("begin;");
			}
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::set_commit", true);
}

void CDBAdapter::flush(const int rowoff/* =0 */, const bool force_flush/* =false */)
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			m_otlMYSQLCur.flush();
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::flush", true);
}

#if 0
void CDBAdapter::flushMysql()
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			m_otlMYSQLCur.flush();
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::flushMysql", true);
}
#endif

int CDBAdapter::is_null()
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			return m_otlMYSQLCur.is_null();
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::is_null", true);

	return 0;
}

int CDBAdapter::DirectExec(int dbOperationType, const string sqlstm)
{
	if(dbOperationType != DB_MYSQL)
	{
		LOG4CXX_ERROR(GETLOGGER("CDBAdapter::Direct_Exec"), "db operation type error: " << dbOperationType);
		return Failure;
	}

	CDBManager *pDBMgr = CDBManager::GetInstance();
	if(!pDBMgr)
	{
		LOG4CXX_ERROR(GETLOGGER("CDBAdapter::Direct_Exec"), "CDBManager::GetInstance failed");
		return Failure;
	}

	m_dbOperationType = dbOperationType;

	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			if(!m_pMYSQLConnection.get())
			{
				pDBMgr->getMYSQLConnection(m_pMYSQLConnection);
				if(!m_pMYSQLConnection.get()) return Failure;
			}

			LOG4CXX_DEBUG(GETLOGGER("CDBAdapter::Direct_Exec mysql"), sqlstm.c_str());
			m_pMYSQLConnection->m_otlConnect.direct_exec(sqlstm.c_str());
		}

		return Success;
	}
	DB_CATCH_EXCEPTION("CDBAdapter::Direct_Exec", true);

	return Failure;
}

int CDBAdapter::DirectExecRead(const string sqlstm)
{
	CDBManager *pDBMgr = CDBManager::GetInstance();
	if(!pDBMgr)
	{
		LOG4CXX_ERROR(GETLOGGER("CDBAdapter::DirectExecRead"), "CDBManager::GetInstance failed");
		return Failure;
	}

	int nRet = Failure;

	try
	{
		nRet =  DirectExec(pDBMgr->m_nReadType, sqlstm);
	}
	DB_CATCH_EXCEPTION("CDBAdapter::DirectExecRead", true);

	return nRet;
}

int CDBAdapter::DirectExecWrite(const string sqlstm)
{
	CDBManager *pDBMgr = CDBManager::GetInstance();
	if(!pDBMgr)
	{
		LOG4CXX_ERROR(GETLOGGER("CDBAdapter::DirectExecWrite"), "CDBManager::GetInstance failed");
		return Failure;
	}

	int nRet = Failure;

	try
	{
		nRet = DirectExec(pDBMgr->m_nWriteType, sqlstm);
	}
	DB_CATCH_EXCEPTION("CDBAdapter::DirectExecWrite", true);

	return nRet;
}

int CDBAdapter::ExecQuery(int dbOperationType, const string sqlstm, const int buff_size, const int implicit_select)
{
	if(dbOperationType != DB_MYSQL)
	{
		LOG4CXX_ERROR(GETLOGGER("CDBAdapter::ExecQuery"), "db operation type error: " << dbOperationType);
		return Failure;
	}

	CDBManager *pDBMgr = CDBManager::GetInstance();
	if(!pDBMgr)
	{
		LOG4CXX_ERROR(GETLOGGER("CDBAdapter::ExecQuery"), "CDBManager::GetInstance failed");
		return Failure;
	}

	m_dbOperationType = dbOperationType;

	try
	{
		close();

		if(m_dbOperationType & DB_MYSQL)
		{
			if(!m_pMYSQLConnection.get())
			{
				pDBMgr->getMYSQLConnection(m_pMYSQLConnection);
				if(!m_pMYSQLConnection.get()) return Failure;
			}

			//LOG4CXX_DEBUG(GETLOGGER("CDBAdapter::ExecQuery mysql"), sqlstm[DB_MYSQL_IDX].c_str());
			m_otlMYSQLCur.open(buff_size, sqlstm.c_str(), m_pMYSQLConnection->m_otlConnect, implicit_select);
		}

//		set_flush(false);

		return Success;
	}
	DB_CATCH_EXCEPTION("CDBAdapter::ExecQuery", true);

	return Failure;
}

int CDBAdapter::ExecRead(const string sqlstm, const int buff_size, const int implicit_select)
{
	CDBManager *pDBMgr = CDBManager::GetInstance();
	if(!pDBMgr)
	{
		LOG4CXX_ERROR(GETLOGGER("CDBAdapter::ExecRead"), "CDBManager::GetInstance failed");
		return Failure;
	}

	int nRet = Failure;

	try
	{
		nRet = ExecQuery(pDBMgr->m_nReadType, sqlstm, buff_size, implicit_select);
	}
	DB_CATCH_EXCEPTION("CDBAdapter::ExecRead", true);

	return nRet;
}

int CDBAdapter::ExecWrite(const string sqlstm, const int buff_size, const int implicit_select)
{
	CDBManager *pDBMgr = CDBManager::GetInstance();
	if(!pDBMgr)
	{
		LOG4CXX_ERROR(GETLOGGER("CDBAdapter::ExecWrite"), "CDBManager::GetInstance failed");
		return Failure;
	}

	int nRet = Failure;

	try
	{
		nRet = ExecQuery(pDBMgr->m_nWriteType, sqlstm, buff_size, implicit_select);
	}
	DB_CATCH_EXCEPTION("CDBAdapter::ExecWrite", true);

	return nRet;
}

int CDBAdapter::GetIntDataValue(int index /* = 0 */)
{
	int nValue = 0;

	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			if(!m_otlMYSQLCur.eof())
				m_otlMYSQLCur >> nValue;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::GetIntDataValue", true);

	windcom::util::Reverse(nValue);
	return nValue;
}

short CDBAdapter::GetShortDataValue(int index /* = 0 */)
{
	short nValue = 0;

	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			if(!m_otlMYSQLCur.eof())
				m_otlMYSQLCur >> nValue;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::GetShortDataValue", true);

	windcom::util::Reverse(nValue);
	return nValue;
}

string CDBAdapter::GetstringDataValue(int index /* = 0 */)
{
	char cBuf[5000] = {0};

	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			if(!m_otlMYSQLCur.eof())
				m_otlMYSQLCur >> cBuf;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::GetstringDataValue", true);

	return cBuf;
}

string CDBAdapter::GetOtlstringValue(int index /* = 0 */)
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			char cBuf[5000] = {0};
			if(!m_otlMYSQLCur.eof())
				m_otlMYSQLCur >> cBuf;
			return cBuf;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::GetOtlstringValue", true);

	string strData = "";
	return strData;
}

unsigned int CDBAdapter::GetDateTimeValue(int index /* = 0 */)
{
	TinyDateTime dt;

	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			mysql_otl_datetime odt;
			if(!m_otlMYSQLCur.eof())
				m_otlMYSQLCur >> odt;
			dt.SetTime(odt.year,odt.month,odt.day,odt.hour,odt.minute,odt.second,0);
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::GetDateTimeValue", true);

	return dt.ToInt();
}

unsigned char CDBAdapter::GetbyteDataValue(int index /* = 0 */)
{
	short cBuf = 0;

	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			if(!m_otlMYSQLCur.eof())
				m_otlMYSQLCur >> cBuf;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::GetbyteDataValue", true);

	return (unsigned char)cBuf;
}

unsigned char CDBAdapter::GetUCharDataValue(int index /* = 0 */)
{
	unsigned char cBuf = 0;

	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			if(!m_otlMYSQLCur.eof())
				m_otlMYSQLCur >> cBuf;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::GetucharDataValue", true);

	return cBuf;
}

CDBAdapter& CDBAdapter::operator<<(const char *s)
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			m_otlMYSQLCur << s;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::operator<<(const char *s)", true);

	return *this;
}

CDBAdapter& CDBAdapter::operator<<(const unsigned u)
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			m_otlMYSQLCur << u;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::operator<<(const unsigned u)", true);

	return *this;
}

CDBAdapter& CDBAdapter::operator<<(const short s)
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			m_otlMYSQLCur << s;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::operator<<(const short s)", true);

	return *this;
}

CDBAdapter& CDBAdapter::operator<<(const int n)
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			m_otlMYSQLCur << n;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::operator<<(const int n)", true);

	return *this;
}

CDBAdapter& CDBAdapter::append(const char* buf, int n)
{
	std::string s(buf, n);
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			m_otlMYSQLCur << s;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::operator<<(const string &s)", true);
	return *this;
}

CDBAdapter& CDBAdapter::operator<<(const string &s)
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			m_otlMYSQLCur << s;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::operator<<(const string &s)", true);

	return *this;
}

CDBAdapter& CDBAdapter::operator<<(const mysql_otl_long_string &s)
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			string strData((const char*)s.v, s.len());
			m_otlMYSQLCur << strData.c_str();
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::operator<<(const mysql_otl_long_string &s)", true);

	return *this;
}

CDBAdapter& CDBAdapter::operator<<(const DateTime &dt)
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			mysql_otl_datetime datetime;
			datetime.year = dt.Year();
			datetime.month = dt.Month();
			datetime.day = dt.Day();
			datetime.hour = dt.Hour();
			datetime.minute = dt.Minute();
			datetime.second = dt.Second();
			datetime.fraction = dt.MSecond()*1000*1000;

			m_otlMYSQLCur << datetime;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::operator<<(const DateTime &dt)", true);

	return *this;
}

CDBAdapter& CDBAdapter::operator<<(const long long u)
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			m_otlMYSQLCur << u;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::operator<<(const long long u)", true);

	return *this;
}

CDBAdapter& CDBAdapter::operator>>(long long &u)
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			m_otlMYSQLCur >> u;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::operator>>(long long &u)", true);

	return *this;
}

CDBAdapter& CDBAdapter::operator>>(int& n)
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			m_otlMYSQLCur >> n;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::operator>>(int& n)", true);

	return *this;
}

CDBAdapter& CDBAdapter::operator>>(short& s)
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			m_otlMYSQLCur >> s;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::operator>>(short& s)", true);

	return *this;
}

CDBAdapter& CDBAdapter::operator>>(unsigned& u)
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			m_otlMYSQLCur >> u;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::operator>>(unsigned& u)", true);

	return *this;
}

CDBAdapter& CDBAdapter::operator>>(char* s)
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			m_otlMYSQLCur >> s;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::operator>>(char* s)", true);

	return *this;
}

CDBAdapter& CDBAdapter::operator>>(string &s)
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			m_otlMYSQLCur >> s;
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::operator>>(string &s)", true);

	return *this;
}

CDBAdapter& CDBAdapter::operator>>(DateTime &dt)
{
	try
	{
		if(m_dbOperationType & DB_MYSQL)
		{
			mysql_otl_datetime odt;
			if(!m_otlMYSQLCur.eof())
				m_otlMYSQLCur >> odt;
			dt.SetTime(odt.year,odt.month,odt.day,odt.hour,odt.minute,odt.second,odt.fraction/(1000*1000));
		}
	}
	DB_CATCH_EXCEPTION("CDBAdapter::operator>>(DateTime &dt)", true);

	return *this;
}

bool CDBAdapter::checkCode(int dbType, int errCode)
{
	bool bDisconnect = false;

	switch(dbType)
	{
	case DB_MYSQL:
		if(errCode == 2006 || errCode == 2013)
		{
			bDisconnect = true;
		}
		break;

	default:break;
	}

	if(bDisconnect)
	{
		g_DBAutoReconnectThread.setReconnect(true);
	}

	return bDisconnect;
}

int CDBAutoReconnect::Run()
{
	while(true)
	{
		SSleep(3);
		if(m_bStartReconnect)
		{
			LOG4CXX_DEBUG(GETLOGGER("CDBAutoReconnect::Run"), "try reconnect to db");

			CDBManager *pDBMgr = CDBManager::GetInstance();
			if(!pDBMgr)
			{
				LOG4CXX_ERROR(GETLOGGER("CDBAutoReconnect::Run"), "CDBManager::GetInstance failed");
				continue;
			}

			if(pDBMgr->reconnect() == Success)
			{
				setReconnect(false);
			}
		}
	}

	return 0;
}

