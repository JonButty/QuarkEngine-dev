/************************************************************************/
/*!
\file		ThreadW.cpp
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

#include "Simian/ThreadW.h"

namespace Simian
{
    Thread::Thread()
        : thread_(0),
          running_(false)
    {
    }

    Thread::~Thread()
    {
        if (running_.Get())
            Terminate();
        else if (thread_)
            CloseHandle(thread_);
        thread_ = 0;
    }

    //--------------------------------------------------------------------------

    DWORD WINAPI Thread::start_( LPVOID param )
    {
        Thread* thread = reinterpret_cast<Thread*>(param);
        thread->entryPoint_();
        thread->running_.Set(false);
        return 0;
    }

    //--------------------------------------------------------------------------

    void Thread::Run()
    {
        // close any old handles
        if (thread_)
            CloseHandle(thread_);
        thread_ = 0;

        running_.Set(true);
        thread_ = CreateThread(NULL, 0, start_, this, 0, NULL);
        // if creation failed, then its not running
        if (!thread_)
            running_.Set(false);
    }

    void Thread::Terminate()
    {
        if (!thread_)
            return;

        TerminateThread(thread_, 0);
        CloseHandle(thread_);
        thread_ = 0;
    }
}

#endif
