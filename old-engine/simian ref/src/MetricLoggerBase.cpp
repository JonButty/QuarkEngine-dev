/******************************************************************************/
/*!
\file		MetricLoggerBase.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "MetricLoggerBase.h"
#include "MetricLogManager.h"

#include "Simian/FileArraySubNode.h"

#include <sstream>

namespace Descension
{
    MetricLoggerBase::MetricLoggerBase(std::string name )
        :   enable_(true),
            arrayNodePtr_(0),
            name_(name),
            filePath_(METRIC_LOGGER_FILE_PATH)
    {
    }

    MetricLoggerBase::MetricLoggerBase( const MetricLoggerBase& logger )
        :   enable_(true),
            arrayNodePtr_(0),
            name_(logger.name_),
            filePath_(logger.filePath_)
    {
        logNode_.Root(logger.logNode_.Root());\
    }

    //--------------------------------------------------------------------------

    Simian::DataFileIOXML& MetricLoggerBase::LogNode()
    {
        return logNode_;
    }

    const std::string& MetricLoggerBase::Name() const
    {
        return name_;
    }

    //-Private------------------------------------------------------------------

    void MetricLoggerBase::logData_( Simian::FileObjectSubNode* obj, const Simian::Vector3& val )
    {
        std::stringstream ss;
        ss << val.X() << "," << val.Y() << "," << val.Z() << '\0';
        obj->AddData("Data", ss.str());
    }

    //-Public-------------------------------------------------------------------

    void MetricLoggerBase::SaveLog()
    {
        logNode_.Write(filePath_ + METRIC_LOGGER_FILE_EXT);
    }

    void MetricLoggerBase::InitLogger()
    {
        logNode_.Root()->Name(name_);
        arrayNodePtr_ = logNode_.Root()->AddArray("Entries");
        initLogger_();
    }
}
