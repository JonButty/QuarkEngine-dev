/************************************************************************/
/*!
\file		ProfilerWatch.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/SimianPlatform.h"

#if SIMIAN_ATTACHPROFILER == 1

#include "Simian/InstrumentedProfiler.h"
#include "Simian/ProfilerWatch.h"
#include "Simian/OS.h"

namespace Simian
{
    ProfilerWatch::ProfilerWatch( u32 index, const std::string& label, const Simian::Color& color )
        : index_(index),
          label_(label),
          startTime_(OS::Instance().GetHighPerformanceCounter()),
          color_(color)
    {
    }

    ProfilerWatch::~ProfilerWatch()
    {
        u64 timePassed = OS::Instance().GetHighPerformanceCounter() - startTime_;
        InstrumentedProfiler::Instance().UpdateWatch(index_, label_, timePassed, color_);
    }
}

#endif
