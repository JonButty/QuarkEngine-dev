#include "debugFileLogger.h"
#include "paths.h"
#include "utilAssert.h"
#include "os.h"
#include "loggerTypes.h"

static const std::string DEBUG_FILE("log.txt");

namespace Util
{
    DebugFileLogger::DebugFileLogger()
        :   FileLogger(LT_DEBUG_FILE)
    {
    }

    DebugFileLogger::~DebugFileLogger()
    {
    }

    //--------------------------------------------------------------------------

    void DebugFileLogger::load()
    {
        file_.open(DATA_DIR + DEBUG_FILE);
        fileW_.open(DATA_DIR + DEBUG_FILE);
        ENGINE_ASSERT_MSG(file_.is_open() || fileW_.is_open(), "Unable to create debug file.");
        file_ << "---------------------------- ENGINE DEBUG LOG --------------------------------" << std::endl;
    }

    void DebugFileLogger::log( const std::string& message )
    {
        file_ << message << std::endl;
    }

    void DebugFileLogger::log( const std::wstring& message )
    {
        fileW_ << message << std::endl;
    }
}