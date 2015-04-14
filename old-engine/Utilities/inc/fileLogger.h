#ifndef FILELOGGER_H
#define FILELOGGER_H

#include "configUtilitiesDLL.h"
#include "logger.h"

#include <fstream>

namespace Util
{
    class ENGINE_UTILITIES_EXPORT FileLogger
        :   public Logger
    {
    public:
        FileLogger(unsigned int type);
        virtual ~FileLogger();
    protected:
        void log(const std::string& message);
        void log(const std::wstring& message);
        void unload();
    protected:
        std::ofstream file_;
        std::wofstream fileW_;
    };
}

#endif