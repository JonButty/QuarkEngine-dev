#ifndef UTILLOGGER_H
#define UTILLOGGER_H

#include "config.h"

#if ENGINE_UTIL_LOGENGINE_MATH == ENGINE_MATH_SSE

#include "logManager.h"

#include <sstream>

#define UTIL_LOG(msg)\
{\
    std::stringstream _ss;\
    _ss << "Utilities> " << msg;\
    LOG(_ss.str());\
}

#define UTIL_LOG_W(msg)\
{\
    std::wstringstream _ss;\
    _ss << "Utilities> " << msg;\
    LOG_W(_ss.str());\
}

#else

#define UTIL_LOG(msg)
#define UTIL_LOG_W(msg)

#endif

#endif