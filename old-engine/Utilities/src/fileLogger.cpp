#include "fileLogger.h"
#include "loggerTypes.h"

namespace Util
{
    FileLogger::FileLogger(unsigned int type)
        :   Logger(type)
    {
    }

    FileLogger::~FileLogger()
    {
    }

    //--------------------------------------------------------------------------

    void FileLogger::log( const std::string& message )
    {
        file_ << message;
    }

    void FileLogger::log( const std::wstring& message )
    {
        fileW_ << message;
    }

    void FileLogger::unload()
    {
        file_.close();
        fileW_.close();
    }
}