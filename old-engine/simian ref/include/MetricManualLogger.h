/******************************************************************************/
/*!
\file		MetricManualLogger.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_METRICMANUALLOGGER_H_
#define DESCENSION_METRICMANUALLOGGER_H_

#include "MetricLoggerBase.h"
#include "MetricLogManager.h"
#include "Simian/FileArraySubNode.h"
#include <sstream>

namespace Descension
{
    const static std::string METRIC_MANUAL_LOGGER_PREFIX = "ManualLog_";

    template < typename T >
    class MetricManualLogger : public MetricLoggerBase
    {
    private:
        T getter_;
    private:

        void initLogger_()
        {
            logNode_.Root()->AddData("LoggerType",std::string("ManualLogger"));
            logNode_.Root()->AddData("VarType",std::string(typeid(typename T::ReturnType).name()));
        }

    public:
        template < typename T >
        MetricManualLogger( const std::string& name, T getter)
            :   MetricLoggerBase(name),
                getter_(getter)
        {
            filePath_ += METRIC_MANUAL_LOGGER_PREFIX + name_;
        }

        template < typename T >
        MetricManualLogger( MetricManualLogger<T>& logger)
            :   MetricLoggerBase(logger.name_),
            timer_(0.0f),
            logInterval_(logger.logInterval_),
            getter_(getter)
        {
        }

        void LogVariable()
        {
            newEntry_(getter_());
        }
    };
}
#endif
