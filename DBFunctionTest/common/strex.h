/*
* =====================================================================================
*
*       Filename:  conv.h
*    Description:  String扩展函数接口定义封装（丁富来：2014-9-3）
*        Version:  1.0
*        Created:  2014-9-3
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  dingfulai
*        Company:  wind, Inc.
*
* =====================================================================================
*/

#ifndef _WIND_STREX_H_
#define _WIND_STREX_H_

#ifndef _WIN32
#ifndef __linux__
#error "Unknown os platform, _WIN32 or __linux__ not defined."
#endif
#endif

#include <string>
#include <vector>
#include <sstream>
#include <stdio.h>
#include "string.h"

#ifdef _WIN32
#define Snprintf _snprintf
#else
#define Snprintf snprintf
#endif

namespace windcom
{
	#define HEX(V) ((V)<10 ? '0'+(V) : 'A'+(V)-10)

    // 整型数字转为字符串，八进制/十进制/十六进制
    template<typename T>
    inline std::string _Num2Str(T sValue, int base)
    {
        /*std::stringstream str;
        switch(base)
        {
        case 8: str<<std::oct; break;
        case 10: str<<std::dec; break;
        case 16: str<<std::hex; break;
        default: str<<std::dec; break;
        }
        str << sValue;
        return str.str();*/

		const char* format = "%u";
		switch(base)
		{
		case 8: format="%o"; break;
		case 16: format="%X"; break;
		case 10: break;
		default: break;
		}

		char buf[32]; // 32字节，足以表示64位整数的八进制形式

		Snprintf(buf, sizeof(buf) - 1, format, sValue); // Linux和Windows下行为不同，但此处已经确保缓存长度足够
		buf[sizeof(buf)-1] = '\0';

		return buf;
    }

    // 浮点数字转为字符串，十进制
    template<typename T>
    inline std::string _NumF2Str(T sValue)
    {
        std::stringstream str;
        str << sValue;
        return str.str();
    }

    // 字符串转为数字，包括整型和浮点型
    template<typename T>
    inline bool _Str2Num(const std::string& str, T& value)
    {
        std::stringstream s(str);
        s >> value;
        if(s.fail()) return false;
        return true;
    }

    // 整型转字符串，支持8/10/16进制的字符串输出，base值若不是8/10/16，将按照10进制输出
    inline std::string Int2Str(short usValue, int base = 10) { return _Num2Str<short>(usValue ,base); }
    inline std::string Int2Str(unsigned short usValue, int base = 10) { return _Num2Str<unsigned short>(usValue, base); }
    inline std::string Int2Str(int usValue, int base = 10) { return _Num2Str<int>(usValue, base); }
    inline std::string Int2Str(unsigned int usValue, int base = 10) { return _Num2Str<unsigned int>(usValue, base); }
    inline std::string Int2Str(long usValue, int base = 10) { return _Num2Str<long>(usValue, base); }
    inline std::string Int2Str(unsigned long usValue, int base = 10) { return _Num2Str<unsigned long>(usValue, base); }
    inline std::string Int2Str(long long usValue, int base = 10) { return _Num2Str<long long>(usValue, base); }
    inline std::string Int2Str(unsigned long long usValue, int base = 10) { return _Num2Str<unsigned long long>(usValue, base); }

    // 浮点数转字符串
    inline std::string Float2Str(float fValue) { return _NumF2Str<float>(fValue); }
    inline std::string Float2Str(double dValue) { return _NumF2Str<double>(dValue); }

    // 字符串转整型
    inline bool Str2Int(const std::string& str, short& value) { return _Str2Num<short>(str, value); }
    inline bool Str2Int(const std::string& str, unsigned short& value) { return _Str2Num<unsigned short>(str, value); }
    inline bool Str2Int(const std::string& str, int& value) { return _Str2Num<int>(str, value); }
    inline bool Str2Int(const std::string& str, unsigned int& value) { return _Str2Num<unsigned int>(str, value); }
    inline bool Str2Int(const std::string& str, long& value) { return _Str2Num<long>(str, value); }
    inline bool Str2Int(const std::string& str, unsigned long& value) { return _Str2Num<unsigned long>(str, value); }
    inline bool Str2Int(const std::string& str, long long& value) { return _Str2Num<long long>(str, value); }
    inline bool Str2Int(const std::string& str, unsigned long long& value) { return _Str2Num<unsigned long long>(str, value); }

