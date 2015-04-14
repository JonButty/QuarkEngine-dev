/************************************************************************/
/*!
\file		ThreadW.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_THREADW_H_
#define SIMIAN_THREADW_H_

#include "Simian/SimianPlatform.h"
#if SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32

#include "SimianPlatform.h"
#include "Thread.h"
#include "LockedAttribute.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Simian
{
    class SIMIAN_EXPORT Thread: public ThreadBase
    {
    private:
        HANDLE thread_;
        LockedAttribute<bool> running_;
    private:
        static DWORD WINAPI start_(LPVOID param);
    public:
        Thread();
        ~Thread();

        void Run();
        void Terminate();
    };
}

#endif

#endif
