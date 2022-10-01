#include "logInterface/windlogcfg.h"
#include "xmlparser/xmlparser.h"
#include "logInterface/logger.h"





namespace windcom
{
namespace log
{

	SpinLock oLog4WInitializeLock;
	bool bLog4WInitialized = false;

	bool WindLogConf::Parse(const char *config)
	{
		XMLParser* pXMLConfig = new XMLParser(config);
		if (!pXMLConfig || !pXMLConfig->Good())
		{
			printf("[WindCom][WindLogConf::Parse] - Load config file [%s] failed.\n", config);
			return false;
		}

		// 解析日志模式
		int iMode = 0;
		TiXmlElement* pElement = pXMLConfig->GetSubNode("WindLog");
		if(!pElement)
		{
			printf("[WindCom][WindLogConf::Parse] - Load config file [%s] failed, no 'WindLog' node.\n", config);
			return false;
		}

		if(!pXMLConfig->GetAttribute(pElement, "LogMode", "Mode", iMode))
		{
			printf("[WindCom][WindLogConf::Parse] - Load config LogMode::Mode failed.\n");
			return false;
		}

		// 如果使用log4cxx组件则读取配置文件名，读取失败使用默认文件
		if(iMode & LOCAL_MODE) 
		{
			LogMode |= LOCAL_MODE;
			if(!pXMLConfig->GetAttribute(pElement, "Log4cxx", "Config", LocalConfig) || LocalConfig.empty())
			{
				LocalConfig = LOG4CXX_CONFIG;
			}
		}

		if(iMode & DS_MODE) 
		{
			LogMode |= DS_MODE;
		}
		
		return true;
	}

	SpinLock CLogConfig::_lock;
	CLogConfig* CLogConfig::_instance = NULL;

	CLogConfig::CLogConfig()
		:_mode(LOCAL_MODE)
	{

	}

	CLogConfig::~CLogConfig()
	{
//		if(_mode & DS_MODE)
	}

	int CLogConfig::Init(const char* config /* = WINDLOG_CONFIG */)
	{
		if(0 != _load(config))
		{
			printf("[WindCom][CLogConfig::Init] - Read from config file [%s] failed. Init local mode.\n", config);
			return -1;
		}

		if(_config.LogMode & LOCAL_MODE)
		{
			if(0 != _InitLocal())
				return -1; // 初始化本地日志系统
		}
		if(_config.LogMode & DS_MODE)
		{
			if(0 != _InitDS())
				return -1; // 初始化分布式日志系统
		}

		return 0;
	}

	int CLogConfig::_InitLocal()
	{
		log4cxx::PropertyConfigurator::configureAndWatch(_config.LocalConfig);
		//printf("[WindCom][CLogConfig::InitLocal] - Init log4cxx, the config file is %s.\n", _config.LocalConfig.c_str());
		return 0;
	}

	int CLogConfig::_InitDS()
	{
		//oLog4WInitializeLock.Lock();
		//if(bLog4WInitialized) // 如果已经初始化，则直接返回
		//{
		//	oLog4WInitializeLock.UnLock();
		//	return 0;
		//}
		//bLog4WInitialized = true; // 未初始化则进行初始化
		//oLog4WInitializeLock.UnLock();
		return 0;
	}

	int CLogConfig::_load(const char* config)
	{
		if(!_config.Parse(config))
			return -1;
		return 0;
	}

}
}
