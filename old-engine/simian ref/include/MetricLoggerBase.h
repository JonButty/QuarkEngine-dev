/******************************************************************************/
/*!
\file		MetricLoggerBase.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_METRICLOGGERBASE_H_
#define DESCENSION_METRICLOGGERBASE_H_

#include "Simian/DataFileIOXML.h"
#include "Simian/Vector3.h"
#include "Simian/Matrix.h"

#include <string>

namespace Descension
{
    class MetricLoggerBase
    {
    protected:
        bool enable_;
        Simian::DataFileIOXML logNode_;
        Simian::FileArraySubNode* arrayNodePtr_;
        std::string name_;
        std::string filePath_;
    public:
        Simian::DataFileIOXML& LogNode();
        const std::string& Name()const;
    protected:
        template <typename U>
        void logData_(Simian::FileObjectSubNode* obj, const U& val)
        {
            obj->AddData("Data", val);
        }

        void logData_(Simian::FileObjectSubNode* obj, const Simian::Vector3& val);

        template <typename T>
        Simian::FileObjectSubNode* newEntry_(const T& val)
        {
            Simian::FileObjectSubNode* obj = arrayNodePtr_->AddObject("Entry");
            obj->AddData("Index",arrayNodePtr_->Size());
            logData_(obj,val);
            return obj;
        }

        virtual void initLogger_() = 0;
        Simian::FileObjectSubNode* logVariable_();
    public:
        MetricLoggerBase( std::string name = "");
        MetricLoggerBase(const MetricLoggerBase& logger);
        virtual ~MetricLoggerBase(){}

        void SaveLog();
        void InitLogger();
        virtual void LogVariable() = 0;
        virtual void Update(Simian::f32) {};
    };
}
#endif
