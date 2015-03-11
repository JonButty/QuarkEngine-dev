/*************************************************************************/
/*!
\file		LoggerBase.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef SIMIAN_LOGGERBASE_H_
#define SIMIAN_LOGGERBASE_H_

#include "SimianPlatform.h"
#if SIMIAN_LOGGING == 1

#include "LogManager.h"

namespace Simian
{
    class SIMIAN_EXPORT LoggerBase
    {
    private:
        LogManager::LoggerLevel minLevel_;
    public:
        LogManager::LoggerLevel MinLevel() const;
        void MinLevel(LogManager::LoggerLevel level);
    protected:
        virtual void log_(LogManager::LoggerLevel level, const std::string& message) = 0;
    public:
        LoggerBase();
        virtual ~LoggerBase();

        void Log(LogManager::LoggerLevel level, const std::string& message);
    };
}

#endif

#endif