    // 字符串转浮点数，注意浮点数精度，转换后不一定与原来一模一样，但是近似相等
    inline bool Str2Float(const std::string& str, float& value) { return _Str2Num<float>(str, value); }
    inline bool Str2Float(const std::string& str, double& value) { return _Str2Num<double>(str, value); }

    // 字符串转数字，失败返回0
    inline short Str2Short(const std::string& str) { short v; return (Str2Int(str,v))?v:0; }
    inline unsigned short Str2UShort(const std::string& str) { unsigned short v; return (Str2Int(str,v))?v:0; }
    inline int Str2Int(const std::string& str) { int v; return (Str2Int(str,v))?v:0; }
    inline unsigned int Str2UInt(const std::string& str) { unsigned int v; return (Str2Int(str,v))?v:0; }
    inline long Str2Long(const std::string& str) { long v; return (Str2Int(str,v))?v:0; }
    inline unsigned long Str2ULong(const std::string& str) { unsigned long v; return (Str2Int(str,v))?v:0; }
    inline long long Str2LLong(const std::string& str) { long long v; return (Str2Int(str,v))?v:0; }
    inline unsigned long long Str2ULLong(const std::string& str) { unsigned long long v; return (Str2Int(str,v))?v:0; }

    inline float Str2Float(const std::string& str) { float v; return (Str2Float(str,v))?v:(float)0.0; }
    inline double Str2Double(const std::string& str) { double v; return (Str2Float(str,v))?v:0.0; }

    // 字符串比较函数
    int StrCmp(const std::string& src, const std::string& dst);
    int StrCmp(const char* src, const char* dst);
    int StrCmpNoCase(const std::string& src, const std::string& dst);
    int StrCmpNoCase(const char* src, const char* dst);


    // 将字符串根据单个分隔符拆分成多个，跳过空字符，例如"a,,c"，其分裂后得到"a"和"c"两个子串
    void Split(const std::string &str, char delim, std::vector<std::string> &elems);
    // 将字符串根据多个分隔符拆分成多个，跳过空字符，例如"a,,c"，其分裂后得到"a"和"c"两个子串
    void Split(const std::string& src, const std::string& separators, std::vector<std::string>& dest);

    // 将字符串根据多个分隔符拆分成多个，保留空字符，例如"a,,c"，其分裂后得到"a"、""和"c"三个子串，而",a,,c,"则分裂为"","a","","c"和""五个子串
    void SplitAbso(const std::string& src, const std::string& separators, std::vector<std::string>& dest);

    // 从第一个出现的分隔符处将字符串拆成两个，成功返回0，失败返回-1
    int Split(const std::string &str, char delim, std::string& first, std::string& second);

	// 找到指定字符中间的字符串
	std::string Between(const std::string &str, char left, char right);

	//从左向右找到第一个满足条件的字符串
	std::string BetweenL(const std::string &str, char first);

	//从右向左找到第一个满足条件的字符串
	std::string BetweenR(const std::string &str, char first);

    // 去除字符串开头和结尾处的符号
    std::string Trim(const std::string &src, const std::string &trim = "\r\n\t ");
    // 去除字符串开头的符号
    std::string TrimHead(const std::string &src, const std::string &trim = "\r\n\t ");
    // 去除字符串结尾的符号
    std::string TrimTail(const std::string &src, const std::string &trim = "\r\n\t ");


    // 将字符串中大写字母转换成小写字母
    std::string LowerStr(const std::string &src);
    // 将字符串中小写字母转换成大写字母
    std::string UpperStr(const std::string &src);


    // 将内存转换成16进制字符串
    std::string Buffer2HexStr(const char* szBuf, unsigned int uiLen);


