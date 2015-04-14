/*************************************************************************/
/*!
\file		LogManager.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef SIMIAN_LOGMANAGER_H
#define SIMIAN_LOGMANAGER_H

#include "SimianPlatform.h"

#if SIMIAN_LOGGING == 1

#include "TemplateFactory.h"
#include "Singleton.h"
#include <string>
#include <sstream>
#include <vector>

namespace Simian
{
    class LoggerBase;

    class SIMIAN_EXPORT LogManager
        : public Singleton<LogManager>,
          public Factory<LoggerBase>
    {
    public:
        enum LoggerTypes
        {
            LT_STDOUT,
            LT_FILE,
            LT_TOTAL
        };

        enum LoggerLevel
        {
            LL_VERBOSE,
            LL_DEBUG,
            LL_WARNING,
            LL_ERROR,
            LL_FATAL
        };

        static const std::string LEVEL_TABLE[];
    private:
        LoggerLevel minLevel_;
        std::vector<LoggerBase*> activeLoggers_;
        static u32 totalLoggers_;
    public:
        void MinLevel(LoggerLevel minLevel);
        LoggerLevel MinLevel() const;

        static void TotalLoggers(u32 num);
    public:
        LogManager();
        
        void Log(LoggerLevel level, const std::string& text);

        void OnCreateInstance(LoggerBase* logger);
        void OnDestroyInstance(LoggerBase* logger);
    };

    SIMIAN_IMPORT template class SIMIAN_EXPORT Singleton<LogManager>;
}

#define SLog(level, msg) \
    { \
        std::stringstream ss__; \
        ss__ << msg; \
        Simian::LogManager::Instance().Log(level, ss__.str()); \
    }

#else

#define SLog(level, msg)

#endif

#define SVerbose(msg) SLog(Simian::LogManager::LL_VERBOSE, msg)
#define SDebug(msg)   SLog(Simian::LogManager::LL_DEBUG, msg)
#define SWarn(msg)    SLog(Simian::LogManager::LL_WARNING, msg)
#define SError(msg)   SLog(Simian::LogManager::LL_ERROR, msg)
#define SFatal(msg)   SLog(Simian::LogManager::LL_FATAL, msg)

#endif
