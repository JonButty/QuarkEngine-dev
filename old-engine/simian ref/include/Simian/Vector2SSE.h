/*****************************************************************************/
/*!
\file		Vector2SSE.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_Vector2SSE_H_
#define SIMIAN_Vector2SSE_H_


#include "Simian/SimianPlatform.h"
#if SIMIAN_SSE_SUPPORT == 1

#include <smmintrin.h>

namespace Simian
{
    class Vector2;
}

namespace Simian
{
    _MM_ALIGN16 class Vector2
    {
    public:
        static const Vector2 X;
        static const Vector2 Y;
        static const Vector2 Zero;
    private:
        union
        {
            __m128 sse_;
            struct
            {
                f32 x_;
                f32 y_;
                f32 reserved_[2];
            };
            f32 v_[4];
        };

    public:
        f32 X() const;
        void X(f32 x);

        f32 Y() const;
        void Y(f32 y);

    public:
        Vector2 operator+(const Vector2& vec) const;

        Vector2& operator+=(const Vector2& vec);

        Vector2 operator-(const Vector2& vec) const;
        Vector2& operator-=(const Vector2& vec);

        Vector2 operator*(const Vector2& vec) const;
        Vector2& operator*=(const Vector2& vec);
        Vector2 operator*(f32 val) const;
        Vector2& operator*=(f32 val);

        Vector2 operator/(const Vector2& vec) const;
        Vector2& operator/=(const Vector2& vec);
        Vector2 operator/(f32 val) const;
        Vector2& operator/=(f32 val);

        void* operator new(size_t x);
        void* operator new[](size_t x);
        void operator delete(void* x);
        void operator delete[](void* x);

        Vector2& operator=(const Vector2& vec);

        Vector2 operator-() const;

        const f32& operator[](s32 index) const;
        f32& operator[](s32 index);
    public:
        Vector2(f32 x = 0, f32 y = 0);
        Vector2(const Vector2& vector);

        f32 LengthSquared() const;
        f32 Length() const;

        void Normalize();
        Vector2 Normalized() const;

        f32 Dot(const Vector2& vec) const;

        friend class Matrix;
    };
}

SIMIAN_EXPORT Simian::Vector2 operator*(f32 val, const Simian::Vector2& vec);
SIMIAN_EXPORT Simian::Vector2 operator/(f32 val, const Simian::Vector2& vec);

#endif

#endif
