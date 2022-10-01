/*
* =====================================================================================
*
*       Filename:  DBManager.h
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

#ifndef _DBMANAGER_H_
#define _DBMANAGER_H_

#include <stdio.h>

#define MYSQL_OTL_ODBC
#define MYSQL_OTL_STL
#define MYSQL_OTL_UNICODE_EXCEPTION
#define MYSQL_OTL_DESTRUCTORS_DO_NOT_THROW
#define MYSQL_OTL_ODBC_SELECT_STM_EXECUTE_BEFORE_DESCRIBE

#ifdef __GNUC__
	#include <stdlib.h>
	#define OTL_ODBC_UNIX
	#define MYSQL_OTL_ODBC_UNIX
	#define OTL_BIGINT						long long
	#define WIND_OTL_BIGINT					long long
	#define MYSQL_OTL_BIGINT				long long
	#define WIND_OTL_STR_TO_BIGINT(str,n)	{ n=strtoll(str,NULL,10); }
	#define WIND_OTL_BIGINT_TO_STR(n,str)	{ sprintf(str,"%lld",n); }
#else
	#define OTL_BIGINT						__int64
	#define WIND_OTL_BIGINT					__int64
	#define MYSQL_OTL_BIGINT				__int64
	#define WIND_OTL_STR_TO_BIGINT(str,n)	{ n=_atoi64(str); }
	#define WIND_OTL_BIGINT_TO_STR(n,str)	{ _i64toa(n,str,10); }
#endif

#define MYSQL_OTL_DEFAULT_STRING_NULL_TO_VAL	""
#define MYSQL_OTL_DEFAULT_NUMERIC_NULL_TO_VAL	0
#define MYSQL_OTL_DEFAULT_CHAR_NULL_TO_VAL		0

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <queue>
#include "common/mutex.h"
#include "common/type.h"
#include "logInterface/logger.h"
#include "common/mytime.h"
#include "common/utility.h"
#include "common/dthread.h"
#include "dbInterface/mysql_otlv4.h"

using namespace std;
using namespace windcom;


#define	DB_MYSQL			0x04
#define DB_COUNT			0x03


#define DB_CATCH_EXCEPTION(error_location, bThorw)		catch(mysql_otl_exception &e) \
														{ \
															bool bDisconnect = checkCode(DB_MYSQL, e.code); \
															LOG4CXX_ERROR(GETLOGGER(error_location), "mysql otl exception : msg = " << e.msg << " errorcode = " << e.code); \
															if(bThorw) \
															{ \
																CWindDBException excep(e.msg, (unsigned char *)e.sqlstate, e.code, bDisconnect); \
																throw excep; \
															} \
														} \
														catch(CWindDBException &e) \
														{ \
															if(bThorw) \
															{ \
																throw e; \
															} \
														} \
														catch (...) \
														{ \
															LOG4CXX_ERROR(GETLOGGER(error_location), "unknown exception"); \
															if(bThorw) \
															{ \
																CWindDBException excep((unsigned char *)"unknown exception", (unsigned char *)"", -99999); \
																throw excep; \
															} \
														}


namespace windcom
{
namespace db
{
	struct TDBParamater
	{
		int		nReadType;
		int		nWriteType;
		int		nConnNum;

		int		nDataSourceTypeMysql;
		string	strDomainMysql;
		string	strNameMysql;
		string	strUserNameMysql;
		string	strPwdMysql;
		string	strDsnInfoMysql;
		string	strDriverName;

		TDBParamater()
		{
			nReadType  = DB_MYSQL;
			nWriteType = DB_MYSQL;
			nConnNum   = 10;
		}
	};

	/**
	* @brief					数据库连接定义
	*/
	struct TMYSQLConnection
	{
		/// 构造
		TMYSQLConnection()
			: m_number(-1)
			,m_bUse(false)
		{}

		/// 连接号
		int m_number;

		/// 是否正在被使用
		bool m_bUse;

		/// 连接
		mysql_otl_connect m_otlConnect;
	};
	typedef std::tr1::shared_ptr<TMYSQLConnection>	TMYSQL_CONN_PTR;

	class CWindDBException
	{
	public:
		CWindDBException(const unsigned char *message, const unsigned char *sqlst, const int ec=0, const bool dc=false);
		~CWindDBException(){};

		unsigned char msg[1000];
		unsigned char sqlstate[1000];
		int code;
		bool disconnect;
	protected:
	private:
	};

	class CDBAutoReconnect : public Thread
	{
	public:
		CDBAutoReconnect() : m_bStartReconnect(false) {}
		~CDBAutoReconnect(){}

		virtual int Run();

		void setReconnect(bool flag) { 	MutexLock lock(&m_mutex); m_bStartReconnect = flag; }
		bool getReconnect() { return m_bStartReconnect; }

	private:
		bool m_bStartReconnect;
		Mutex m_mutex;
	};

	/**
	* @brief                    数据库连接池
	*/
	class CDBManager
	{
	public:
		static CDBManager* GetInstance()
		{
			if (m_pDBMgr == NULL)
			{
                windcom::MutexLock lock(&m_mutex);
				if (m_pDBMgr == NULL)
				{
					m_pDBMgr = new CDBManager();
				}
			}
			return m_pDBMgr;
		}

    public:
		// 对外接口
		int init(const TDBParamater &dbPara);

	public:
		int		m_nReadType;
		int		m_nWriteType;

		int getMYSQLConnection(TMYSQL_CONN_PTR & conn);
		void releaseMYSQLConnection(TMYSQL_CONN_PTR &pCon);
		int reconnect();

    private:
		CDBManager() : m_nReadType(DB_MYSQL), m_nWriteType(DB_MYSQL), m_nConnectCount(10) {}
    public:
		~CDBManager()
		{
#if defined(WIND_OTL_ORA11G_R2)
		    wind_otl_connect::wind_otl_terminate();
#endif
		}

	private:
		queue<TMYSQL_CONN_PTR> m_queMYSQLConn;
        static windcom::Mutex m_mutex;

		int		m_nConnectCount;
		string	m_strMYSQLConnect;

		static CDBManager* m_pDBMgr;

		int initMYSQL(const string& dbDsnInfo, const string& dbUser, const string& dbSrvInfo, int DataSrcType, const string& dbPwd, const string& dbName, const string& driverName);
		int SetOdbcDsn(const string &strDBServer, const string &strDBName, const string &strDSN, const char *szDirverString);

		int createMYSQLConnect();
	};

	class CDBAdapter
	{
	public:
		CDBAdapter();
		~CDBAdapter();

		void setOperationType(int dbOperationType){ m_dbOperationType = dbOperationType; }

		int ExecQuery(int dbOperationType, const string sqlstm, const int buff_size=1024, const int implicit_select=mysql_otl_explicit_select);
		int DirectExec(int dbOperationType, const string sqlstm);

		int ExecRead(const string sqlstm, const int buff_size=1024, const int implicit_select=mysql_otl_explicit_select);
		int ExecWrite(const string sqlstm, const int buff_size=1024, const int implicit_select=mysql_otl_explicit_select);

		int DirectExecRead(const string sqlstm);
		int DirectExecWrite(const string sqlstm);

		int RollBack();
		int Commit();

		bool IsEnd();
		void skip_to_end_of_row();
		void close();
		void clean();
		void set_flush(const bool flush_flag=false);
		void set_commit(int auto_commit=0);
		void flush(const int rowoff=0, const bool force_flush=false);
//		void flushMysql();
		int is_null();

		int GetIntDataValue(int index = 0);
		short GetShortDataValue(int index = 0);
		string GetstringDataValue(int index = 0);
		string GetOtlstringValue(int index = 0);
		unsigned int GetDateTimeValue(int index = 0);
		unsigned char GetbyteDataValue(int index = 0);
		unsigned char GetUCharDataValue(int index = 0);

		CDBAdapter& operator<<(const char *s);
		CDBAdapter& operator<<(const unsigned u);
		CDBAdapter& operator<<(const short s);
		CDBAdapter& operator<<(const int n);
		CDBAdapter& operator<<(const string &s);
		CDBAdapter& operator<<(const mysql_otl_long_string &s);
		CDBAdapter& operator<<(const DateTime &dt);
		CDBAdapter& operator<<(const long long u);

		// added by lizhengxing
		CDBAdapter& append(const char* buf, int n);

		CDBAdapter& operator>>(int& n);
		CDBAdapter& operator>>(short& s);
		CDBAdapter& operator>>(unsigned& u);
		CDBAdapter& operator>>(char* s);
		CDBAdapter& operator>>(string &s);
		CDBAdapter& operator>>(DateTime &dt);
		CDBAdapter& operator>>(long long &u);

		mysql_otl_stream    m_otlMYSQLCur;

		TMYSQL_CONN_PTR m_pMYSQLConnection;

		int m_dbOperationType;

	private:
		bool checkCode(int dbType, int errCode);
	};
}
}

#endif
