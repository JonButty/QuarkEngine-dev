/*************************************************************************/
/*!
\file		StdoutLogger.cpp
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

#include "Simian/StdoutLogger.h"
#include "Simian/DateTime.h"
#include <iostream>

namespace Simian
{
    FactoryPlant<LoggerBase, StdoutLogger> StdoutLogger::factoryPlant_
        (LogManager::InstancePtr(), LogManager::LT_STDOUT);

    void StdoutLogger::log_(LogManager::LoggerLevel level, const std::string& message)
    {
        std::cout << DateTime().ToString() << " | " << LogManager::LEVEL_TABLE[level] << " > " << message << std::endl;
    }
}

#endif
