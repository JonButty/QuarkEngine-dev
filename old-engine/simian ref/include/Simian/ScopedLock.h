/************************************************************************/
/*!
\file		ScopedLock.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_SCOPEDLOCK_H_
#define SIMIAN_SCOPEDLOCK_H_

#include "SimianPlatform.h"

namespace Simian
{
    class Lock;

    class SIMIAN_EXPORT ScopedLock
    {
    private:
        Lock* lock_;
    public:
        ScopedLock(Lock& lock);
        ~ScopedLock();
    };
}

#endif
