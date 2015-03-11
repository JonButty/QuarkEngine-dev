/************************************************************************/
/*!
\file		LockW.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/SimianPlatform.h"
#if SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32

#include "Simian/LockW.h"
#include "Simian/Debug.h"

namespace Simian
{
    Lock::Lock(bool initiallyLocked)
        : LockBase(initiallyLocked), 
          semaphore_(0)
    {
        semaphore_ = CreateSemaphore(NULL, initiallyLocked ? 0 : 1, 1, NULL);
        SAssert(semaphore_ != 0, "Failed to create semaphore.");
    }

    Lock::~Lock()
    {
        CloseHandle(semaphore_);
    }

    void Lock::Acquire() volatile
    {
        WaitForSingleObject(semaphore_, INFINITE);
    }

    void Lock::Release() volatile
    {
        ReleaseSemaphore(semaphore_, 1, NULL);
    }
}

#endif
