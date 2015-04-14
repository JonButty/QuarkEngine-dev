#ifndef UTILASSERT_H
#define UTILASSERT_H

#include <assert.h>
#include <sstream>
#include <string>
#include <iostream>

#include "logManager.h"

#define ENGINE_ASSERT_MSG(expr,msg)\
{\
    if(!(expr))\
    {\
        std::stringstream _ss_;\
        _ss_ << msg;\
        LOG(_ss_.str());\
        std::abort();\
    }\
}

#define ENGINE_ASSERT(expr) assert(expr)

#endif