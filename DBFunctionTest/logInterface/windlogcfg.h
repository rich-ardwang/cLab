#ifndef _WIND_LOG_CFG_H_
#define _WIND_LOG_CFG_H_

#include "./common/spinlock.h"
#include <string>

using namespace std;

namespace windcom
{
namespace log
{
	extern SpinLock oLog4WInitializeLock;
	extern bool bLog4WInitialized;

	// ��־���ģʽ����
	static const int LOCAL_MODE = 0x01;	// ʹ��log4cxx���
	static const int DS_MODE = 0x02;	// ���÷ֲ�ʽ��־���

	static const char* LOG4CXX_CONFIG = "log4cxx.properties";
	static const char* WINDLOG_CONFIG = "WindCommonConfig.xml";

	// �ֲ�ʽ��־ϵͳ����
	struct WindLogConf
	{
		WindLogConf():LogMode(LOCAL_MODE)
		{}

		bool Parse(const char* file);

		unsigned int LogMode; // ��־���ģʽ
		string LocalConfig; // log4cxx�����ļ���		
	};

	// ��־���������
	class CLogConfig
	{
	public:
		~CLogConfig();

		static CLogConfig* GetInstance()
		{
			if(!_instance)
			{
				_lock.Lock();
				if(!_instance) _instance = new CLogConfig();
				_lock.UnLock();
			}
			return _instance;
		}

		inline WindLogConf GetConf() const
		{
			return _config;
		}

		inline unsigned int GetMode() const
		{
			return _config.LogMode;
		}

		int Init(const char* config = WINDLOG_CONFIG);

	private:
		CLogConfig();
		CLogConfig(const CLogConfig&);

	private:
		int _InitLocal();
		int _InitDS();
		int _load(const char* config);

	private:
		static SpinLock _lock;
		static CLogConfig* _instance;
		unsigned int _mode;
		WindLogConf _config;
	};

}
}


#endif
