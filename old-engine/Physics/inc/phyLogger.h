#ifndef PHYLOGGER_H
#define PHYLOGGER_H

#include "config.h"

#if ENGINE_PHY_LOGENGINE_MATH == ENGINE_MATH_SSE

#include "logManager.h"

#include <sstream>

#define PHY_LOG(msg)\
{\
    std::stringstream _ss;\
    _ss << "Physics> " << msg;\
    LOG(_ss.str());\
}

#define PHY_LOG_W(msg)\
{\
    std::wstringstream _ss;\
    _ss << "Physics> " << msg;\
    LOG_W(_ss.str());\
}

#else

#define PHY_LOG(msg)
#define PHY_LOG_W(msg)

#endif


#endif