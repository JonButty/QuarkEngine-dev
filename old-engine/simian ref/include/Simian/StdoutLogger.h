/*************************************************************************/
/*!
\file		StdoutLogger.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef SIMIAN_STDOUTLOGGER_H
#define SIMIAN_STDOUTLOGGER_H

#include "SimianPlatform.h"
#if SIMIAN_LOGGING == 1

#include "LoggerBase.h"
#include "TemplateFactory.h"

namespace Simian
{
    class SIMIAN_EXPORT StdoutLogger: public LoggerBase
    {
    private:
        static FactoryPlant<LoggerBase, StdoutLogger> factoryPlant_;
        
        void log_(LogManager::LoggerLevel level, const std::string& message);
    };
}

#endif

#endif
