/*****************************************************************************/
/*!
\file		Color.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/Color.h"

namespace Simian
{
    Color::Color(f32 r, f32 g, f32 b, f32 a)
    : r_(r),
      g_(g),
      b_(b),
      a_(a)
    {
    }

    //--------------------------------------------------------------------------

    void Color::R(f32 r)
    {
        r_ = r;
    }

    f32 Color::R() const
    {
        return r_;
    }

    void Color::G(f32 g)
    {
        g_ = g;
    }

    f32 Color::G() const
    {
        return g_;
    }

    void Color::B(f32 b)
    {
        b_ = b;
    }

    f32 Color::B() const
    {
        return b_;
    }

    void Color::A(f32 a)
    {
        a_ = a;
    }

    f32 Color::A() const
    {
        return a_;
    }

    u32 Color::ColorU32() const
    {
        u8 r, g, b, a;
        r = static_cast<u8>(r_ * a_ * 255);
        g = static_cast<u8>(g_ * a_ * 255);
        b = static_cast<u8>(b_ * a_ * 255);
        a = static_cast<u8>(a_ * 255);

        return (a << 24) | (r << 16) | (g << 8) | b;
    }

    //--------------------------------------------------------------------------

    Color Color::operator-() const
    {
        return Color(-r_, -g_, -b_, -a_);
    }

    Color Color::operator-( const Color& color ) const
    {
        return Color(
            r_ - color.r_,
            g_ - color.g_,
            b_ - color.b_,
            a_ - color.a_);
    }

    Color Color::operator+( const Color& color ) const
    {
        return Color(
            r_ + color.r_,
            g_ + color.g_,
            b_ + color.b_,
            a_ + color.a_);
    }

    Color& Color::operator*=( f32 val )
    {
        r_ *= val;
        g_ *= val;
        b_ *= val;
        a_ *= val;
        return *this;
    }
}

Simian::Color operator*( Simian::f32 val, const Simian::Color& col )
{
    return Simian::Color(col) *= val;
}
