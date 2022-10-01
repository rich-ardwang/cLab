#ifndef _WIND_LOG_IMPLS_H_
#define _WIND_LOG_IMPLS_H_

#include "windlogif.h"
#include "logger.h"
#include "windlog.h"

using namespace std;

namespace windcom
{
namespace log
{
	class Log4CxxImpl : public IWindLog
	{
	public:
		Log4CxxImpl(){}
		virtual ~Log4CxxImpl(){}

	public:
		virtual void DoLog(int level, const char* logger, const string& msg, void* data = NULL)
		{
			switch(level)
			{
			case LEVEL_DEBUG: 
				if (LOG4CXX_UNLIKELY(log4cxx::Logger::getLogger(logger)->isDebugEnabled()))
				{
					log4cxx::Logger::getLogger(logger)->forcedLog(::log4cxx::Level::getDebug(), msg, LOG4CXX_LOCATION); 
				}
				break;
			case LEVEL_INFO:
			case LEVEL_NOTICE:
				if (LOG4CXX_UNLIKELY(log4cxx::Logger::getLogger(logger)->isInfoEnabled()))
				{
					log4cxx::Logger::getLogger(logger)->forcedLog(::log4cxx::Level::getInfo(), msg, LOG4CXX_LOCATION); 
				}
				break;
			case LEVEL_WARN:
				if (LOG4CXX_UNLIKELY(log4cxx::Logger::getLogger(logger)->isWarnEnabled()))
				{
					log4cxx::Logger::getLogger(logger)->forcedLog(::log4cxx::Level::getWarn(), msg, LOG4CXX_LOCATION); 
				}
				break;
			case LEVEL_ERROR:
				if (LOG4CXX_UNLIKELY(log4cxx::Logger::getLogger(logger)->isErrorEnabled()))
				{
					log4cxx::Logger::getLogger(logger)->forcedLog(::log4cxx::Level::getError(), msg, LOG4CXX_LOCATION); 
				}
				break;
			case LEVEL_FATAL:
				if (LOG4CXX_UNLIKELY(log4cxx::Logger::getLogger(logger)->isFatalEnabled()))
				{
					log4cxx::Logger::getLogger(logger)->forcedLog(::log4cxx::Level::getFatal(), msg, LOG4CXX_LOCATION); 
				}
				break;
			default:
				break;
			}
		}
	};


	// 分布式日志系统操作类
	class CDSLog
	{
	public:
		CDSLog();
		~CDSLog();

		// 构造Head格式 __LINE__, __FILE__
		// 格式预留，暂采用固定格式
		static void GenerateHead(const char* p, string& str, const char* format = NULL);
		// 直接调用SendInfo接口上传日志
		static void SendLog(unsigned char ucType, const char* logger, const string& msg, unsigned short usMsgId = 500);
	};

	class CDSLogImpl : public IWindLog
	{
	public:
		CDSLogImpl(){}
		~CDSLogImpl(){}

		virtual void DoLog(int level, const char* logger, const string& msg, void* data = NULL)
		{
			switch(level)
			{
			case LEVEL_DEBUG: 
				CDSLog::SendLog('D', logger, msg); break;
			case LEVEL_INFO:
				CDSLog::SendLog('I', logger, msg); break;
			case LEVEL_NOTICE:
				CDSLog::SendLog('N', logger, msg); break;
			case LEVEL_WARN:
				CDSLog::SendLog('W', logger, msg); break;
			case LEVEL_ERROR:
				CDSLog::SendLog('E', logger, msg); break;
			case LEVEL_FATAL:
				CDSLog::SendLog('F', logger, msg); break;
			case LEVEL_CUSTOM:
				CDSLog::SendLog((unsigned char)(unsigned long long)data,logger, msg); break;
			default:
				break;
			}
		}
	};

	
}
}



#endif
