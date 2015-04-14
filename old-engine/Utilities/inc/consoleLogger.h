#ifndef CONSOLELOGGER_H
#define CONSOLELOGGER_H

#include "logger.h"
#include "singleton.h"

namespace Util
{
    class ConsoleLogger
        :   public Logger,
            public Singleton<ConsoleLogger>
    {
    public:
        ConsoleLogger();
        ~ConsoleLogger();
    protected:
        void load();
        void log(const std::string& message);
        void log(const std::wstring& message);
        void unload();
    };
}

#endif