/******************************************************************************/
/*!
\file		MetricLogManager.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_METRICLOGMANAGER_H_
#define DESCENSION_METRICLOGMANAGER_H_

#include "MetricTimedLogger.h"
#include "MetricManualLogger.h"
#include "GetterFuncPtr.h"
#include "GetterMemFuncPtr.h"

#include "Simian/Singleton.h"
#include "Simian/SimianTypes.h"

#include <vector>

namespace Descension
{
    const static std::string METRIC_LOGGER_FILE_PATH = "MetricLogger/";
    const static std::string METRIC_LOGGER_FILE_EXT = ".log";

    class MetricLogManager : public Simian::Singleton<MetricLogManager>
    {
    private:
        unsigned long numFiles_;
        std::vector<MetricLoggerBase*> loggers_;
    private:

        // Adds Loggers
        template < typename T >
        T* addLogger_( const T& logger )
        {
            T* ptr = new T(logger);
            ptr->InitLogger();
            loggers_.push_back(ptr);
            return ptr;
        }

    public:
        MetricLogManager();
        ~MetricLogManager();

        // Adds any type of Logger
        template <typename U, typename T>
        U* AddLogger(std::string str, T getter)
        {
            U<T> logger(str, getter);
            return addLogger_(logger);
        }

        template <typename T>
        MetricTimedLogger<T>* AddTimedLogger(std::string str, T getter, Simian::f32 interval)
        {
            MetricTimedLogger<T> logger(str, getter, interval);
            return addLogger_(logger);
        }

        template <typename T>
        MetricManualLogger<T>* AddManualLogger(std::string str, T getter)
        {
            MetricManualLogger<T> logger(str, getter);
            return addLogger_(logger);
        }

        void Update( Simian::f32 dt );

        void SaveLog();
    public:
        template <typename T>
        static GetterFuncPtr<T> MakeFuncPtr(T(*func)())
        {
            return GetterFuncPtr<T>(func);
        }

        template <typename T, typename U>
        static GetterMemFuncPtr<T, U, T(U::*)()const> MakeMemFuncPtr(U* obj, T(U::*func)()const)
        {
            return GetterMemFuncPtr<T, U, T(U::*)()const>(obj, func);
        }

        template <typename T, typename U>
        static GetterMemFuncPtr<T, U, T(U::*)()> MakeMemFuncPtr(U* obj, T(U::*func)())
        {
            return GetterMemFuncPtr<T, U, T(U::*)()>(obj, func);
        }

    };
}

#if 0
#define DMakeMemFuncPtr(ptr,func) MetricLogManager::MakeMemFuncPtr(ptr,func)
#define DMakeMemFuncPtrExplicit(ret,dest,ptr,func) MetricLogManager::MakeMemFuncPtr<ret,dest>(ptr,func)
#define DMakeFuncPtr(func) MetricLog?Manager::MakeFuncPtr(func)

#define DAddTimedLogger(name,funcPtr, interval) MetricLogManager::Instance().AddTimedLogger(name,funcPtr,interval)
#define DAddManualLogger(name,funcPtr) MetricLogManager::Instance().AddManualLogger(name,funcPtr)

#define DManualLog(logger) logger->LogVariable()
#define DManualSaveLog(logger) if(logger)logger->SaveLog()

#define DLogManagerUpdate(dt) MetricLogManager::Instance().Update(dt)
#define DLogManagerSave() MetricLogManager::Instance().SaveLog()
#else
#define DMakeMemFuncPtr(ptr,func)
#define DMakeFuncPtr(func)

#define DAddTimedLogger(name,funcPtr,interval) NULL
#define DAddManualLogger(name,funcPtr) NULL

#define DManualLog(logger)
#define DManualSaveLog(logger)

#define DLogManagerUpdate(dt)
#define DLogManagerSave()
#endif // METRIC_LOGGING

#endif
