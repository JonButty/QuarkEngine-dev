#include "consoleLogger.h"
#include "os.h"
#include "loggerTypes.h"

#include <iostream>

namespace Util
{
    ConsoleLogger::ConsoleLogger()
        :   Logger(LT_DEBUG_CONSOLE)
    {
    }

    ConsoleLogger::~ConsoleLogger()
    {
    }

    void ConsoleLogger::load()
    {
        std::cout << "---------------------------- ENGINE DEBUG LOG --------------------------------" << std::endl;
    }

    void ConsoleLogger::log( const std::string& message )
    {
        std::cout << message << std::endl;
    }

    void ConsoleLogger::log( const std::wstring& message )
    {
        std::wcout << message << std::endl;
    }

    void ConsoleLogger::unload()
    {
    }
}