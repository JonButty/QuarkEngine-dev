#ifndef CORELOGGER_H
#define CORELOGGER_H

#include "config.h"

#if ENGINE_CORE_LOGENGINE_MATH == ENGINE_MATH_SSE

#include "logManager.h"

#include <sstream>

#define CORE_LOG(msg)\
{\
    std::stringstream _ss;\
    _ss << "Core> " << msg;\
    LOG(_ss.str());\
}

#define CORE_LOG_W(msg)\
{\
    std::wstringstream _ss;\
    _ss << "Core> " << msg;\
    LOG_W(_ss.str());\
}
#else

#define CORE_LOG(msg)
#define CORE_LOG_W(msg)
#endif

#endif