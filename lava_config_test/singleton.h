//
//  singleton.h
//
//  Created by Colin on 2019-07-25.
//  Copyright (c) 2019 Sumscope. All rights reserved.
//
#pragma once 
#include <mutex>

template < class T, class I = T >
class singleton_lazy
{
public:
	static I* instance()
	{
		//
		// double check
		//
		if ( s_instance == nullptr )
		{
			std::lock_guard<std::mutex> guard(s_mutex);
			if ( s_instance == nullptr )
				s_instance = new T();
		}

		return static_cast<I*>(s_instance);
	}

	static void release()
	{
		delete s_instance;
		s_instance = nullptr;
	}

private:
	static std::mutex s_mutex;	
	static T*	 s_instance;
};

template<class T, class I>
/*__declspec(selectany)*/ std::mutex singleton_lazy<T, I>::s_mutex;

template<class T, class I> 
/*__declspec(selectany)*/ T* singleton_lazy<T, I>::s_instance = nullptr;

template < class T, class I = T >
class singleton_hungry
{
public:
	I* instance()
	{
		return static_cast<I*>(&s_instance);
	}

private:
	static T	 s_instance;
};

template<class T, class I>
/*__declspec(selectany)*/ T singleton_hungry<T, I>::s_instance;