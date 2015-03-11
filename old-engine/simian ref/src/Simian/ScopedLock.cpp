/************************************************************************/
/*!
\file		ScopedLock.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/ScopedLock.h"
#include "Simian/Lock.h"

namespace Simian
{
    ScopedLock::ScopedLock( Lock& lock )
        : lock_(&lock)
    {
        lock.Acquire();
    }

    ScopedLock::~ScopedLock()
    {
        lock_->Release();
    }
}
