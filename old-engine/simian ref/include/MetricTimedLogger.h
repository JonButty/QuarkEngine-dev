/******************************************************************************/
/*!
\file		MetricTimedLogger.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_METRICLOGGER_H_
#define DESCENSION_METRICLOGGER_H_

#include "MetricLoggerBase.h"
#include "MetricLogManager.h"
#include "Simian/FileArraySubNode.h"

namespace Descension
{
    const static std::string METRIC_TIMED_LOGGER_PREFIX = "TimedLog_";

    template < typename T >
    class MetricTimedLogger : public MetricLoggerBase
    {
    private:
        Simian::f32 timer_;
        Simian::f32 logInterval_;
        T getter_;
    private:

        void initLogger_()
        {
            logNode_.Root()->AddData("LoggerType",std::string("TimedLogger"));
            logNode_.Root()->AddData("LogInterval",logInterval_);
            logNode_.Root()->AddData("VarType","Vector3");
        }

    public:
        template < typename T >
        MetricTimedLogger( const std::string& name, T getter, Simian::f32 interval)
            :   MetricLoggerBase(name),
                timer_(0.0f),
                logInterval_(interval),
                getter_(getter)
        {
            filePath_ += METRIC_TIMED_LOGGER_PREFIX + name_;
        }

        template < typename T >
        MetricTimedLogger( MetricTimedLogger<T>& logger)
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

        void Update(Simian::f32 dt)
        {
            if(timer_ < logInterval_)
            {
                timer_ += dt;
                return;
            }

            timer_ = 0.0f;
            LogVariable();
        }
    };
}

#endif
