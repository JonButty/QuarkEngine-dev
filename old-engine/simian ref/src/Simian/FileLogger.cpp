/*************************************************************************/
/*!
\file		FileLogger.cpp
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

#include "Simian/FileLogger.h"
#include "Simian/DateTime.h"

namespace Simian
{
    FactoryPlant<LoggerBase, FileLogger> FileLogger::factoryPlant_
        (LogManager::InstancePtr(), LogManager::LT_FILE);

    void FileLogger::log_(LogManager::LoggerLevel level, const std::string& message)
    {
        SAssert(file_.is_open(), "File logger not loaded.");
        file_ <<  DateTime().ToString() << " | " << LogManager::LEVEL_TABLE[level] << " > " << message << std::endl;
    }

    //--------------------------------------------------------------------------

    bool FileLogger::Load( const std::string& filename )
    {
        file_.open(filename);
        return !file_.fail();
    }

    void FileLogger::Unload()
    {
        file_.close();
    }
}
#endif
