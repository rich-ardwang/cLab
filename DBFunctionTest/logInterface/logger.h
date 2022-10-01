/*
 * =====================================================================================
 *
 *       Filename:  logger.h
 *    Description:  日志操作相关函数封装（丁富来：2014-5-14）
 *        Version:  1.0
 *        Created:  2014-5-14
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  dingfulai
 *        Company:  wind, Inc.
 *
 * =====================================================================================
 */

#ifndef _WIND3RD_LOGGER_H_
#define _WIND3RD_LOGGER_H_

#include <string>

#ifdef _WIN32
#include "./log4cxx/header/log4cxx/logger.h"
// Appender头文件
#include "./log4cxx/header/log4cxx/asyncappender.h"
// 配置器头文件
#include "./log4cxx/header/log4cxx/propertyconfigurator.h"
#include "./log4cxx/header/log4cxx/basicconfigurator.h"
#include "./log4cxx/header/log4cxx/xml/domconfigurator.h"
// 其他
#include "./log4cxx/header/log4cxx/logmanager.h"
#else
#include "logger.h"
#include "asyncappender.h"
#include "propertyconfigurator.h"
#include "basicconfigurator.h"
#include "xml/domconfigurator.h"
#include "logmanager.h"
#endif

#define GETLOGGER log4cxx::Logger::getLogger

namespace wind3rd
{

    // DomConfigurator初始化类
    class DomLogInit
    {
    public:
        DomLogInit(const std::string& config)
        {
            log4cxx::xml::DOMConfigurator::configure(config);
        }

        ~DomLogInit()
        {
            log4cxx::LogManager::shutdown();
        }
    };

    // PropertyConfigurator初始化类
    class ProLogInit
    {
    public:
        ProLogInit(const std::string& config)
        {
            log4cxx::PropertyConfigurator::configureAndWatch(config);
        }

        ~ProLogInit()
        {

        }
    };
}

#endif
