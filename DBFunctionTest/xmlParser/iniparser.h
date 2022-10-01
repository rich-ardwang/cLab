/*
* =====================================================================================
*
*       Filename:  ini.h
*    Description:  读取ini文件配置（卞军：2014-09-10）
*        Version:  1.0
*        Created:  2014-09-10
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  jbian
*        Company:  wind, Inc.
*
* =====================================================================================
*/

#ifndef _WIND_INIPARSER_H_
#define _WIND_INIPARSER_H_

#ifndef _WIN32
#ifndef __linux__
#error "Unknown os platform, _WIN32 or __linux__ not defined."
#endif
#endif

#include <string>

using namespace std;

namespace windcom
{
	class INIParser
	{
	public:
		static const char LEFT_BRACE = '[';
		static const char RIGHT_BRACE = ']';

	public:
		INIParser(const char* file);
		virtual ~INIParser();

	public:
		inline bool Good() const;

	public:
		bool GetValue(const string& strSection, const string& strKey, string& strValue);
		bool GetValue(const string& strSection, const string& strKey, int& iValue);
		/*template<typename T>
		bool GetValue(const string& strSection, const string& strKey, T& tValue)
		{
			string strValue;
			if(GetValue(strSection, strKey, strValue))
			{
				
			}
			return false;
		}*/

	private:
		inline bool isnewline(char c)
		{
			return ('\n' == c ||  '\r' == c );
		}
		inline bool isend(char c)
		{
			return '\0'==c;
		}
		inline bool isleft_barce(char c)
		{
			return LEFT_BRACE == c;
		}
		inline bool isright_brace(char c )
		{
			return RIGHT_BRACE == c;
		}

		bool parse(const char *section, const char *key, const char *buf, int *sec_s, int *sec_e,
			int *key_s, int *key_e, int *value_s, int *value_e);

	private:
		char* m_szBuf;
		int m_iBufLen;
		bool m_bGood;
	};
}

#endif//_WIND_INIFILE_H_
