/*
 * =====================================================================================
 *
 *       Filename:  utility.h
 *    Description:  ������ĺ�����װ����������2014-4-30��
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

#ifndef _WIND_UTILITY_H_
#define _WIND_UTILITY_H_


//GUID����  ����ʱ,WIN32������ole32, Linux������libuuid, ��װlibuuid��, libuuid.so.1������e2fsprogs

#ifdef _WIN32
#include <objbase.h>
#else
#include <uuid/uuid.h>
typedef struct _GUID
{
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} GUID, UUID;
#endif

#include <string>

namespace windcom
{
namespace util
{
	typedef union
	{
		unsigned int  i;
		unsigned char c;
	}TEndian;

	inline bool LittleEndian()
	{
		static TEndian a;
		a.c = 1;
		return (a.i==1);
	}

    // ��������
    template<typename T>
    inline void Swap(T& a, T& b)
    {
        if(&a != &b)
        {
            T t = a;
            a = b;
            b = t;
        }
    }

    // �ֽڷ�תʵ�֣���д����
    template<typename T>
    void _ReverseImpl(T& v)
    {
        for (int i = 0, j = sizeof(v); i < j; ++i, --j)
        {
            Swap(*(char *)((char *)&v + i), *(char *)((char *)&v + j - 1));
        }
    }

	// �ֽڷ�תʵ�֣�����д����
	template<typename T>
	T _ReverseNoChangeImpl(T v)
	{
		_ReverseImpl<T>(v);
		return v;
	}

    // �ֽڷ�ת���غ��������޸Ĳ�����ֵ
    inline void Reverse(char& v) { _ReverseImpl<char>(v); }
    inline void Reverse(unsigned char& v) { _ReverseImpl<unsigned char>(v); }
    inline void Reverse(short& v) { _ReverseImpl<short>(v); }
    inline void Reverse(unsigned short& v) { _ReverseImpl<unsigned short>(v); }
    inline void Reverse(int& v) { _ReverseImpl<int>(v); }
    inline void Reverse(unsigned int& v) { _ReverseImpl<unsigned int>(v); }
    inline void Reverse(long& v) { _ReverseImpl<long>(v); }
    inline void Reverse(unsigned long& v) { _ReverseImpl<unsigned long>(v); }
    inline void Reverse(long long& v) { _ReverseImpl<long long>(v); }
    inline void Reverse(unsigned long long& v) { _ReverseImpl<unsigned long long>(v); }
	inline void Reverse(std::string& v)
	{
		for (size_t i = 0, j = v.length(); i < j; ++i, --j)
		{
			Swap(v[i], v[j - 1]);
		}
	}

	// �ֽڷ�ת���غ��������޸Ĳ�����ֵ
	inline char ReverseR(char v) { return _ReverseNoChangeImpl<char>(v); }
	inline unsigned char ReverseR(unsigned char v) { return _ReverseNoChangeImpl<unsigned char>(v); }
	inline short ReverseR(short v) { return _ReverseNoChangeImpl<short>(v); }
	inline unsigned short ReverseR(unsigned short v) { return _ReverseNoChangeImpl<unsigned short>(v); }
	inline int ReverseR(int v) { return _ReverseNoChangeImpl<int>(v); }
	inline unsigned int ReverseR(unsigned int v) { return _ReverseNoChangeImpl<unsigned int>(v); }
	inline long ReverseR(long v) { return _ReverseNoChangeImpl<long>(v); }
	inline unsigned long ReverseR(unsigned long v) { return _ReverseNoChangeImpl<unsigned long>(v); }
	inline long long ReverseR(long long v) { return _ReverseNoChangeImpl<long long>(v); }
	inline unsigned long long ReverseR(unsigned long long v) { return _ReverseNoChangeImpl<unsigned long long>(v); }


    // GUID
    // ����GUID���������ַ���
    std::string Guid(void);
    // ��GUIDת�����ַ�����ʽ
    std::string GuidToString(const GUID &guid);
}
}

#endif
