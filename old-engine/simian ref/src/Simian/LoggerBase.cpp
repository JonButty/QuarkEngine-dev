/*************************************************************************/
/*!
\file		LoggerBase.cpp
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

#include "Simian/LoggerBase.h"

namespace Simian
{
    LoggerBase::LoggerBase()
        : minLevel_(LogManager::LL_VERBOSE)
    {
    }

    LoggerBase::~LoggerBase()
    {
    }

    //--------------------------------------------------------------------------

    LogManager::LoggerLevel LoggerBase::MinLevel() const
    {
        return minLevel_;
    }

    void LoggerBase::MinLevel(LogManager::LoggerLevel level)
    {
        minLevel_ = level;
    }

    //--------------------------------------------------------------------------

    void LoggerBase::Log(LogManager::LoggerLevel level, const std::string& message)
    {
        if (level >= minLevel_)
            log_(level, message);
    }
}

#endif
