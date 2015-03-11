#ifndef DEBUGFILELOGGER_H
#define DEBUGFILELOGGER_H

#include "configUtilitiesDLL.h"
#include "fileLogger.h"
#include "singleton.h"

namespace Util
{
    class ENGINE_UTILITIES_EXPORT DebugFileLogger
        :   public FileLogger,
            public Singleton<DebugFileLogger>
    {
    public:
        DebugFileLogger();
        ~DebugFileLogger();
    protected:
        void load();
        void log(const std::string& message);
        void log(const std::wstring& message);
    };
}

#endif