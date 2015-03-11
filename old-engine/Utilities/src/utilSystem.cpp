#include "utilSystem.h"
#include "debugFileLogger.h"
#include "consoleLogger.h"
#include "logManager.h"
#include "inputDeviceManager.h"
#include "utilLogger.h"
#include "console.h"
#include "os.h"

namespace Util
{
    System::System()
    {
    }

    System::~System()
    {
    }

    void System::Load()
    {
        Console::Instance().Show();
        OS::Instance().Load();
        DebugFileLogger::Instance();
        ConsoleLogger::Instance();
        LogManager::Instance().Load();
        InputDeviceManager::Instance().Load();
        UTIL_LOG("Utilities loaded");
    }

    void System::Unload()
    {
        InputDeviceManager::Instance().Unload();
        LogManager::Instance().Unload();
        OS::Instance().Unload();
    }
}