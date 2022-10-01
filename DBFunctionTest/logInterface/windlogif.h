#ifndef _WIND_LOG_IF_H_
#define _WIND_LOG_IF_H_

#include <string>

namespace windcom
{
namespace log
{
	class IWindLog
	{
	public:
		IWindLog(){}
		virtual ~IWindLog(){}

	public:
		virtual void DoLog(int level, const char* logger, const std::string& msg, void* data = NULL) = 0; 
	};
}
}

#endif
