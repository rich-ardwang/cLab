/*
 * =====================================================================================
 *
 *       Filename:  type.h
 *    Description:  �����������ͷ�װ����������2014-4-30��
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

#ifndef _WIND_TYPE_H_
#define _WIND_TYPE_H_

#include <string>

namespace windcom
{
#if 0
#ifdef _WIN32
    // �������Ͷ���
    typedef char int8_t;
    typedef short int16_t;
    typedef int int32_t;
    typedef long long int64_t;

    typedef unsigned char uint8_t;
    typedef unsigned short uint16_t;
    typedef unsigned int uint32_t;
    typedef unsigned long long uint64_t;
#endif
#endif

    typedef std::string string_t;


    // ������
    enum ReturnValue
    {
        Failure = -1,
        Success = 0
    };

}
//�Զ���ö�ٵĺ֧꣬��ö�����ظ���
#define DECLARE_ENUM(E) \
    struct E \
    { \
    public: \
        E(int value = 0) : _value((__Enum)value) { \
        } \
        E& operator=(int value) { \
            this->_value = (__Enum)value; \
            return *this; \
        } \
        operator int() const { \
            return this->_value; \
        } \
        \
        enum __Enum

#define END_ENUM() \
    ; \
    private: \
        __Enum _value; \
    };

#endif
