/*****************************************************************************/
/*!
\file		Angle.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/Angle.h"
#include "Simian/Math.h"

namespace Simian
{
    Degree::Degree(f32 degrees)
        : degrees_(degrees)
    {
    }

    Degree::Degree( const Radian& radian )
        : degrees_(radian.Degrees())
    {
    }

    //--------------------------------------------------------------------------

    f32 Degree::Degrees() const
    {
        return degrees_;
    }

    f32 Degree::Radians() const
    {
        return degrees_/180.0f * Math::PI;
    }

    Degree& Degree::operator=(Radian& radian)
    {
        degrees_ = radian.Degrees();
        return *this;
    }

    //--------------------------------------------------------------------------

    Radian::Radian(f32 radians)
        : radians_(radians)
    {
    }

    Radian::Radian( const Degree& degree )
        : radians_(degree.Radians())
    {
    }

    //--------------------------------------------------------------------------

    f32 Radian::Degrees() const
    {
        return radians_/Math::PI * 180.0f;
    }

    f32 Radian::Radians() const
    {
        return radians_;
    }
    
    Radian& Radian::operator=(Degree& degrees)
    {
        radians_ = degrees.Radians();
        return *this;
    }
}
