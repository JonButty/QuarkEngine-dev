/************************************************************************/
/*!
\file		Lock.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_LOCK_H_
#define SIMIAN_LOCK_H_

#include "SimianPlatform.h"

namespace Simian
{
    class SIMIAN_EXPORT LockBase
    {
    public:
        LockBase(bool = false) {}
        virtual ~LockBase() {}
        S_NIMPL void Acquire() {}
        S_NIMPL void Release() {}
    };
}

#if SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32
#include "LockW.h"
#elif SIMIAN_PLATFORM == SIMIAN_PLATFORM_LINUX
#elif SIMIAN_PLATFORM == SIMIAN_PLATFORM_OSX
#endif

#endif
