/************************************************************************/
/*!
\file		Interpolation.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_INTERPOLATION_H_
#define SIMIAN_INTERPOLATION_H_

#include "SimianPlatform.h"

namespace Simian
{
    class SIMIAN_EXPORT Interpolation
    {
    public:
        /** Linear interpolation. */
        static f32 Linear(f32 intp);

        /** Easing inwards (slow to fast). */
        static f32 EaseSquareIn(f32 intp);
        /** Easing outwards (fast to slow). */
        static f32 EaseSquareOut(f32 intp);
        /** Easing both ways (slow to fast to slow). */
        static f32 EaseSquareInOut(f32 intp);

        /** Easing inwards (slow to fast). */
        static f32 EaseCubicIn(f32 intp);
        /** Easing outwards (fast to slow). */
        static f32 EaseCubicOut(f32 intp);
        /** Easing both ways (slow to fast to slow). */
        static f32 EaseCubicInOut(f32 intp);

        /** Bounce interpolation, falls and bounce back twice. */
        static f32 Bounce(f32 intp);

        /** Go over, then come back. */
        static f32 Overshoot(f32 intp);

        /** Interpolates any type using an interpolator */
        template <class T>
        static T Interpolate(const T& initial, const T& final, f32 intp, f32(*interpolator)(f32))
        {
            return initial + (*interpolator)(intp) * (final - initial);
        }
    };
}

#endif
