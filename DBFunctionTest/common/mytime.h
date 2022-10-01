/*
 * =====================================================================================
 *
 *       Filename:  mytime.h
 *    Description:  时间操作相关函数封装（丁富来：2014-4-30）
 *        Version:  1.0
 *        Created:  2014-4-30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  dingfulai
 *        Company:  wind, Inc.
 *
 * =====================================================================================
 */

#ifndef _WIND_MYTIME_H_
#define _WIND_MYTIME_H_

#ifdef _WIN32
#include <windows.h>
#include <time.h>
#elif defined __linux__
#include <unistd.h>
#include <sys/time.h>
#include <cstdlib>
#else
#error "Unknown os platform, _WIN32 or __linux__ not defined."
#endif

#include <string>




namespace windcom
{
	//////////////////////////////////////////////////////////////////////////
	// 休眠函数
	void SSleep(unsigned int sec);
	void MSleep(unsigned int msec);

#ifdef __linux__
	void USleep(unsigned int usec);
#endif


	//////////////////////////////////////////////////////////////////////////
	// 返回1970年1月1日零点至今经过的秒数
	long long SecsFrom1970();

	// 是否闰年
	bool LeapYear(int year);


	//////////////////////////////////////////////////////////////////////////
	// 精度为秒计数器
	class SecsCounter
	{
	public:
		static long long Counter()
		{
			return time(0);
		}
	};

	// 精度为微秒计数器
	class MicroSecsCounter
	{
	public:
		static long long Counter()
		{
#ifdef _WIN32
			LARGE_INTEGER t;
			QueryPerformanceCounter(&t);
			return t.QuadPart/(GetFrequerncy()/1000000);
#else
			timeval tv;
			gettimeofday(&tv, NULL);
			return tv.tv_sec * 1000000 + tv.tv_usec;
#endif
		}

#ifdef _WIN32
	private:
		static long long GetFrequerncy()
		{
			static int i = 0;
			static LARGE_INTEGER feq;
			if(i == 0)
			{
				QueryPerformanceFrequency(&feq);//每秒跳动次数
				i++;
			}
			return feq.QuadPart;
		}
#endif
	};

	// 精度为毫秒的计数器
	class MilliSecsCounter
	{
	public:
		static long long Counter()
		{
			return MicroSecsCounter::Counter()/1000;
		}
	};

	// 不同精度的计时器模板
	template<typename T>
	class TCounter
	{
	public:
		TCounter()
			:_started(false),_begin(0),_elapsed(0),_elapsed_all(0)
		{

		}

	public:
		// 启动计时器
		void Start()
		{
			if(_started)
				return ; // 已经运行，直接退出
			_begin = T::Counter();
			_started = true;
		}

		// 暂停计时器，计算本次运行的时间差并累加总时间差，返回本次运行的时间差
		long long Stop()
		{
			if(!_started)
				return _elapsed; // 已经停止，直接退出
			long long end = T::Counter();
			_elapsed = end - _begin; // 计算本次计时
			_elapsed_all += _elapsed; // 累加总计时
			_started = false;

			return _elapsed;
		}

		// 重置计时器
		void Reset()
		{
			if(_started)
				return; // 正在运行的计时器不允许重置
			_begin = _elapsed = _elapsed_all = 0;
		}

	public:



		// 返回计时器最近的一次启动到停止所经过的时间
		inline long long Elapsed() const { return _elapsed; }

		// 返回计时器从第一次启动到本次停止所经过的时间
		inline long long ElapsedAll() const { return _elapsed_all; }

	private:
		bool _started; // 是否启动

		long long _begin; // 最近一次启动时间
		long long _elapsed; // 最近一次启动到结束经过的秒数
		long long _elapsed_all; // 第一次启动到结束经过的秒数
	};

	typedef TCounter<SecsCounter> SecsTimer; // 精度为秒的计数器
	typedef TCounter<MilliSecsCounter> MilliSecsTimer; // 精度为毫秒的计数器
	typedef TCounter<MicroSecsCounter> MicroSecsTimer; // 精度为微秒的计数器


	//////////////////////////////////////////////////////////////////////////
	// 时间类
	class DateTime
	{
	public:
		DateTime();
		~DateTime();

	public:
		DateTime(const DateTime& rhs);
		DateTime& operator =(const DateTime &rhs);

	public:
		// 比较运算符
		bool operator <(const DateTime& rhs) const;
		bool operator >(const DateTime& rhs) const;
		bool operator ==(const DateTime& rhs) const;
		bool operator !=(const DateTime& rhs) const;
		bool operator <=(const DateTime& rhs) const;
		bool operator >=(const DateTime& rhs) const;

		// 相减运算符，计算两个时间的差值，单位秒
		long long operator - (const DateTime& rhs) const;

	public:
		// 是否合法
		inline bool Valid() const { return m_valid; }

		// 设置时间
		DateTime& SetTime(int y, int m, int d, int h, int mi, int sec, int msec);
		// 设置时间，从字符串解析出时间并设置
		DateTime& SetTime(const std::string& strTime);
		// 设置时间，根据指定格式，从字符串解析出时间并设置，format如"yyyyMMdd hhmmss"
		DateTime& SetTime(const std::string& strTime, const std::string& strFormat);

		// 增加一定时间
		DateTime& AddYear(int n);
		DateTime& AddMonth(int n);
		DateTime& AddDay(int n);
		DateTime& AddHour(int n);
		DateTime& AddMinute(int n);
		DateTime& AddSecond(int n);
		DateTime& AddMSecond(int n);

