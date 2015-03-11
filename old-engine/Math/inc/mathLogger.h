#ifndef MATHLOGGER_H
#define MATHLOGGER_H

#include "config.h"

#if ENGINE_MATH_LOGENGINE_MATH == ENGINE_MATH_SSE

#include "logManager.h"

#include <sstream>

#define MATH_LOG(msg)\
{\
    std::stringstream _ss;\
    _ss << "Math> " << msg;\
    LOG(_ss.str());\
}

#define MATH_LOG_W(msg)\
{\
    std::wstringstream _ss;\
    _ss << "Math> " << msg;\
    LOG_W(_ss.str());\
}

#else

#define MATH_LOG(msg)
#define MATH_LOG_W(msg)

#endif

#endif