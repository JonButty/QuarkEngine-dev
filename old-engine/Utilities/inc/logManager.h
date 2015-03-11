#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include "configUtilitiesDLL.h"
#include "singleton.h"
#include "loggerTypes.h"

#include <string>
#include <sstream>
#include <list>

namespace Util
{
    class Logger;

    class ENGINE_UTILITIES_EXPORT LogManager
        :   public Singleton<LogManager>
    {
    public:
        LogManager();
        ~LogManager();
        void Load();
        void RegisterLogger(Logger* logger);
        void EnableLogger(unsigned int type,
                          bool enable);
        void Log(const std::string& message);
        void Log(const std::wstring& message);
        void Log(unsigned int type,
                 const std::string& message);
        void Log(unsigned int type,
                 const std::wstring& message);
        void Unload();
    private:
        typedef std::list<Logger*> LoggerList;
    private:
        LoggerList* loggers_[LT_TOTAL];
    };
}

#if 1
#define LOG(msg)\
{\
    std::stringstream __ss;\
    __ss << msg;\
    Util::LogManager::Instance().Log(Util::LT_DEBUG_CONSOLE,__ss.str());\
    Util::LogManager::Instance().Log(Util::LT_DEBUG_FILE,__ss.str());\
}

#define LOG_W(msg)\
{\
    std::wstringstream __ss;\
    __ss << msg;\
    Util::LogManager::Instance().Log(Util::LT_DEBUG_CONSOLE,__ss.str());\
    Util::LogManager::Instance().Log(Util::LT_DEBUG_FILE,__ss.str());\
}
#else
#define LOG(msg)
#define LOG_W(msg)
#endif

#endif