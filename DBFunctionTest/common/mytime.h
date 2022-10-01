/*
 * =====================================================================================
 *
 *       Filename:  mytime.h
 *    Description:  ʱ�������غ�����װ����������2014-4-30��
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
	// ���ߺ���
	void SSleep(unsigned int sec);
	void MSleep(unsigned int msec);

#ifdef __linux__
	void USleep(unsigned int usec);
#endif


	//////////////////////////////////////////////////////////////////////////
	// ����1970��1��1��������񾭹�������
	long long SecsFrom1970();

	// �Ƿ�����
	bool LeapYear(int year);


	//////////////////////////////////////////////////////////////////////////
	// ����Ϊ�������
	class SecsCounter
	{
	public:
		static long long Counter()
		{
			return time(0);
		}
	};

	// ����Ϊ΢�������
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
				QueryPerformanceFrequency(&feq);//ÿ����������
				i++;
			}
			return feq.QuadPart;
		}
#endif
	};

	// ����Ϊ����ļ�����
	class MilliSecsCounter
	{
	public:
		static long long Counter()
		{
			return MicroSecsCounter::Counter()/1000;
		}
	};

	// ��ͬ���ȵļ�ʱ��ģ��
	template<typename T>
	class TCounter
	{
	public:
		TCounter()
			:_started(false),_begin(0),_elapsed(0),_elapsed_all(0)
		{

		}

	public:
		// ������ʱ��
		void Start()
		{
			if(_started)
				return ; // �Ѿ����У�ֱ���˳�
			_begin = T::Counter();
			_started = true;
		}

		// ��ͣ��ʱ�������㱾�����е�ʱ���ۼ���ʱ�����ر������е�ʱ���
		long long Stop()
		{
			if(!_started)
				return _elapsed; // �Ѿ�ֹͣ��ֱ���˳�
			long long end = T::Counter();
			_elapsed = end - _begin; // ���㱾�μ�ʱ
			_elapsed_all += _elapsed; // �ۼ��ܼ�ʱ
			_started = false;

			return _elapsed;
		}

		// ���ü�ʱ��
		void Reset()
		{
			if(_started)
				return; // �������еļ�ʱ������������
			_begin = _elapsed = _elapsed_all = 0;
		}

	public:



		// ���ؼ�ʱ�������һ��������ֹͣ��������ʱ��
		inline long long Elapsed() const { return _elapsed; }

		// ���ؼ�ʱ���ӵ�һ������������ֹͣ��������ʱ��
		inline long long ElapsedAll() const { return _elapsed_all; }

	private:
		bool _started; // �Ƿ�����

		long long _begin; // ���һ������ʱ��
		long long _elapsed; // ���һ����������������������
		long long _elapsed_all; // ��һ����������������������
	};

	typedef TCounter<SecsCounter> SecsTimer; // ����Ϊ��ļ�����
	typedef TCounter<MilliSecsCounter> MilliSecsTimer; // ����Ϊ����ļ�����
	typedef TCounter<MicroSecsCounter> MicroSecsTimer; // ����Ϊ΢��ļ�����


	//////////////////////////////////////////////////////////////////////////
	// ʱ����
	class DateTime
	{
	public:
		DateTime();
		~DateTime();

	public:
		DateTime(const DateTime& rhs);
		DateTime& operator =(const DateTime &rhs);

	public:
		// �Ƚ������
		bool operator <(const DateTime& rhs) const;
		bool operator >(const DateTime& rhs) const;
		bool operator ==(const DateTime& rhs) const;
		bool operator !=(const DateTime& rhs) const;
		bool operator <=(const DateTime& rhs) const;
		bool operator >=(const DateTime& rhs) const;

		// ������������������ʱ��Ĳ�ֵ����λ��
		long long operator - (const DateTime& rhs) const;

	public:
		// �Ƿ�Ϸ�
		inline bool Valid() const { return m_valid; }

		// ����ʱ��
		DateTime& SetTime(int y, int m, int d, int h, int mi, int sec, int msec);
		// ����ʱ�䣬���ַ���������ʱ�䲢����
		DateTime& SetTime(const std::string& strTime);
		// ����ʱ�䣬����ָ����ʽ�����ַ���������ʱ�䲢���ã�format��"yyyyMMdd hhmmss"
		DateTime& SetTime(const std::string& strTime, const std::string& strFormat);

		// ����һ��ʱ��
		DateTime& AddYear(int n);
		DateTime& AddMonth(int n);
		DateTime& AddDay(int n);
		DateTime& AddHour(int n);
		DateTime& AddMinute(int n);
		DateTime& AddSecond(int n);
		DateTime& AddMSecond(int n);

		// ���ظ���ʱ��ֵ
		inline int Year() const { return m_year; }
		inline int Month() const { return m_month; }
		inline int Day() const { return m_day; }
		inline int Hour() const { return m_hour; }
		inline int Minute() const { return m_minute; }
		inline int Second() const { return m_second; }
		inline int MSecond() const { return m_millisecond; }

	public: 
		// ��̬����
		// ��ȡ��ǰʱ���Ӧ���ַ�����ʽ��Ĭ�ϸ�ʽyyyyMMdd hhmmss
		// ֧��yyyyMMdd hhmmss.fff
		static std::string CurrentToString(const std::string &format = "yyyyMMdd hhmmss");

	public:
		// Ĭ�ϸ�ʽΪ"yyyyMMdd HHmmss"����ʽ�������ʱ��Ƿ����򷵻ؿ�
		std::string ToString(const std::string& format = "yyyyMMdd hhmmss") const;

		//public:
		//    // ���º�������ʹ��
		//    // ת�����ַ�����splitΪfalseʱ����ʽ"yyyyMMdd"������"yyyy-MM-dd"
		//    std::string ToDateString() const;
		//    // ת�����ַ���, splitΪfalseʱ����ʽ"yyyyMMdd hhmmss"������"yyyy-MM-dd hh:mm:ss"
		//    std::string ToTimeString() const;
		//    // ת�����ַ�������ʽΪ yyyy-MM-dd hh:mm:ss.fff 
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
	// ���ȵ��������ʱ��
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
		// �ȽϺ���
		bool operator<(const TinyDateTime& rhs) const;
		bool operator>(const TinyDateTime& rhs) const;
		bool operator==(const TinyDateTime& rhs) const;
		bool operator!=(const TinyDateTime& rhs) const;
		bool operator>=(const TinyDateTime& rhs) const;
		bool operator<=(const TinyDateTime& rhs) const;

		// ������������������ʱ��Ĳ�ֵ����λ��
		long long operator - (const TinyDateTime& rhs) const;

	public:
		// �Ƿ�Ϸ�
		inline bool Valid() const { return U_IM_T1.m_isValid == 1; }

	public:
		// ����ʱ��
		TinyDateTime& SetTime(int iYear, int iMonth, int iDay, int iHour, int iMinute, int iSecond, int iMSecond);
		// ����ʱ�䣬���ַ���������ʱ�䲢����
		TinyDateTime& SetTime(const std::string& strTime);        
		// ����ָ����ʽ����ʱ�䣬δʵ��
		TinyDateTime& SetTime(const std::string& strTime, const std::string& format);

		// ����ʱ��/��ȡʱ��
		inline TinyDateTime& SetTime(unsigned int nTime) { U_IM_T1.m_imt1 = nTime; return *this; }
		inline unsigned int ToInt() const { return U_IM_T1.m_imt1;}
		inline TinyDateTime& ClearTime() { U_IM_T1.m_hour = U_IM_T1.m_min = U_IM_T1.m_sec = 0; return *this; }

		// ��ȡ����ʱ��ֵ
		inline int Year() const { return U_IM_T1.m_year +  BaseYear; }
		inline int Month() const { return U_IM_T1.m_month; }
		inline int Day() const { return U_IM_T1.m_day; }
		inline int Hour() const { return U_IM_T1.m_hour; }
		inline int Minute() const { return U_IM_T1.m_min; }
		inline int Second() const { return U_IM_T1.m_sec; }

		// ת��Ϊtime_t
		time_t GetTime() const;

		// ����ʱ��
		inline TinyDateTime& AddYear(int n) { return AddTime(n, YEAR); }
		inline TinyDateTime& AddMonth(int n) { return AddTime(n, MON); }
		inline TinyDateTime& AddDay(int n) { return AddTime(n, DAY); }
		inline TinyDateTime& AddHour(int n) { return AddTime(n, HOUR); }
		inline TinyDateTime& AddMinute(int n) { return AddTime(n, MIN); }
		inline TinyDateTime& AddSecond(int n) { return AddTime(n, SEC); }

	public:
		// ת��Ϊ�ַ�����Ĭ�ϸ�ʽ"yyyyMMdd hhmmss"
		std::string ToString(const std::string& format = "yyyyMMdd hhmmss") const;

	public:
		// ��̬����
		// ��ȡ��ǰʱ����ַ���
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
