#include "windlog.h"

#include "windlogif.h"
#include "windlogimpls.h"
#include "windlogcfg.h"

namespace windcom
{
namespace log
{

	std::vector<IWindLog*> WindLog::oImpls;

	void WindLog::Init()
	{
		CLogConfig::GetInstance()->Init();
		if(CLogConfig::GetInstance()->GetConf().LogMode & LOCAL_MODE) WindLog::oImpls.push_back(new Log4CxxImpl());
		if(CLogConfig::GetInstance()->GetConf().LogMode & DS_MODE) WindLog::oImpls.push_back(new CDSLogImpl());
	}

	void WindLog::Init(const std::string &cfg)
	{
		CLogConfig::GetInstance()->Init(cfg.c_str());
		if(CLogConfig::GetInstance()->GetConf().LogMode & LOCAL_MODE) WindLog::oImpls.push_back(new Log4CxxImpl());
		if(CLogConfig::GetInstance()->GetConf().LogMode & DS_MODE) WindLog::oImpls.push_back(new CDSLogImpl());
	}

	void WindLog::forcedLog(int level, const char *logger, const std::string &msg, void *data)
	{
		for(size_t i=0; i<oImpls.size(); i++)
		{
			if(oImpls[i])
				oImpls[i]->DoLog(level, logger, msg, data);
		}
	}


    std::string TrimFile(char * s, int len, int l)
    {
        if (!s || len <= 2)
            return "";

        string sret=" - ))";

        int d = l;
        while(d)
        {
            sret += char(0x30 + d%10);
            d /= 10;
        }
        sret.append("(");
        int count = 0;
        for (int i = len-2; i>0; i--)
        {
            char ch = *(s + i);
            sret += ch;
            if (ch == '/' || ch == '\\')
            {
                if (++count >= 2)
                    break;
            }
        }
        sret.append(".(");

        return string(sret.rbegin(), sret.rend());
    }

}
}
