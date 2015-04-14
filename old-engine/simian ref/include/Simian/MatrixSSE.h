/*****************************************************************************/
/*!
\file		MatrixSSE.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_Matrix_SSE_H_
#define SIMIAN_Matrix_SSE_H_

#include "SimianPlatform.h"
#if SIMIAN_SSE_SUPPORT == 1

#include "Vector2.h"
#include "Vector3.h"
#include <smmintrin.h>

namespace Simian
{
    class Angle;

    _MM_ALIGN16 class Matrix
    {
    public:
        static const Matrix Identity;
        static const Matrix Zero;
    private:
        union
        {
            __m128 sse_[4];
            struct 
            {
                f32 m11_, m12_, m13_, m14_;
                f32 m21_, m22_, m23_, m24_;
                f32 m31_, m32_, m33_, m34_;
                f32 m41_, m42_, m43_, m44_;
            };
            f32 m_[16];
            f32 mrows_[4][4];
            struct
            {
                __m128 sse1_;
                __m128 sse2_;
                __m128 sse3_;
                __m128 sse4_;
            };
        };
    public:
        static Matrix Translation(f32 x, f32 y, f32 z);
        static Matrix Translation(const Vector2& vec);
        static Matrix Translation(const Vector3& vec);
        static Matrix Scale(f32 sx, f32 sy, f32 sz);
        static Matrix Scale(const Vector2& vec);
        static Matrix Scale(const Vector3& vec);
        static Matrix Rotation2D(const Angle& angle);
        static Matrix RotationAxisAngle(const Vector3& axis, const Angle& angle);
        static Matrix RotationEuler(const Angle& x, const Angle& y, const Angle& z);
        static Matrix PerspectiveFOV(const Angle& fovy, f32 aspect, f32 near, f32 far);
        static Matrix LookAt(const Simian::Vector2& eye, const Simian::Vector2& lookat, const Simian::Vector2& up);

        Matrix Transposed() const;
    public:
        Matrix operator+(const Matrix& mat) const;
        Matrix& operator+=(const Matrix& mat);

        Matrix operator-(const Matrix& mat) const;
        Matrix& operator-=(const Matrix& mat);

        Matrix operator*(const Matrix& mat) const;
        Matrix& operator*=(const Matrix& mat);
        
        Vector2 operator*(const Vector2& vec) const;

        Matrix& operator=(const Matrix& mat);

        void* operator new(size_t x);
        void* operator new[](size_t x);
        void operator delete(void* x);
        void operator delete[](void* x);

        f32* const operator[](s32 index);
        const f32* const operator[](s32 index) const;
    public:
        Matrix(f32 m11 = 1, f32 m12 = 0, f32 m13 = 0, f32 m14 = 0, 
                f32 m21 = 0, f32 m22 = 1, f32 m23 = 0, f32 m24 = 0,
                f32 m31 = 0, f32 m32 = 0, f32 m33 = 1, f32 m34 = 0,
                f32 m41 = 0, f32 m42 = 0, f32 m43 = 0, f32 m44 = 1);
        Matrix(f32* m);
        Matrix(f32 (*m)[4]);
        Matrix(const Matrix& mat);

        void Transpose();
        Vector2& Transform(Vector2& vec) const;
    };
}

#endif

#endif
