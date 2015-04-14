/************************************************************************/
/*!
\file		Thread.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_THREAD_H_
#define SIMIAN_THREAD_H_

#include "SimianPlatform.h"
#include "Delegate.h"

namespace Simian
{
    class SIMIAN_EXPORT ThreadBase
    {
    protected:
        Delegate entryPoint_;
    public:
        const Simian::Delegate& EntryPoint() const { return entryPoint_; }
        void EntryPoint(const Simian::Delegate& val) { entryPoint_ = val; }
    public:
        ThreadBase() {}
        virtual ~ThreadBase() {}

        S_NIMPL void Run() {}
        S_NIMPL void Terminate() {}
    };
}

#if SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32
#include "ThreadW.h"
#elif SIMIAN_PLATFORM == SIMIAN_PLATFORM_LINUX
#elif SIMIAN_PLATFORM == SIMIAN_PLATFORM_OSX
#endif

#endif
