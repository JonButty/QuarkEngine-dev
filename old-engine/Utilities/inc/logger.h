#ifndef LOGGER_H
#define LOGGER_H

#include "configUtilitiesDLL.h"

#include <string>

namespace Util
{
    class ENGINE_UTILITIES_EXPORT Logger
    {
    public:
        Logger(unsigned int type);
        virtual ~Logger();
        void Load();
        void Log(const std::string& message);
        void Log(const std::wstring& message);
        void Unload();
    public:
        bool Enable() const;
        void Enable(bool val);
        unsigned int Type() const;
        void Type(unsigned int val);
    protected:
        virtual void load();
        virtual void log(const std::string&) = 0;
        virtual void log(const std::wstring&) = 0;
        virtual void unload();;
    private:
        bool enable_;
        unsigned int type_;
    };
}

#endif