	class bpointer
	{
	public:
		bpointer() : m_buf(NULL), m_size(0){}
		bpointer(const char* p, size_t len) : m_buf((char*)p), m_size(len){}
		bpointer(const bpointer& rhs) : m_buf(rhs.m_buf), m_size(rhs.m_size){}
		bpointer& operator =(const bpointer& rhs) {
			if (this != &rhs) { m_buf = rhs.m_buf; m_size = rhs.m_size; } return *this;
		}
		bool operator ==(const bpointer& rhs) const {
			if (m_size != rhs.size()) return false;
			return memcmp(m_buf, rhs.ptr(), m_size) == 0;
		}
		virtual ~bpointer(){}

		inline const char* ptr() const { return m_buf; }
		inline size_t size() const { return m_size; }
		inline bool empty() const { return (m_buf == NULL || m_size == 0); }
		inline void attach(const char* p, size_t len) { m_buf = (char*)p; m_size = len; }
		inline void reset() { m_buf = NULL; m_size = 0; }
		std::string rhex() const;

	private:
		char*	m_buf;					//数据区
		size_t	m_size;					//数据区大小
	};

	class sstring
	{
	public:
		sstring();
		sstring(const char *p, size_t len);
		sstring(const char *p);
		sstring(const sstring& rhs);
		sstring(const std::string& rhs);

		sstring& assign(const char* p, int n);
		sstring& operator =(const char* p);
		sstring& operator =(const sstring& rhs);
		sstring& operator =(const std::string& rhs);
		bool operator ==(const sstring& rhs) const;
		bool operator !=(const sstring& rhs) const;
		char& operator [](int i) const;
		inline operator bpointer() const { bpointer bp(m_pBuffer, m_nSize); return bp; }
		virtual ~sstring();

	public:
		bool Clone(const char *p, size_t len);
		void Reset() { CheckFree(); }
		size_t FindFirstOf(const sstring &trim);
		size_t FindLastOf(const sstring &trim);
		size_t FindFirstNotOf(const sstring &trim);
		size_t FindLastNotOf(const sstring &trim);
		sstring TrimLeft(const sstring &trim);
		sstring TrimRight(const sstring &trim);
		sstring Trim(const sstring &trim);
		sstring ToUpper();
		sstring ToLower();
		inline const char* Ptr() const { return m_pBuffer; }
		inline size_t Size() const { return m_nSize; }
		inline bool IsEmpty() const { return (m_pBuffer == NULL || m_nSize == 0); }
		inline void FromStdStr(const std::string &str) { Clone(str.data(), str.size()); }
		inline std::string ToStdStr() const { std::string str; str.assign(m_pBuffer, m_nSize); return str; }
		void CheckFree();

	private:
		bool CharIn(char c, const sstring &s);

	private:
		char *m_pBuffer;		//数据区
		size_t m_nSize;			//数据区大小
	};

	class sbinary : public sstring
	{
	public:
		sbinary() : sstring() {}
		sbinary(const char *p, size_t len) : sstring(p, len) {}
		sbinary(const sbinary& rhs) : sstring(rhs.Ptr(), rhs.Size()) {}
		sbinary(const std::string& rhs) : sstring(rhs) {}
	};

	class sbuffer : public sstring
	{
	public:
		sbuffer() : sstring() {}
		sbuffer(const char *p, size_t len) : sstring(p, len) {}
		sbuffer(const sbuffer& rhs) : sstring(rhs.Ptr(), rhs.Size()) {}
		sbuffer(const std::string& rhs) : sstring(rhs) {}
	};

	class spointer
	{
	public:
		spointer();
		spointer(const char *p, size_t len);
		spointer(const spointer& rhs);
		spointer& operator =(const spointer& rhs);
		~spointer();

		inline operator sbuffer()
		{
			sbuffer sb(m_pBuffer, m_nSize);
			return sb;
		}

		void Attach(const char *p, size_t len);
		inline const char* Ptr() const { return m_pBuffer; }
		inline size_t Size() const { return m_nSize; }

	private:
		char *m_pBuffer;		//数据区
		size_t m_nSize;			//数据区大小
	};
}

#endif
