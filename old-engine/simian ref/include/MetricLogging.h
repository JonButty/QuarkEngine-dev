#ifdef METRIC_LOGGING

#include "MetricLogger.h"
#include "MetricLoggerMaster.h"
#include "Simian/Singleton.h"

template <typename T>
void LogVariable(std::string str, T getter)
{
    Descension::MetricTimedLogger<T> logger(str, getter);
    Descension::MetricLogManager::Instance().addLogger_(logger);
}

#define MAKE_FUNC_PTR(name,retType, func) GetterFuncPtr <retType> name(reType(*func))
#define MAKE_MEM_CONST_FUNC_PTR(name, objType, obj, retType, func) GetterMemFuncPtr <retType, objType, retType(objType::*)()const>name(obj, func)
#define MAKE_MEM_FUNC_PTR(name, objType, obj, retType, func) GetterMemFuncPtr <retType, objType, retType(objType::*)()>name(obj, func)
#define LOG_VARIABLE(name,string) AddLogger(string,name)
//#include "MetricLogger.h"
//#define METRIC_LOGGER( logger_, type ) Descension::MetricLogger <type> logger_
//#define METRIC_LOGGER_INIT( lo0gger_, var ) logger_.Init(var)
#else
#define MAKE_FUNC_PTR 
#define MAKE_MEM_CONST_FUNC_PTR 
#define MAKE_MEM_FUNC_PTR 
#define LOG_VARIABLE 
#endif
