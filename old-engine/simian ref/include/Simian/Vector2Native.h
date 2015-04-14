/*****************************************************************************/
/*!
\file		Vector2Native.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_VECTOR2NATIVE_H_
#define SIMIAN_VECTOR2NATIVE_H_

#include "Simian/SimianPlatform.h"
#if SIMIAN_SSE_SUPPORT == 0

namespace Simian
{
    /** Implementation of a 2d vector.
        @remarks
            The preprocessor flag SIMIAN_SSE_SUPPORT controls whether the vector
            is optimized to use SSE4.
    */
    class SIMIAN_EXPORT Vector2
    {
    public:
        /** Unit vector in the X direction. */
        static const Vector2 UnitX;
        /** Unit vector in the Y direction. */
        static const Vector2 UnitY;
        /** Zero vector. */
        static const Vector2 Zero;
    private:
        union
        {
            struct
            {
                f32 x_;
                f32 y_;
            };
            f32 v_[2];
        };

    public:
        /** Gets the X coordinate of the vector. */
        f32 X() const;
        /** Sets the X coordinate of the vector. */
        void X(f32 x);

        /** Gets the Y coordinate of the vector. */
        f32 Y() const;
        /** Sets the Y coordinate of the vector. */
        void Y(f32 y);

    public:
        /** Arithmetically adds 2 vectors. */
        Vector2 operator+(const Vector2& vec) const;
        /** Arithmetically adds 2 vectors in place. */
        Vector2& operator+=(const Vector2& vec);

        /** Arithmetically subtracts 2 vectors. */
        Vector2 operator-(const Vector2& vec) const;
        /** Arithmetically subtracts 2 vectors in place. */
        Vector2& operator-=(const Vector2& vec);

        /** Arithmetically multiplies 2 vectors. 
            @remarks
                This is scalar multiplication. e.g. vec1.x * vec2.x and vec1.y *
                vec2.y.
        */
        Vector2 operator*(const Vector2& vec) const;
        /** Arithmetically multiplies 2 vectors in place.
            @remarks
                This is scalar multiplication. e.g. vec1.x * vec2.x and vec1.y *
                vec2.y.
        */
        Vector2& operator*=(const Vector2& vec);
        /** Arithmetically multiplies a vector with a float. */
        Vector2 operator*(f32 val) const;
        /** Arithmetically multiplies a vector with a float in place. */
        Vector2& operator*=(f32 val);
        
        /** Arithmetically divides 2 vectors. 
            @remarks
                This is scalar division. e.g. vec1.x/vec2.x and vec1.y/vec2.y.
        */
        Vector2 operator/(const Vector2& vec) const;
        /** Arithmetically divides 2 vectors in place. 
            @remarks
                This is scalar division. e.g. vec1.x/vec2.x and vec1.y/vec2.y.
        */
        Vector2& operator/=(const Vector2& vec);
        /** Arithmetically divides a vector by a float. */
        Vector2 operator/(f32 val) const;
        /** Arithmetically divides a vector by a float in place. */
        Vector2& operator/=(f32 val);

        /** Unary negation operator. Returns a negative version of a vector. */
        Vector2 operator-() const;

        /** Accesses the members of the vector. 
            @remarks
                Index 0 is X and Index 1 is Y. 
        */
        const f32& operator[](s32 index) const;
        /** Accesses the members of the vector. 
            @remarks
                Index 0 is X and Index 1 is Y. 
        */
        f32& operator[](s32 index);
    public:
        /** Initializes a Vector2 with the specified members. */
        Vector2(f32 x = 0, f32 y = 0);

        /** Calculates the length squared of the vector. 
            @remarks
                This is typically the step before square rooting a vector to
                find the length. It can be used to approximate length without
                incurring the cost of a square root.
        */
        f32 LengthSquared() const;
        /** Calculates the length of the vector. */
        f32 Length() const;

        /** Normalizes the vector in place. */
        void Normalize();
        /** Returns a normalized vector.
            @remarks
                This function does not change the vector itself.
        */
        Vector2 Normalized() const;

        /** Performs dot product with another Vector2. */
        f32 Dot(const Vector2& vec) const;
    };
}

SIMIAN_EXPORT Simian::Vector2 operator*(Simian::f32 val, const Simian::Vector2& vec);
SIMIAN_EXPORT Simian::Vector2 operator/(Simian::f32 val, const Simian::Vector2& vec);

#endif

#endif
