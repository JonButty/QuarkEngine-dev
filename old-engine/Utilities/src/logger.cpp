#include "logger.h"
#include "logManager.h"

namespace Util
{
    Logger::Logger(unsigned int type)
        :   enable_(true),
            type_(type)
    {
        LogManager::Instance().RegisterLogger(this);
    }

    Logger::~Logger()
    {
    }

    void Logger::Load()
    {
        load();
    }

    void Logger::Log( const std::string& message )
    {
        if(enable_)
            log(message);
    }

    void Logger::Log( const std::wstring& message )
    {
        if(enable_)
            log(message);
    }

    void Logger::Unload()
    {
        unload();
    }

    //--------------------------------------------------------------------------

    bool Logger::Enable() const
    {
        return enable_;
    }

    void Logger::Enable( bool val )
    {
        enable_ = val;
    }

    unsigned int Logger::Type() const
    {
        return type_;
    }

    void Logger::Type( unsigned int val )
    {
        type_ = val;
    }

    //--------------------------------------------------------------------------

    void Logger::load()
    {
    }

    void Logger::unload()
    {
    }
}