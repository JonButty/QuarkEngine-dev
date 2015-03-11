/*****************************************************************************/
/*!
\file		Vector2SSE.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/SimianPlatform.h"
#if SIMIAN_SSE_SUPPORT == 1

#include "Simian/Vector2SSE.h"
#include "Simian/Debug.h"

#include <cmath>

namespace Simian
{
    Vector2::Vector2(f32 x, f32 y)
    : sse_(_mm_set_ps(1, 0, y, x))
    {
    }

    Vector2::Vector2(const Vector2& vec)
    : sse_(vec.sse_)
    {
    }

    //--------------------------------------------------------------------------

    Vector2 Vector2::operator+(const Vector2& vec) const
    {
        __m128 result = _mm_add_ps(sse_, vec.sse_);
        return Vector2(result.m128_f32[0], result.m128_f32[1]);
    }

    Vector2& Vector2::operator+=(const Vector2& vec)
    {
        sse_ = _mm_add_ps(sse_, _mm_set_ps(0, 0, vec.y_, vec.x_));
        return *this;
    }

    Vector2 Vector2::operator-(const Vector2& vec) const
    {
        __m128 result = _mm_sub_ps(sse_, vec.sse_);
        return Vector2(result.m128_f32[0], result.m128_f32[1]);
    }

    Vector2& Vector2::operator-=(const Vector2& vec)
    {
        sse_ = _mm_sub_ps(sse_, _mm_set_ps(0, 0, vec.y_, vec.x_));
        return *this;
    }

    Vector2 Vector2::operator*(const Vector2& vec) const
    {
        __m128 result = _mm_mul_ps(sse_, vec.sse_);
        return Vector2(result.m128_f32[0], result.m128_f32[1]);
    }

    Vector2& Vector2::operator*=(const Vector2& vec)
    {
        sse_ = _mm_mul_ps(sse_, vec.sse_);
        return *this;
    }

    Vector2 Vector2::operator*(f32 val) const
    {
        __m128 result = _mm_mul_ps(sse_, _mm_set_ps(1, 0, val, val));
        return Vector2(result.m128_f32[0], result.m128_f32[1]);
    }

    Vector2& Vector2::operator*=(f32 val)
    {
        sse_ = _mm_mul_ps(sse_, _mm_set_ps(1, 0, val, val));
        return *this;
    }

    Vector2 Vector2::operator/(const Vector2& vec) const
    {
        __m128 result = _mm_div_ps(sse_, vec.sse_);
        return Vector2(result.m128_f32[0], result.m128_f32[1]);
    }

    Vector2& Vector2::operator/=(const Vector2& vec)
    {
        sse_ = _mm_div_ps(sse_, vec.sse_);
        return *this;
    }

    Vector2 Vector2::operator/(f32 val) const
    {
        __m128 result = _mm_div_ps(sse_, _mm_set_ps(1, 0, val, val));
        return Vector2(result.m128_f32[0], result.m128_f32[1]);
    }

    Vector2& Vector2::operator/=(f32 val)
    {
        sse_ = _mm_div_ps(sse_, _mm_set_ps(1, 0, val, val));
        return *this;
    }
    
    Vector2& Vector2::operator=(const Vector2& vec)
    {
        sse_ = vec.sse_;
        return *this;
    }

    void* Vector2::operator new(size_t x)
    {
        return _aligned_malloc(x, 16);
    }

    void* Vector2::operator new[](size_t x)
    {
        return _aligned_malloc(x, 16);
    }

    void Vector2::operator delete(void* x)
    {
        if (x)
            _aligned_free(x);
    }

    void Vector2::operator delete[](void* x)
    {
        if (x)
            _aligned_free(x);
    }

    //--------------------------------------------------------------------------

    f32 Vector2::LengthSquared() const
    {
        return _mm_cvtss_f32(_mm_dp_ps(sse_, sse_, 0x31));
    }

    f32 Vector2::Length() const
    {
        return _mm_cvtss_f32(_mm_sqrt_ps(_mm_dp_ps(sse_, sse_, 0x31)));
    }

    void Vector2::Normalize()
    {
        sse_ = _mm_mul_ps(sse_, _mm_rsqrt_ps(_mm_dp_ps(sse_, sse_, 0x33)));
    }

    Vector2 Vector2::Normalized() const
    {
        __m128 result = _mm_mul_ps(sse_, _mm_rsqrt_ps(_mm_dp_ps(sse_, sse_, 0x33)));
        return Vector2(result.m128_f32[0], result.m128_f32[1]);
    }

    f32 Vector2::Dot(const Vector2& vec) const
    {
        return _mm_cvtss_f32(_mm_dp_ps(sse_, vec.sse_, 0x31));
    }
}

Simian::Vector2 operator*(f32 val, const Simian::Vector2& vec)
{
    __m128 result = _mm_mul_ps(_mm_set_ps(1, 0, val, val),*(__m128*)&vec);
    return Simian::Vector2(result.m128_f32[0], result.m128_f32[1]);
}

Simian::Vector2 operator/(f32 val, const Simian::Vector2& vec)
{
    __m128 result = _mm_div_ps(_mm_set_ps(1, 0, val, val),*(__m128*)&vec);
    return Simian::Vector2(result.m128_f32[0], result.m128_f32[1]);
}

#endif
