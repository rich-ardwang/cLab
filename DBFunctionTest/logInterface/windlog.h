#ifndef _WIND_LOG_H_
#define _WIND_LOG_H_

#include <string>
#include <vector>
#include <cstdio>

/*------------------------------------对外接口-----------------------------------*/

// 初始化日志组件接口 使用默认配置文件名
#define WINDLOG_INIT_DEFALUT(){ \
	windcom::log::WindLog::Init(); \
}

// 初始化日志组件接口 使用自定义配置文件名
#define WINDLOG_INIT(config) { \
	windcom::log::WindLog::Init(config); \
}

#if 0
// 记录DEBUG级别日志
#define WINDLOG_DEBUG(logger, msg) 

// 记录INFO级别日志
#define WINDLOG_INFO(logger, msg) 

// 记录WARN级别日志
#define WINDLOG_WARN(logger,msg)

// 记录ERROR级别日志
#define WINDLOG_ERROR(logger, msg)

// NOTICE 解除警报
#define WINDLOG_NOTICE(logger, msg) 

// 自定义
#define WINDLOG_CUSTOM(logger, msg, msgType) 

#else


// 记录DEBUG级别日志
#define WINDLOG_DEBUG(logger, msg) { \
    windcom::log::CNewLog oss_; oss_ << windcom::log::TrimFile(__FILE__, sizeof(__FILE__), __LINE__) << msg; windcom::log::WindLog::forcedLog(windcom::log::LEVEL_DEBUG, logger, oss_.getBuf()); \
}

// 记录INFO级别日志
#define WINDLOG_INFO(logger, msg) { \
    windcom::log::CNewLog oss_; oss_ << windcom::log::TrimFile(__FILE__, sizeof(__FILE__), __LINE__) << msg; windcom::log::WindLog::forcedLog(windcom::log::LEVEL_INFO, logger, oss_.getBuf()); \
}

// 记录WARN级别日志
#define WINDLOG_WARN(logger,msg) { \
	windcom::log::CNewLog oss_; oss_ << windcom::log::TrimFile(__FILE__, sizeof(__FILE__), __LINE__) << msg; windcom::log::WindLog::forcedLog(windcom::log::LEVEL_WARN, logger, oss_.getBuf()); \
}

// 记录ERROR级别日志
#define WINDLOG_ERROR(logger, msg) { \
	windcom::log::CNewLog oss_; oss_ << windcom::log::TrimFile(__FILE__, sizeof(__FILE__), __LINE__) << msg; windcom::log::WindLog::forcedLog(windcom::log::LEVEL_ERROR, logger, oss_.getBuf()); \
}

// NOTICE 解除警报
#define WINDLOG_NOTICE(logger, msg) { \
	windcom::log::CNewLog oss_; oss_ << windcom::log::TrimFile(__FILE__, sizeof(__FILE__), __LINE__) << msg; windcom::log::WindLog::forcedLog(windcom::log::LEVEL_NOTICE, logger, oss_.getBuf()); \
}

// 自定义
#define WINDLOG_CUSTOM(logger, msg, msgType) { \
	windcom::log::CNewLog oss_; oss_ << windcom::log::TrimFile(__FILE__, sizeof(__FILE__), __LINE__) <<  msg; windcom::log::WindLog::forcedLog(windcom::log::LEVEL_CUSTOM, logger, oss_.getBuf(), (void*)msgType); \
}
#endif

/*-------------------------------------------------------------------------------*/

namespace windcom
{
namespace log
{
	enum LogLevel
	{
		LEVEL_DEBUG = 1,
		LEVEL_INFO	= 11,
		LEVEL_WARN	= 21,
		LEVEL_ERROR = 31,
		LEVEL_FATAL = 41,
		LEVEL_NOTICE = 51,
		LEVEL_CUSTOM = 100
	};

	class CNewLog
	{
	public:
		CNewLog(){}
		~CNewLog(){}

	public:
		inline CNewLog& operator << (const std::string& str)
		{
			_store.append(str);
			return *this;
		}

		inline CNewLog& operator << (std::string& str)
		{
			_store.append(str);
			return *this;
		}

		inline CNewLog& operator << (const char* p)
		{
			_store.append(p);
			return *this;
		}

		inline CNewLog& operator << (char* p)
		{
			_store.append(p);
			return *this;
		}

		template<typename T>
		inline CNewLog& operator << (T t)
		{
			char szBuf[21];
#ifdef _WIN32
			unsigned int uiLen = sprintf(szBuf, "%I64d", (unsigned long long)t);
#else
			unsigned int uiLen = sprintf(szBuf, "%llu", (unsigned long long)t);
#endif
			_store.append(szBuf, uiLen);
			return *this;
		}

		inline const char* getBuf()
		{
			return _store.data();
		}

	private:
		std::string _store;
	};

	class IWindLog;

	class WindLog
	{
	public:
		static void Init();
		static void Init(const std::string& cfg);
		static void forcedLog(int level, const char* logger, const std::string& msg, void* data = NULL);
	private:
		static std::vector<IWindLog*> oImpls;
	};

    std::string TrimFile(char * s, int len, int l);

}
}



#endif