		// 返回各个时间值
		inline int Year() const { return m_year; }
		inline int Month() const { return m_month; }
		inline int Day() const { return m_day; }
		inline int Hour() const { return m_hour; }
		inline int Minute() const { return m_minute; }
		inline int Second() const { return m_second; }
		inline int MSecond() const { return m_millisecond; }

	public: 
		// 静态函数
		// 获取当前时间对应的字符串形式，默认格式yyyyMMdd hhmmss
		// 支持yyyyMMdd hhmmss.fff
		static std::string CurrentToString(const std::string &format = "yyyyMMdd hhmmss");

	public:
		// 默认格式为"yyyyMMdd HHmmss"，格式错误或者时间非法，则返回空
		std::string ToString(const std::string& format = "yyyyMMdd hhmmss") const;

		//public:
		//    // 以下函数不再使用
		//    // 转换成字符串，split为false时，格式"yyyyMMdd"，否则"yyyy-MM-dd"
		//    std::string ToDateString() const;
		//    // 转换成字符串, split为false时，格式"yyyyMMdd hhmmss"，否则"yyyy-MM-dd hh:mm:ss"
		//    std::string ToTimeString() const;
		//    // 转换成字符串，格式为 yyyy-MM-dd hh:mm:ss.fff 
		//    std::string ToTimeStamp() const;

	private:
		bool valid(int y, int m, int d, int h, int mi, int sec, int msec);

	private:
		int m_year;
		int m_month;
		int m_day;
		int m_hour;
		int m_minute;
		int m_second;
		int m_millisecond;
		bool m_valid;
	};


	//////////////////////////////////////////////////////////////////////////
	// 精度到秒的日期时间
	class TinyDateTime
	{
	public:
		enum
		{
			YEAR = 0,
			MON,
			DAY,
			HOUR,
			MIN,
			SEC
		};

	public:
		TinyDateTime();
		~TinyDateTime(){}

	public:
		TinyDateTime(const TinyDateTime& rhs) { U_IM_T1.m_imt1 = rhs.U_IM_T1.m_imt1; }
		TinyDateTime& operator=(const TinyDateTime& rhs);

	public:
		// 比较函数
		bool operator<(const TinyDateTime& rhs) const;
		bool operator>(const TinyDateTime& rhs) const;
		bool operator==(const TinyDateTime& rhs) const;
		bool operator!=(const TinyDateTime& rhs) const;
		bool operator>=(const TinyDateTime& rhs) const;
		bool operator<=(const TinyDateTime& rhs) const;

		// 相减运算符，计算两个时间的差值，单位秒
		long long operator - (const TinyDateTime& rhs) const;

	public:
		// 是否合法
		inline bool Valid() const { return U_IM_T1.m_isValid == 1; }

	public:
		// 设置时间
		TinyDateTime& SetTime(int iYear, int iMonth, int iDay, int iHour, int iMinute, int iSecond, int iMSecond);
		// 设置时间，从字符串解析出时间并设置
		TinyDateTime& SetTime(const std::string& strTime);        
		// 按照指定格式设置时间，未实现
		TinyDateTime& SetTime(const std::string& strTime, const std::string& format);

		// 设置时间/获取时间
		inline TinyDateTime& SetTime(unsigned int nTime) { U_IM_T1.m_imt1 = nTime; return *this; }
		inline unsigned int ToInt() const { return U_IM_T1.m_imt1;}
		inline TinyDateTime& ClearTime() { U_IM_T1.m_hour = U_IM_T1.m_min = U_IM_T1.m_sec = 0; return *this; }

		// 获取各个时间值
		inline int Year() const { return U_IM_T1.m_year +  BaseYear; }
		inline int Month() const { return U_IM_T1.m_month; }
		inline int Day() const { return U_IM_T1.m_day; }
		inline int Hour() const { return U_IM_T1.m_hour; }
		inline int Minute() const { return U_IM_T1.m_min; }
		inline int Second() const { return U_IM_T1.m_sec; }

		// 转换为time_t
		time_t GetTime() const;

		// 增加时间
		inline TinyDateTime& AddYear(int n) { return AddTime(n, YEAR); }
		inline TinyDateTime& AddMonth(int n) { return AddTime(n, MON); }
		inline TinyDateTime& AddDay(int n) { return AddTime(n, DAY); }
		inline TinyDateTime& AddHour(int n) { return AddTime(n, HOUR); }
		inline TinyDateTime& AddMinute(int n) { return AddTime(n, MIN); }
		inline TinyDateTime& AddSecond(int n) { return AddTime(n, SEC); }

	public:
		// 转化为字符串，默认格式"yyyyMMdd hhmmss"
		std::string ToString(const std::string& format = "yyyyMMdd hhmmss") const;

	public:
		// 静态函数
		// 获取当前时间的字符串
		static std::string CurrentToString(const std::string& format);

	private:
		TinyDateTime& AddTime(int n, int bitmap);
		void DefTime();
		bool VaildTime(int iYear, int iMonth, int iDay, int iHour, int iMinute, int iSecond);

	private:
		union
		{
			struct
			{
				unsigned int m_isValid:1;
				unsigned int m_sec:6;
				unsigned int m_min:6;
				unsigned int m_hour:5;
				unsigned int m_day:5;
				unsigned int m_month:4;
				unsigned int m_year:5;
			};
			unsigned int m_imt1;
		}U_IM_T1;

		// Year is based on 2000, 2000 - 2032
		static const int BaseYear = 2000;
	};

}


#endif
