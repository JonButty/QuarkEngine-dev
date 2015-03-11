/*************************************************************************/
/*!
\file		FileLogger.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef SIMIAN_FILELOGGER_H
#define SIMIAN_FILELOGGER_H

#include "SimianPlatform.h"
#if SIMIAN_LOGGING == 1

#include "LoggerBase.h"
#include "TemplateFactory.h"
#include <string>
#include <fstream>

namespace Simian
{
    class SIMIAN_EXPORT FileLogger: public LoggerBase
    {
    private:
        static FactoryPlant<LoggerBase, FileLogger> factoryPlant_;
        std::ofstream file_;

        void log_(LogManager::LoggerLevel level, const std::string& message);
    public:
        FileLogger& operator=(const FileLogger&) { return *this; }
    public:
        bool Load(const std::string& filename);
        void Unload();

    };
}

#endif

#endif
