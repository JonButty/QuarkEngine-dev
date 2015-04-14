/*************************************************************************/
/*!
\file		LogManager.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Simian/SimianPlatform.h"
#if SIMIAN_LOGGING == 1

#include "Simian/LogManager.h"
#include "Simian/LoggerBase.h"

#include <algorithm>

namespace Simian
{
    static const u32 INITIAL_LOGGER_COUNT = 5;

    u32 LogManager::totalLoggers_ = LogManager::LT_TOTAL;

    const std::string LogManager::LEVEL_TABLE[] =
    {
        "V",
        "D",
        "W",
        "E",
        "F"
    };

    LogManager::LogManager()
        : Factory(totalLoggers_), 
          minLevel_(LL_VERBOSE)
    {
        activeLoggers_.reserve(INITIAL_LOGGER_COUNT);
    }
    
    //--------------------------------------------------------------------------
    
    void LogManager::MinLevel(LoggerLevel minLevel)
    {
        minLevel_ = minLevel;
        
        // Sets all logger's minLevel_ to current minLevel_
        for (u32 i = 0; i < activeLoggers_.size(); ++i)
            activeLoggers_[i]->MinLevel(minLevel_);
    }

    LogManager::LoggerLevel LogManager::MinLevel() const
    {
        return minLevel_;
    }

    void LogManager::TotalLoggers(u32 totalLoggers)
    {
        totalLoggers_ = totalLoggers;
    }

    //--------------------------------------------------------------------------
            
    void LogManager::Log(LoggerLevel level, const std::string& text)
    {
        for (u32 i = 0; i < activeLoggers_.size(); ++i)
            activeLoggers_[i]->Log(level, text);
    }

    void LogManager::OnCreateInstance(LoggerBase* logger)
    {
        logger->MinLevel(minLevel_);
        activeLoggers_.push_back(logger);
    }

    void LogManager::OnDestroyInstance(LoggerBase* logger)
    {
        std::remove(activeLoggers_.begin(), activeLoggers_.end(), logger);
    }
}

#endif
