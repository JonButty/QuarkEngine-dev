/*****************************************************************************/
/*!
\file		Color.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_COLOR_H_
#define SIMIAN_COLOR_H_

#include "SimianPlatform.h"

namespace Simian
{
    /** Represents a color with the R, G, B and A components. */
    class SIMIAN_EXPORT Color
    {
    private:
        f32 r_, g_, b_, a_;
    public:
        /** Sets the red component of the Color. */
        void R(f32 r);
        /** Gets the red component of the Color. */
        f32 R() const;
        
        /** Sets the green component of the Color. */
        void G(f32 g);
        /** Gets the green component of the Color. */
        f32 G() const;

        /** Sets the blue component of the Color. */
        void B(f32 b);
        /** Gets the blue component of the Color. */
        f32 B() const;

        /** Sets the alpha component of the Color. */
        void A(f32 a);
        /** Gets the alpha component of the Color. */
        f32 A() const;

        /** Gets the color as a 32-bit unsigned integral. */
        u32 ColorU32() const;
    public:
        Color operator-() const;
        Color operator-(const Color& color) const;
        Color operator+(const Color& color) const;
        Color& operator*=(f32 val);
    public:
        /** Creates a new Color. */
        Color(f32 r = 1.0f, f32 g = 1.0f, f32 b = 1.0f, f32 a = 1.0f);
    };
}

SIMIAN_EXPORT Simian::Color operator*(Simian::f32 val, const Simian::Color& col);

#endif
