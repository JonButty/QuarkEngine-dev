/************************************************************************/
/*!
\file		Interpolation.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Interpolation.h"
#include "Simian/Math.h"
#include <cmath>

namespace Simian
{
    f32 Interpolation::Linear(f32 intp)
    {
        return intp;
    }

    f32 Interpolation::EaseSquareIn(f32 intp)
    {
        return intp * intp;
    }

    f32 Interpolation::EaseSquareOut(f32 intp)
    {
        intp = 1 - intp;
        intp *= intp;
        intp = 1 - intp;
        return intp;
    }

    f32 Interpolation::EaseSquareInOut(f32 intp)
    {
        if (intp < 0.5f)
        {
            intp /= 0.5f;
            intp = EaseSquareIn(intp);
            intp *= 0.5f;
        }
        else
        {
            intp = (intp - 0.5f)/0.5f;
            intp = EaseSquareOut(intp);
            intp = intp * 0.5f + 0.5f;
        }
        return intp;
    }

    f32 Interpolation::EaseCubicIn(f32 intp)
    {
        return intp * intp * intp;
    }

    f32 Interpolation::EaseCubicOut(f32 intp)
    {
        intp = 1 - intp;
        intp *= intp * intp;
        intp = 1 - intp;
        return intp;
    }

    f32 Interpolation::EaseCubicInOut(f32 intp)
    {
        if (intp < 0.5f)
        {
            intp /= 0.5f;
            intp = EaseCubicIn(intp);
            intp *= 0.5f;
        }
        else
        {
            intp = (intp - 0.5f)/0.5f;
            intp = EaseCubicOut(intp);
            intp = intp * 0.5f + 0.5f;
        }
        return intp;
    }

    f32 Interpolation::Bounce( f32 dt )
    {
        if (dt < 0.5f)
        {
            f32 intp = dt/0.5f;
            return Simian::Interpolation::EaseSquareIn(intp);
        }
        else if (dt < 0.65f)
        {
            f32 intp = (dt - 0.5f)/0.15f;
            return 1.0f - Simian::Interpolation::EaseSquareOut(intp) * 0.2f;
        }
        else if (dt < 0.8f)
        {
            f32 intp = (dt - 0.65f)/0.15f;
            return 0.8f + Simian::Interpolation::EaseSquareIn(intp) * 0.2f;
        }
        else if (dt < 0.9f)
        {
            f32 intp = (dt - 0.8f)/0.1f;
            return 1.0f - Simian::Interpolation::EaseSquareOut(intp) * 0.05f;
        }
        else
        {
            f32 intp = (dt - 0.9f)/0.1f;
            return 0.95f + Simian::Interpolation::EaseSquareIn(intp) * 0.05f;
        }
    }

    f32 Interpolation::Overshoot( f32 intp )
    {
        if (intp < 0.5f)
        {
            intp /= 0.5f;
            return std::sin(-Math::PI * 0.5f + intp * Math::PI * 0.5f) + 1;
        }
        else if (intp < 0.7f)
        {
            intp = (intp - 0.5f)/0.2f;
            f32 inc = std::sin(intp * Math::PI * 0.5f);
            inc *= 0.2f;
            return 1.0f + inc;
        }
        else
        {
            intp = (intp - 0.7f)/0.3f;
            f32 inc = (std::sin(Math::PI * 0.5f + intp * Math::PI) + 1.0f) * 0.5f;
            inc *= 0.2f;
            return 1.0f + inc;
        }
    }
}
