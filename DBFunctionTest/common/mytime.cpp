/*
 * =====================================================================================
 *
 *       Filename:  time.cpp
 *    Description:  时间操作相关函数实现（丁富来：2014-4-30）
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

#include "common/mytime.h"
#include <sys/timeb.h>




namespace windcom
{
		//////////////////////////////////////////////////////////////////////////
	// 休眠函数
	void SSleep(unsigned int sec)
	{
	#ifdef _WIN32
		Sleep(sec * 1000);
	#else
		sleep(sec);
	#endif
	}

	void MSleep(unsigned int msec)
	{
	#ifdef _WIN32
		Sleep(msec);
	#else
		usleep(msec * 1000);
	#endif
	}

	#ifdef __linux__
	void USleep(unsigned int usec)
	{
		usleep(usec);
	}
	#endif


	//////////////////////////////////////////////////////////////////////////
	// 1970至今的秒数
	long long SecsFrom1970()
	{
		return time(0);
	}

	// 是否闰年
	bool LeapYear(int iYear)
	{
		return (((iYear%4==0)&&(iYear%100!=0))||(iYear%400==0));
	}


	//////////////////////////////////////////////////////////////////////////
	// 日期时间类
	DateTime::DateTime()
		:m_valid(false)
	{
		struct timeb tmb;
		ftime(&tmb);
		struct tm ptm = { 0 };
	#ifdef _WIN32
		errno_t ret = localtime_s(&ptm, &tmb.time);
	#else
		struct tm * ret = localtime_r(&tmb.time, &ptm);
	#endif

	#ifdef _WIN32
		if(ret == 0)
	#else
		if(ret)
	#endif
		{
			m_year = ptm.tm_year + 1900;
			m_month = ptm.tm_mon + 1;
			m_day = ptm.tm_mday;
			m_hour = ptm.tm_hour;
			m_minute = ptm.tm_min;
			m_second = ptm.tm_sec;
			m_millisecond = tmb.millitm;

			m_valid = true;
		}
	}

	DateTime::~DateTime()
	{

	}

	DateTime::DateTime(const DateTime& rhs)
		:m_year(rhs.m_year),
		m_month(rhs.m_month),
		m_day(rhs.m_day),
		m_hour(rhs.m_hour),
		m_minute(rhs.m_minute),
		m_second(rhs.m_second),
		m_millisecond(rhs.m_millisecond),
		m_valid(rhs.m_valid)
	{

	}

	DateTime& DateTime::operator = (const DateTime &rhs)
	{
		if(&rhs == this) return *this;

		m_year = rhs.m_year;
		m_month = rhs.m_month;
		m_day = rhs.m_day;
		m_hour = rhs.m_hour;
		m_minute = rhs.m_minute;
		m_second = rhs.m_second;
		m_millisecond = rhs.m_millisecond;
		m_valid = rhs.m_valid;
		return *this;
	}

	bool DateTime::operator < (const DateTime& rhs) const
	{
		return !operator>=(rhs);
	}

	bool DateTime::operator > (const DateTime& rhs) const
	{
		return !operator<=(rhs);
	}

	bool DateTime::operator == (const DateTime& rhs) const
	{
		return (m_year == rhs.m_year && m_month == rhs.m_month 
			&& m_day == rhs.m_day && m_hour == rhs.m_hour
			&& m_minute == rhs.m_minute && m_second == rhs.m_second
			&& m_millisecond == rhs.m_millisecond);
	}

	bool DateTime::operator != (const DateTime& rhs) const
	{
		return !(*this == rhs);
	}

	bool DateTime::operator <= (const DateTime& rhs) const
	{
		if (m_year > rhs.m_year) return false;
		else if (m_year < rhs.m_year) return true;

		if (m_month > rhs.m_month) return false;
		else if (m_month < rhs.m_month) return true;

		if (m_day > rhs.m_day) return false;
		else if (m_day < rhs.m_day) return true;

		if (m_hour > rhs.m_hour) return false;
		else if (m_hour < rhs.m_hour) return true;

		if (m_minute > rhs.m_minute) return false;
		else if (m_minute < rhs.m_minute) return true;

		if (m_second > rhs.m_second) return false;
		else if (m_second < rhs.m_second) return true;

		return m_millisecond <= rhs.m_millisecond;
	}

	bool DateTime::operator >= (const DateTime& rhs) const
	{
		if (m_year > rhs.m_year) return true;
		else if (m_year < rhs.m_year) return false;

		if (m_month > rhs.m_month) return true;
		else if (m_month < rhs.m_month) return false;

		if (m_day > rhs.m_day) return true;
		else if (m_day < rhs.m_day) return false;

		if (m_hour > rhs.m_hour) return true;
		else if (m_hour < rhs.m_hour) return false;

		if (m_minute > rhs.m_minute) return true;
		else if (m_minute < rhs.m_minute) return false;

		if (m_second > rhs.m_second) return true;
		else if (m_second < rhs.m_second) return false;

		return m_millisecond >= rhs.m_millisecond;
	}

	long long DateTime::operator -(const DateTime &rhs) const
	{
		struct tm timev1;
		struct tm timev2;

		timev1.tm_year = this->Year() - 1900;
		timev1.tm_mon  = this->Month() - 1;
		timev1.tm_mday = this->Day();
		timev1.tm_hour = this->Hour();
		timev1.tm_min  = this->Minute();
		timev1.tm_sec  = this->Second();

		time_t  t1 = mktime(&timev1);

		timev2.tm_year = rhs.Year() - 1900;
		timev2.tm_mon  = rhs.Month() - 1;
		timev2.tm_mday = rhs.Day();
		timev2.tm_hour = rhs.Hour();
		timev2.tm_min  = rhs.Minute();
		timev2.tm_sec  = rhs.Second();

		time_t t2 = mktime(&timev2);

		return t1 - t2;
	}

	DateTime& DateTime::SetTime(int y, int m, int d, int h, int min, int sec, int msec)
	{
		if(!valid(y, m, d, h, min, sec, msec))
		{
			m_valid = false;
			return *this;
		}
		m_year = y;
		m_month = m;
		m_day = d;
		m_hour = h;
		m_minute = min;
		m_second = sec;
		m_millisecond = msec;
		m_valid = true;

		return *this;
	}

	DateTime& DateTime::SetTime(const std::string& strTime)
	{
		const char split[] = {4,2,2,2,2,2,3};  //年月日时分秒毫秒 每个字段最长的占位

		int tmpbuf[7] = {0};    //存放的是年月日时分秒的值
		char buf[8];            //缓存
		int i = 0;
		int j = 0;

		std::string::const_iterator it = strTime.begin();
		for (; it != strTime.end(); ++it)
		{
			if(j >= 7)
				break;

			if (isdigit((unsigned char)*it))
			{
				if (i<split[j])
				{
					buf[i++] = *it;
				}
				else
				{
					buf[i] = 0;
					tmpbuf[j] = atoi(buf);
					i = 0;
					buf[i++] = *it;
					j++;
				}
			}
			else
			{
				buf[i] = 0;
				if (i>0)
				{
					tmpbuf[j] = atoi(buf);

					i = 0;
					j++;
				}
			}
		}

		if (j < 7 && i<=split[j])
		{
			buf[i] = 0;
			tmpbuf[j] = atoi(buf);
		}

		return SetTime(tmpbuf[0], tmpbuf[1], tmpbuf[2], tmpbuf[3], tmpbuf[4], tmpbuf[5], tmpbuf[6]);
	}

	DateTime& DateTime::SetTime(const std::string& strTime, const std::string& strFormat)
	{
		std::string tmpbuf[7];
		int tmptime[7] = {0};
		bool valid = true;

		if (strTime.length() == strFormat.length())
		{
			for (size_t i=0; i<strFormat.length(); ++i)
			{
				switch (strFormat[i])
				{
				case 'y':
					tmpbuf[0].push_back(strTime[i]);
					break;
				case 'M':
					tmpbuf[1].push_back(strTime[i]);
					break;
				case 'd':
					tmpbuf[2].push_back(strTime[i]);
					break;
				case 'h':
					tmpbuf[3].push_back(strTime[i]);
					break;
				case 'm':
					tmpbuf[4].push_back(strTime[i]);
					break;
				case 's':
					tmpbuf[5].push_back(strTime[i]);
					break;
				case 'f':
					tmpbuf[6].push_back(strTime[i]);
					break;
				case ':':
				case ' ':
				case '/':
				case '-':
					break;
				default:
					valid = false;
					break;
				}
			}

			if (valid)
			{
				for (int i=0; i<7; ++i)
				{
					tmptime[i] = atoi(tmpbuf[i].c_str());
				}
			}
		}

		 return SetTime(tmptime[0], tmptime[1], tmptime[2], tmptime[3], tmptime[4], tmptime[5], tmptime[6]);
	}

	DateTime & DateTime::AddYear(int n)
	{
		m_year += n;
		return *this;
	}

	DateTime & DateTime::AddMonth(int n)
	{
		tm stm;
		stm.tm_year = m_year - 1900;
		stm.tm_mon = m_month - 1 + n;
		stm.tm_mday = m_day;
		stm.tm_hour = m_hour;
		stm.tm_min = m_minute;
		stm.tm_sec = m_second;

		time_t mt =  mktime(&stm);
		if (-1 == mt)
		{
			m_valid = false;
		}
		else
		{
			m_valid = true;

			//tm *ptm = localtime(&mt);
			struct tm ptm = { 0 };
	#ifdef _WIN32
			localtime_s(&ptm, &mt);
	#else
			localtime_r(&mt, &ptm);
	#endif
			m_year = ptm.tm_year + 1900;
			m_month = ptm.tm_mon + 1;
			m_day = ptm.tm_mday;
			m_hour = ptm.tm_hour;
			m_minute = ptm.tm_min;
			m_second = ptm.tm_sec;
		}

		return *this;
	}

	DateTime & DateTime::AddDay(int n)
	{
		tm stm;
		stm.tm_year = m_year - 1900;
		stm.tm_mon = m_month - 1;
		stm.tm_mday = m_day + n;
		stm.tm_hour = m_hour;
		stm.tm_min = m_minute;
		stm.tm_sec = m_second;

		time_t mt =  mktime(&stm);
		if (-1 == mt)
		{
			m_valid = false;
		}
		else
		{
			m_valid = true;

			//tm *ptm = localtime(&mt);
			struct tm ptm = { 0 };
	#ifdef _WIN32
			localtime_s(&ptm, &mt);
	#else
			localtime_r(&mt, &ptm);
	#endif
			m_year = ptm.tm_year + 1900;
			m_month = ptm.tm_mon + 1;
			m_day = ptm.tm_mday;
			m_hour = ptm.tm_hour;
			m_minute = ptm.tm_min;
			m_second = ptm.tm_sec;
		}

		return *this;
	}

	DateTime & DateTime::AddHour(int n)
	{
		tm stm;
		stm.tm_year = m_year - 1900;
		stm.tm_mon = m_month - 1;
		stm.tm_mday = m_day;
		stm.tm_hour = m_hour + n;
		stm.tm_min = m_minute;
		stm.tm_sec = m_second;

		time_t mt =  mktime(&stm);
		if (-1 == mt)
		{
			m_valid = false;
		}
		else
		{
			m_valid = true;

			//tm *ptm = localtime(&mt);
			struct tm ptm = { 0 };
	#ifdef _WIN32
			localtime_s(&ptm, &mt);
	#else
			localtime_r(&mt, &ptm);
	#endif
			m_year = ptm.tm_year + 1900;
			m_month = ptm.tm_mon + 1;
			m_day = ptm.tm_mday;
			m_hour = ptm.tm_hour;
			m_minute = ptm.tm_min;
			m_second = ptm.tm_sec;
		}

		return *this;
	}

	DateTime & DateTime::AddMinute(int n)
	{
		tm stm;
		stm.tm_year = m_year - 1900;
		stm.tm_mon = m_month - 1;
		stm.tm_mday = m_day;
		stm.tm_hour = m_hour;
		stm.tm_min = m_minute + n;
		stm.tm_sec = m_second;

		time_t mt =  mktime(&stm);
		if (-1 == mt)
		{
			m_valid = false;
		}
		else
		{
			m_valid = true;

			//tm *ptm = localtime(&mt);
			struct tm ptm = { 0 };
	#ifdef _WIN32
			localtime_s(&ptm, &mt);
	#else
			localtime_r(&mt, &ptm);
	#endif
			m_year = ptm.tm_year + 1900;
			m_month = ptm.tm_mon + 1;
			m_day = ptm.tm_mday;
			m_hour = ptm.tm_hour;
			m_minute = ptm.tm_min;
			m_second = ptm.tm_sec;
		}

		return *this;
	}

	DateTime & DateTime::AddSecond(int n)
	{
		tm stm;
		stm.tm_year = m_year - 1900;
		stm.tm_mon = m_month - 1;
		stm.tm_mday = m_day;
		stm.tm_hour = m_hour;
		stm.tm_min = m_minute;
		stm.tm_sec = m_second + n;

		time_t mt =  mktime(&stm);
		if (-1 == mt)
		{
			m_valid = false;
		}
		else
		{
			m_valid = true;

			//tm *ptm = localtime(&mt);

			struct tm ptm = { 0 };
	#ifdef _WIN32
			localtime_s(&ptm, &mt);
	#else
			localtime_r(&mt, &ptm);
	#endif

			m_year = ptm.tm_year + 1900;
			m_month = ptm.tm_mon + 1;
			m_day = ptm.tm_mday;
			m_hour = ptm.tm_hour;
			m_minute = ptm.tm_min;
			m_second = ptm.tm_sec;
		}
		return *this;
	}

	DateTime & DateTime::AddMSecond(int n)
	{
		int tsec = n/1000;
		int tmsec = n%1000;

		int msecond;
		if(tmsec + m_millisecond < 0)
		{
			tsec -= 1;
			msecond = (tmsec + m_millisecond + 1000)%1000;
		}
		else
		{
			tsec += (tmsec + m_millisecond)/1000;
			msecond = (tmsec + m_millisecond)%1000;
		}

		AddSecond(tsec);
		m_millisecond = msecond;

		return *this;
	}

	bool DateTime::valid(int y, int m, int d, int h, int min, int sec, int msec)
	{
		const static int monthLengths[2][13] = {
			{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
			{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, };

			if (y<1900 || m<1 || m>12) return false;
			if (d<0 || d>monthLengths[LeapYear(y)?1:0][m]) return false;
			if (h<0 || h>23 || min<0 || min>59 || sec<0 || sec>59 || msec<0 || msec>999) return false;
			return true;
	}

	std::string DateTime::ToString(const std::string& format) const
	{
		if(format.empty() || !m_valid)
			return "";

		// 表示年、月、日、时、分、秒、毫秒的长度
		int pos[7];
		for(unsigned int i=0; i<sizeof(pos)/sizeof(pos[0]); i++)
			pos[i] = -1;

		const static int len[7] = {4,2,2,2,2,2,3};

		int val[7];
		val[0] = m_year;
		val[1] = m_month;
		val[2] = m_day;
		val[3] = m_hour;
		val[4] = m_minute;
		val[5] = m_second;
		val[6] = m_millisecond;

		bool b24 = false;

		for(unsigned int i=0; i<format.length(); i++)
		{
			char c = format[i];
			if(c == 'y')
			{
				if(format[i+1] == 'y' && format[i+2] == 'y' && format[i+3] == 'y')
				{
					pos[0] = i;
					i += 3;
				}
			}
			else if(c == 'M')
			{
				if(format[i+1] == 'M')
				{
					pos[1] = i;
					i += 1;
				}
			}
			else if(c == 'd')
			{
				if(format[i+1] == 'd')
				{
					pos[2] = i;
					i += 1;
				}
			}
			else if(c == 'h')
			{
				if(format[i+1] == 'h')
				{
					pos[3] = i;
					i += 1;
				}
			}
			else if(c == 'm')
			{
				if(format[i+1] == 'm')
				{
					pos[4] = i;
					i += 1;
				}
			}
			else if(c == 's')
			{
				if(format[i+1] == 's')
				{
					pos[5] = i;
					i += 1;
				}
			}
			else if(c == 'f')
			{
				if(format[i+1] == 'f' && format[i+2] == 'f')
				{
					pos[6] = i;
					i += 2;
				}
			}
		}

		char buf[32];
		std::string ret = format;
		for(unsigned int i=0; i<sizeof(pos)/sizeof(pos[0]); i++)
		{
			if(pos[i] != -1)
			{
				std::string fmt = "%0";
	#ifdef _WIN32
				_snprintf
	#else
				snprintf
	#endif
					(buf, sizeof(buf) - 1, "%d", len[i]);
				fmt += buf;
				fmt += "d";

	#ifdef _WIN32
				_snprintf
	#else
				snprintf
	#endif
					(buf, sizeof(buf) - 1, fmt.c_str(), val[i]);

				ret.replace(pos[i], len[i], buf);
			}
		}

		return ret;
	}


	std::string DateTime::CurrentToString(const std::string &format)
	{
		DateTime current;
		return current.ToString(format);
	}

	//////////////////////////////////////////////////////////////////////////

	// TinyDateTime
	TinyDateTime::TinyDateTime()
	{
		time_t curTime = time(NULL);
		struct tm curTm;

	#ifdef _WIN32
		localtime_s(&curTm, &curTime);
	#else
		localtime_r(&curTime, &curTm);
	#endif
		U_IM_T1.m_year = curTm.tm_year + 1900 - BaseYear;
		U_IM_T1.m_month = curTm.tm_mon+1;
		U_IM_T1.m_day = curTm.tm_mday;
		U_IM_T1.m_hour = curTm.tm_hour;
		U_IM_T1.m_min = curTm.tm_min;
		U_IM_T1.m_sec = curTm.tm_sec;
		U_IM_T1.m_isValid = 1;

	}

	TinyDateTime& TinyDateTime::operator=(const TinyDateTime& date)
	{
		if (this != &date)
		{
			U_IM_T1.m_imt1 = date.U_IM_T1.m_imt1;
		}
		return *this;
	}

	bool TinyDateTime::operator<(const TinyDateTime& date) const
	{
		return U_IM_T1.m_imt1 < date.U_IM_T1.m_imt1;
	}

	bool TinyDateTime::operator>(const TinyDateTime& date) const
	{
		return U_IM_T1.m_imt1 > date.U_IM_T1.m_imt1;
	}

	bool TinyDateTime::operator==(const TinyDateTime& date) const
	{
		return U_IM_T1.m_imt1 == date.U_IM_T1.m_imt1;
	}

	bool TinyDateTime::operator!=(const TinyDateTime& date) const
	{
		return U_IM_T1.m_imt1 != date.U_IM_T1.m_imt1;
	}

	bool TinyDateTime::operator>=(const TinyDateTime& date) const
	{
		return U_IM_T1.m_imt1 >= date.U_IM_T1.m_imt1;
	}

	bool TinyDateTime::operator<=(const TinyDateTime& date) const
	{
		return U_IM_T1.m_imt1 <= date.U_IM_T1.m_imt1;
	}

	long long TinyDateTime::operator -(const TinyDateTime &rhs) const
	{
		struct tm timev1;
		struct tm timev2;

		timev1.tm_year = this->Year();
		timev1.tm_mon  = this->Month();
		timev1.tm_mday = this->Day();
		timev1.tm_hour = this->Hour();
		timev1.tm_min  = this->Minute();
		timev1.tm_sec  = this->Second();

		time_t  t1 = mktime(&timev1);

		timev2.tm_year = rhs.Year();
		timev2.tm_mon  = rhs.Month();
		timev2.tm_mday = rhs.Day();
		timev2.tm_hour = rhs.Hour();
		timev2.tm_min  = rhs.Minute();
		timev2.tm_sec  = rhs.Second();

		time_t t2 = mktime(&timev2);

		return t1 - t2;
	}

	bool TinyDateTime::VaildTime(int iYear, int iMonth, int iDay, int iHour, int iMinute, int iSecond)
	{
		static const int monthLengths[2][13] = {
			{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
			{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},};

			if (iYear < 1900 || iMonth < 1 || iMonth > 12)
			{
				return false;
			}
			if (iDay < 0 || iDay > monthLengths[LeapYear(iYear)][iMonth])
			{
				return false;
			}
			if (iHour < 0 || iHour > 23 || iMinute < 0 || iMinute > 59
				|| iSecond < 0 || iMinute > 59)
			{
				return false;
			}
			return true;
	}



	TinyDateTime& TinyDateTime::SetTime(int iYear, int iMonth, int iDay, int iHour, int iMinute, int iSecond, int iMSecond)
	{
		if (!VaildTime(iYear, iMonth, iDay, iHour, iMinute, iSecond))
		{
			DefTime();
			U_IM_T1.m_isValid = 0;
			return *this;
		}

		if (iYear >= (BaseYear + (1<<5)))
		{
			U_IM_T1.m_isValid = 0;
			return *this;
		}

		U_IM_T1.m_year = iYear - BaseYear;
		U_IM_T1.m_month = iMonth;
		U_IM_T1.m_day = iDay;
		U_IM_T1.m_hour = iHour;
		U_IM_T1.m_min = iMinute;
		U_IM_T1.m_sec = iSecond;
		U_IM_T1.m_isValid = 1;
	    
		return *this;
	}

	TinyDateTime& TinyDateTime::SetTime(const std::string& strTime)
	{
		const char split[] = {4,2,2,2,2,2,3};  //年月日时分秒毫秒 每个字段最长的占位

		int tmpbuf[7] = {0};    //存放的是年月日时分秒的值
		char buf[8];            //缓存
		int i = 0;
		int j = 0;

		std::string::const_iterator it = strTime.begin();
		for (; it != strTime.end(); ++it)
		{
			if (isdigit((unsigned char)*it))
			{
				if (i<split[j])
				{
					buf[i++] = *it;
				}
				else
				{
					buf[i] = 0;
					tmpbuf[j] = atoi(buf);
					i = 0;
					buf[i++] = *it;
					j++;
				}
			}
			else
			{
				buf[i] = 0;
				if (i>0)
				{
					tmpbuf[j] = atoi(buf);

					i = 0;
					j++;
				}
			}
		}

		if (i<=split[j])
		{
			buf[i] = 0;
			tmpbuf[j] = atoi(buf);
		}

		return SetTime(tmpbuf[0], tmpbuf[1], tmpbuf[2], tmpbuf[3], tmpbuf[4], tmpbuf[5], tmpbuf[6]);
	}

	void TinyDateTime::DefTime()
	{
		U_IM_T1.m_year = 0;
		U_IM_T1.m_month = 1;
		U_IM_T1.m_day = 1;
		U_IM_T1.m_hour = 0;
		U_IM_T1.m_min = 0;
		U_IM_T1.m_sec = 0;
		U_IM_T1.m_isValid = 0;
	}

	time_t TinyDateTime::GetTime() const
	{
		struct tm stm;
		stm.tm_year = U_IM_T1.m_year + BaseYear - 1900;
		stm.tm_mon = U_IM_T1.m_month - 1;
		stm.tm_mday = U_IM_T1.m_day;
		stm.tm_hour = U_IM_T1.m_hour;
		stm.tm_min = U_IM_T1.m_min;
		stm.tm_sec = U_IM_T1.m_sec;

		return mktime(&stm);
	}

	std::string TinyDateTime::ToString(const std::string& format) const
	{
		if(format.empty() || 1 != U_IM_T1.m_isValid)
			return "";

		// 表示年、月、日、时、分、秒、毫秒的长度
		int pos[6];
		for(unsigned int i=0; i<sizeof(pos)/sizeof(pos[0]); i++)
			pos[i] = -1;

		const static int len[6] = {4,2,2,2,2,2};

		int val[6];
		val[0] = Year();
		val[1] = Month();
		val[2] = Day();
		val[3] = Hour();
		val[4] = Minute();
		val[5] = Second();

		for(unsigned int i=0; i<format.length(); i++)
		{
			char c = format[i];
			if(c == 'y')
			{
				if(format[i+1] == 'y' && format[i+2] == 'y' && format[i+3] == 'y')
				{
					pos[0] = i;
					i += 3;
				}
			}
			else if(c == 'M')
			{
				if(format[i+1] == 'M')
				{
					pos[1] = i;
					i += 1;
				}
			}
			else if(c == 'd')
			{
				if(format[i+1] == 'd')
				{
					pos[2] = i;
					i += 1;
				}
			}
			else if(c == 'h')
			{
				if(format[i+1] == 'h')
				{
					pos[3] = i;
					i += 1;
				}
			}
			else if(c == 'm')
			{
				if(format[i+1] == 'm')
				{
					pos[4] = i;
					i += 1;
				}
			}
			else if(c == 's')
			{
				if(format[i+1] == 's')
				{
					pos[5] = i;
					i += 1;
				}
			}
		}

		char buf[32];
		std::string ret = format;
		for(unsigned int i=0; i<sizeof(pos)/sizeof(pos[0]); i++)
		{
			if(pos[i] != -1)
			{
				std::string fmt = "%0";
	#ifdef _WIN32
				_snprintf
	#else
				snprintf
	#endif
					(buf, sizeof(buf) - 1, "%d", len[i]);
				fmt += buf;
				fmt += "d";

	#ifdef _WIN32
				_snprintf
	#else
				snprintf
	#endif
					(buf, sizeof(buf) - 1, fmt.c_str(), val[i]);

				ret.replace(pos[i], len[i], buf);
			}
		}

		return ret;
	}

	std::string TinyDateTime::CurrentToString(const std::string& format)
	{
		TinyDateTime tmpDateTime;
		return tmpDateTime.ToString(format);
	}

	TinyDateTime& TinyDateTime::AddTime(int n, int bitmap)
	{
		struct tm stm;
		stm.tm_year = U_IM_T1.m_year + 100;
		stm.tm_mon = U_IM_T1.m_month - 1;
		stm.tm_mday = U_IM_T1.m_day;
		stm.tm_hour = U_IM_T1.m_hour;
		stm.tm_min = U_IM_T1.m_min;
		stm.tm_sec = U_IM_T1.m_sec;

		switch (bitmap)
		{
		case YEAR:
			stm.tm_year += n;
			break;
		case MON:
			stm.tm_mon += n;
			break;
		case DAY:
			stm.tm_mday += n;
			break;
		case HOUR:
			stm.tm_hour += n;
			break;
		case MIN:
			stm.tm_min += n;
			break;
		case SEC:
			stm.tm_sec += n;
			break;
		default:
			return *this;
		}

		time_t addtm;
		addtm = mktime(&stm);
		if (addtm < 0)
		{
			U_IM_T1.m_isValid = 0;
			return *this;
		}
		struct tm addStm;
	#ifdef _WIN32
		localtime_s(&addStm, &addtm);
	#else
		localtime_r(&addtm, &addStm);
	#endif

		if (addStm.tm_year-BaseYear+1900 >= 1<<5)
		{
			U_IM_T1.m_isValid = 0;
			return *this;
		}
		U_IM_T1.m_year = addStm.tm_year - BaseYear + 1900;
		U_IM_T1.m_month = addStm.tm_mon + 1;
		U_IM_T1.m_day = addStm.tm_mday;
		U_IM_T1.m_hour = addStm.tm_hour;
		U_IM_T1.m_min = addStm.tm_min;
		U_IM_T1.m_sec = addStm.tm_sec;
		U_IM_T1.m_isValid = 1;

		return *this;
	}

	/*DateTime& DateTime::SetTime(const std::string& strTime, const std::string& strFormat)
	{
		std::string tmpbuf[4];
		int tmptime[4] = {0};
		bool valid = true;

		if (strTime.length() != strFormat.length())
			return *this;

		for (size_t i=0; i<strFormat.length(); ++i)
		{
			switch (strFormat[i])
			{
			case 'h':
				tmpbuf[0].push_back(strTime[i]);
				break;
			case 'm':
				tmpbuf[1].push_back(strTime[i]);
				break;
			case 's':
				tmpbuf[2].push_back(strTime[i]);
				break;
			case 'f':
				tmpbuf[3].push_back(strTime[i]);
				break;
			case ':':
			case ' ':
				break;
			default:
				valid = false;
				break;
			}
		}

		if (!valid)
			return *this;

		for (int i=0; i<4; ++i)
		{
			tmptime[i] = atoi(tmpbuf[i].c_str());
		}

		return SetTime(tmptime[0], tmptime[1], tmptime[2], tmptime[3]);
	}*/
}

