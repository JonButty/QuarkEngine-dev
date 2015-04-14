#ifndef GFXLOGGER_H
#define GFXLOGGER_H

#include "config.h"

#if ENGINE_GFX_LOGENGINE_MATH == ENGINE_MATH_SSE

#include "logManager.h"

#include <sstream>

#define GFX_LOG(msg)\
{\
    std::stringstream _ss;\
    _ss << "Graphics> " << msg;\
    LOG(_ss.str());\
}

#define GFX_LOG_W(msg)\
{\
    std::wstringstream _ss;\
    _ss << "Graphics> " << msg;\
    LOG_W(_ss.str());\
}

#else

#define GFX_LOG(msg){}
#define  GFX_LOG_W(msg){}

#endif

#endif