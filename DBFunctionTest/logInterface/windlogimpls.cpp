#include "logInterface/windlogimpls.h"

#include "common/mytime.h"





namespace windcom
{
namespace log
{
	CDSLog::CDSLog()
	{

	}

	CDSLog::~CDSLog()
	{

	}

	void CDSLog::GenerateHead(const char* p, string& str, const char* format /* = NULL */)
	{
		char szBuf[512];
		DateTime oTmp;
		int n = sprintf(szBuf, "%s[%s:%d]", oTmp.ToString("yyyy-MM-dd hh:mm:ss,fff").data(),  __FILE__, __LINE__);
		str.append(szBuf, n);
		str.append("[");
		str.append(p);
		str.append("]");

		printf("Head: %s\n", str.data());
	}

	void CDSLog::SendLog(unsigned char ucType, const char* logger, const string& msg, unsigned short usMsgId /* = 500 */)
	{
		string strHead;
		GenerateHead(logger, strHead);
	}
}
}

