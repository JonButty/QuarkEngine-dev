/************************************************************************/
/*!
\file		ProfilerWatch.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_PROFILERWATCH_H_
#define SIMIAN_PROFILERWATCH_H_

#include "SimianPlatform.h"

#if SIMIAN_ATTACHPROFILER == 1

#include "Color.h"

#include <string>

namespace Simian
{
    class SIMIAN_EXPORT ProfilerWatch
    {
    private:
        u32 index_;
        std::string label_;
        u64 startTime_;
        Simian::Color color_;
    public:
        ProfilerWatch(u32 index, const std::string& label, const Simian::Color& color);
        ~ProfilerWatch();
    };
}

#endif

#endif
