//
//  lava_crt.h
//
//  Created by Colin on 2019-07-15.
//  Copyright (c) 2019 Sumscope. All rights reserved.
//
#pragma once
#include <time.h>
#include <string.h>
#include <functional>
#include <algorithm>
#include <vector>
#include "lava_base.h"

namespace lava
{
	namespace utils
	{
		inline void strlwr(__lv_inout char* str_)
		{
		#ifdef LAVA_WIN
			::_strlwr_s(str_, strlen(str_) + 1);
		#else // LAVA_LINUX or LAVA_MACOSX
			for (char* p = str_; *p; ++p)
			{
				if ('A' <= *p && *p <= 'Z')
					*p += 'a' - 'A';
			}
		#endif
		}

		inline void strupr(__lv_inout char* str_)
		{
		#ifdef LAVA_WIN
			::_strupr_s(str_, strlen(str_) + 1);
		#else // LAVA_LINUX or LAVA_MACOSX
			for (char* p = str_; *p; ++p)
			{
				if ('a' <= *p && *p <= 'z')
					*p += 'A' - 'a';
			}
		#endif
		}

		inline void split(char* line, const char* delimiters, std::vector<std::string>& ret)
		{
		#ifdef LAVA_WIN
			char* src = _strdup(line);
			char* next_token;
			char* token = ::strtok_s(src, delimiters, &next_token);
			while (token)
			{
				ret.push_back(token);
				token = ::strtok_s(NULL, delimiters, &next_token);
			}
			free(src);
		#else // LAVA_LINUX or LAVA_MACOSX
			char* src = strdup(line);
			char* token = ::strtok(src, delimiters);
			while (token)
			{
				ret.push_back(token);
				token = ::strtok(NULL, delimiters);
			}
			free(src);
		#endif
		}

		//determine whether the des string contains a sub string
		inline bool isInclude(const std::string &des, const std::string &sub)
		{
			std::string::size_type idx = des.find(sub);
			if (std::string::npos != idx) { return true; }
			return false;
		}

		inline void localtime(struct tm* t, const time_t* val)
		{
		#ifdef LAVA_WIN
			::localtime_s(t, val);
		#else // LAVA_LINUX or LAVA_MACOSX
			::localtime_r(val, t);
		#endif
		}

		inline std::string& trim_l(std::string& s)
		{
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(isspace))));
			return s;
		}

		inline std::string& trim_r(std::string& s)
		{
			s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(isspace))).base(), s.end());
			return s;
		}

		inline std::string& trim(std::string& s)
		{
			return trim_l(trim_r(s));
		}
	}
}
