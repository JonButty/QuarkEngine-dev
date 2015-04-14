/*****************************************************************************/
/*!
\file		Vector3Native.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_VECTOR3NATIVE_H_
#define SIMIAN_VECTOR3NATIVE_H_

#include "Simian/SimianPlatform.h"
#if SIMIAN_SSE_SUPPORT == 0

namespace Simian
{
    /** Implementation of a 3d vector.
        @remarks
            The preprocessor flag SIMIAN_SSE_SUPPORT controls whether the vector
            is optimized to use SSE4.
    */
    class SIMIAN_EXPORT Vector3
    {
    public:
        /** Unit vector in the X direction. */
        static const Vector3 UnitX;
        /** Unit vector in the Y direction. */
        static const Vector3 UnitY;
        /** Unit vector in the Z direction. */
        static const Vector3 UnitZ;
        /** Zero vector. */
        static const Vector3 Zero;
    private:
        union
        {
            struct
            {
                f32 x_, y_, z_;
            };
            f32 v[3];
        };
    public:
        /** Sets the X coordinate of the vector. */
        void X(f32 x);
        /** Gets the X coordinate of the vector. */
        f32 X() const;

        /** Sets the Y coordinate of the vector. */
        void Y(f32 y);
        /** Gets the Y coordinate of the vector. */
        f32 Y() const;

        /** Sets the Z coordinate of the vector. */
        void Z(f32 z);
        /** Gets the Z coordinate of the vector. */
        f32 Z() const;

        /** Gets a normalized copy of the vector. */
        Vector3 Normalized() const;
    public:
        /** Arithmetic add with a vector. */
        Vector3 operator+(const Vector3& v) const;
        /** Arithmetic subtract with a vector. */
        Vector3 operator-(const Vector3& v) const;
        /** Unary negation. */
        Vector3 operator-() const;
        /** Arithmetic multiply with a vector. */
        Vector3 operator*(const Vector3& v) const;
        /** Arithmetic devide with a vector. */
        Vector3 operator/(const Vector3& v) const;
        /** Arithmetic multiply with a constant. */
        Vector3 operator*(f32 c) const;
        /** Arithmetic divide with a constant. */
        Vector3 operator/(f32 c) const;

        /** Arithmetic add with a vector in place. */
        Vector3& operator+=(const Vector3& v);
        /** Arithmetic subtract with a vector in place. */
        Vector3& operator-=(const Vector3& v);
        /** Arithmetic multiply with a vector in place. */
        Vector3& operator*=(const Vector3& v);
        /** Arithmetic divide with a vector in place. */
        Vector3& operator/=(const Vector3& v);
        /** Arithmetic multiply with a constant in place. */
        Vector3& operator*=(f32 c);
        /** Arithmetic divide with a constant in place. */
        Vector3& operator/=(f32 c);

        /** Gets a component of the vector. */
        f32& operator [](s32 index);
        /** Gets a component of the vector (read-only). */
        const f32& operator [](s32 index) const;
    public:
        /** Called when Vector3 is constructed. */
        Vector3(f32 x = 0.0f, f32 y = 0.0f, f32 z = 0.0f);

        /** Normalizes the vector.
            @remarks
                To obtain a normalized copy of the vector, use 
                Vector3::Normalized.
            @see
                Vector3::Normalized
        */
        void Normalize();
        /** Gets the length of the vector squared. */
        f32 LengthSquared() const;
        /** Gets the length of the vector. */
        f32 Length() const;
        /** Gets the dot product of the vector. */
        f32 Dot(const Vector3& vec) const;
        /** Gets the cross product of the vector. */
        Vector3 Cross(const Vector3& vec) const;
    };
}

/** Arithmetic multiply for a constant and a Vector3. */
SIMIAN_EXPORT Simian::Vector3 operator*(Simian::f32 c, const Simian::Vector3& vec);
/** Arithmetic divide for a constant and a Vector3. */
SIMIAN_EXPORT Simian::Vector3 operator/(Simian::f32 c, const Simian::Vector3& vec);

#endif

#endif
