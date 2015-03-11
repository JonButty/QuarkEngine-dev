/******************************************************************************/
/*!
\file		MetricLogManager.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "Simian/EnginePhases.h"
#include "Simian/Delegate.h"
#include "Simian/FileArraySubNode.h"
#include "StateGame.h"
#include "Simian/DateTime.h"

#include "MetricLogManager.h"
#include "MetricLoggerBase.h"
#include "Simian/LogManager.h"

namespace Descension
{
    const static size_t NUM_RESERVED_LOGGERS = 6;

    MetricLogManager::MetricLogManager()
        :   numFiles_(0)
    {
        loggers_.reserve(NUM_RESERVED_LOGGERS);
    }

    MetricLogManager::~MetricLogManager()
    {
        Simian::u32 size = loggers_.size();

        for( Simian::u32 i = 0; i < size; ++i )
            delete loggers_[i];
        loggers_.clear();
    }

    // Change to writing to a xml file
    void MetricLogManager::Update(Simian::f32 dt)
    {
        Simian::u32 size = loggers_.size();

        // Only write when time interval has reached
        for( Simian::u32 i = 0; i < size; ++i )
            loggers_[i]->Update(dt);
    }

    void MetricLogManager::SaveLog()
    {
        Simian::u32 size = loggers_.size();
        Simian::DataFileIOXML file;
        Simian::FileRootSubNode* root = file.Root();
        root->Name("Root");
        Simian::FileArraySubNode* array = root->AddArray("Loggers");

        // Only write when time interval has reached
        for( Simian::u32 i = 0; i < size; ++i )
        {
            Simian::FileRootSubNode* root = loggers_[i]->LogNode().Root();
            
            // Logger is empty
            if(root->Array("Entries")->Size() == 0)
               continue;

            array->AddObject("Logger")->AddChild(root);
        }
        
        // if the array is empty
        if( array->Size() == 0 )
            array->AddObject("Logger")->AddData("NULL","NULL");

        loggers_.clear();

        Simian::DateTime date;
        std::string level(StateGame::Instance().LevelFile());
        size_t startOfLevel = level.find_last_of('/');

        std::stringstream ss;
        ss << METRIC_LOGGER_FILE_PATH << level.substr(startOfLevel + 1,level.size() - startOfLevel - 5)
        << "-" << static_cast<int>(date.Month()) << static_cast<int>(date.Day()) << static_cast<int>(date.Hour()) << static_cast<int>(date.Minute()) 
        << static_cast<int>(date.Second()) << METRIC_LOGGER_FILE_EXT << '\0';

        file.Write(ss.str());
        ++numFiles_;
    }
}
