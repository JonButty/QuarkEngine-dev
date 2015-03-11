/************************************************************************/
/*!
\file		LockW.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_LOCKW_H_
#define SIMIAN_LOCKW_H_

#include "SimianPlatform.h"
#if SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32

#include "Lock.h"

#include <Windows.h>

namespace Simian
{
    class SIMIAN_EXPORT Lock: public LockBase
    {
    private:
        volatile HANDLE semaphore_;
    public:
        Lock(bool initiallyLocked = false);
        ~Lock();

        void Acquire() volatile;
        void Release() volatile;
    };
}

#endif

#endif
