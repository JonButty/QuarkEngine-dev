/*****************************************************************************/
/*!
\file		MatrixNative.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/SimianPlatform.h"
#if SIMIAN_SSE_SUPPORT == 0

#include "Simian/MatrixNative.h"

namespace Simian
{
    Matrix::Matrix(f32 m11, f32 m12, f32 m13, f32 m14,
                   f32 m21, f32 m22, f32 m23, f32 m24,
                   f32 m31, f32 m32, f32 m33, f32 m34,
                   f32 m41, f32 m42, f32 m43, f32 m44)
    : m11_(m11), m12_(m12), m13_(m13), m14_(m14),
      m21_(m21), m22_(m22), m23_(m23), m24_(m24),
      m31_(m31), m32_(m32), m33_(m33), m34_(m34),
      m41_(m41), m42_(m42), m43_(m43), m44_(m44)
    {
    }

    Matrix::Matrix(f32* m)
    : m11_(m[0]), m12_(m[1]), m13_(m[2]), m14_(m[3]),
      m21_(m[4]), m22_(m[5]), m23_(m[6]), m24_(m[7]),
      m31_(m[8]), m32_(m[9]), m33_(m[10]), m34_(m[11]),
      m41_(m[12]), m42_(m[13]), m43_(m[14]), m44_(m[15])
    {
    }

    Matrix::Matrix(f32 (*m)[4])
    : m11_(m[0][0]), m12_(m[0][1]), m13_(m[0][2]), m14_(m[0][3]),
      m21_(m[1][0]), m22_(m[1][1]), m23_(m[1][2]), m24_(m[1][3]),
      m31_(m[2][0]), m32_(m[2][1]), m33_(m[2][2]), m34_(m[2][3]),
      m41_(m[3][0]), m42_(m[3][1]), m43_(m[3][2]), m44_(m[3][3])
    {
    }

    //--------------------------------------------------------------------------

    Matrix Matrix::operator+(const Matrix& mat) const
    {
        return Matrix(
            m11_ + mat.m11_, m12_ + mat.m12_, m13_ + mat.m13_, m14_ + mat.m14_,
            m21_ + mat.m21_, m22_ + mat.m22_, m23_ + mat.m23_, m24_ + mat.m24_,
            m31_ + mat.m31_, m32_ + mat.m32_, m33_ + mat.m33_, m34_ + mat.m34_,
            m41_ + mat.m41_, m42_ + mat.m42_, m43_ + mat.m43_, m44_ + mat.m44_);
    }

    Matrix& Matrix::operator+=(const Matrix& mat)
    {
        m11_ += mat.m11_;
        m12_ += mat.m12_;
        m13_ += mat.m13_;
        m14_ += mat.m14_;

        m21_ += mat.m21_;
        m22_ += mat.m22_;
        m23_ += mat.m23_;
        m24_ += mat.m24_;

        m31_ += mat.m31_;
        m32_ += mat.m32_;
        m33_ += mat.m33_;
        m34_ += mat.m34_;

        m41_ += mat.m41_;
        m42_ += mat.m42_;
        m43_ += mat.m43_;
        m44_ += mat.m44_;

        return *this;
    }

    Matrix Matrix::operator-(const Matrix& mat) const
    {
        return Matrix(
            m11_ - mat.m11_, m12_ - mat.m12_, m13_ - mat.m13_, m14_ - mat.m14_,
            m21_ - mat.m21_, m22_ - mat.m22_, m23_ - mat.m23_, m24_ - mat.m24_,
            m31_ - mat.m31_, m32_ - mat.m32_, m33_ - mat.m33_, m34_ - mat.m34_,
            m41_ - mat.m41_, m42_ - mat.m42_, m43_ - mat.m43_, m44_ - mat.m44_);
    }

    Matrix& Matrix::operator-=(const Matrix& mat)
    {
        m11_ -= mat.m11_;
        m12_ -= mat.m12_;
        m13_ -= mat.m13_;
        m14_ -= mat.m14_;

        m21_ -= mat.m21_;
        m22_ -= mat.m22_;
        m23_ -= mat.m23_;
        m24_ -= mat.m24_;

        m31_ -= mat.m31_;
        m32_ -= mat.m32_;
        m33_ -= mat.m33_;
        m34_ -= mat.m34_;

        m41_ -= mat.m41_;
        m42_ -= mat.m42_;
        m43_ -= mat.m43_;
        m44_ -= mat.m44_;

        return *this;
    }

    Matrix Matrix::operator*( f32 scalar ) const
    {
        return Simian::Matrix(
            m11_ * scalar, m12_ * scalar, m13_ * scalar, m14_ * scalar,
            m21_ * scalar, m22_ * scalar, m23_ * scalar, m24_ * scalar,
            m31_ * scalar, m32_ * scalar, m33_ * scalar, m34_ * scalar,
            m41_ * scalar, m42_ * scalar, m43_ * scalar, m44_ * scalar);
    }

    Matrix& Matrix::operator*=( f32 scalar )
    {
        m11_ *= scalar;
        m12_ *= scalar;
        m13_ *= scalar;
        m14_ *= scalar;

        m21_ *= scalar;
        m22_ *= scalar;
        m23_ *= scalar;
        m24_ *= scalar;

        m31_ *= scalar;
        m32_ *= scalar;
        m33_ *= scalar;
        m34_ *= scalar;

        m41_ *= scalar;
        m42_ *= scalar;
        m43_ *= scalar;
        m44_ *= scalar;

        return *this;
    }

    Matrix Matrix::operator*(const Matrix& mat) const
    {
        return Matrix(
            m11_ * mat.m11_ + m12_ * mat.m21_ + m13_ * mat.m31_ + m14_ * mat.m41_,
            m11_ * mat.m12_ + m12_ * mat.m22_ + m13_ * mat.m32_ + m14_ * mat.m42_,
            m11_ * mat.m13_ + m12_ * mat.m23_ + m13_ * mat.m33_ + m14_ * mat.m43_,
            m11_ * mat.m14_ + m12_ * mat.m24_ + m13_ * mat.m34_ + m14_ * mat.m44_,

            m21_ * mat.m11_ + m22_ * mat.m21_ + m23_ * mat.m31_ + m24_ * mat.m41_,
            m21_ * mat.m12_ + m22_ * mat.m22_ + m23_ * mat.m32_ + m24_ * mat.m42_,
            m21_ * mat.m13_ + m22_ * mat.m23_ + m23_ * mat.m33_ + m24_ * mat.m43_,
            m21_ * mat.m14_ + m22_ * mat.m24_ + m23_ * mat.m34_ + m24_ * mat.m44_,

            m31_ * mat.m11_ + m32_ * mat.m21_ + m33_ * mat.m31_ + m34_ * mat.m41_,
            m31_ * mat.m12_ + m32_ * mat.m22_ + m33_ * mat.m32_ + m34_ * mat.m42_,
            m31_ * mat.m13_ + m32_ * mat.m23_ + m33_ * mat.m33_ + m34_ * mat.m43_,
            m31_ * mat.m14_ + m32_ * mat.m24_ + m33_ * mat.m34_ + m34_ * mat.m44_,
            
            m41_ * mat.m11_ + m42_ * mat.m21_ + m43_ * mat.m31_ + m44_ * mat.m41_,
            m41_ * mat.m12_ + m42_ * mat.m22_ + m43_ * mat.m32_ + m44_ * mat.m42_,
            m41_ * mat.m13_ + m42_ * mat.m23_ + m43_ * mat.m33_ + m44_ * mat.m43_,
            m41_ * mat.m14_ + m42_ * mat.m24_ + m43_ * mat.m34_ + m44_ * mat.m44_);
    }

    Matrix& Matrix::operator*=(const Matrix& mat)
    {
        m11_ = m11_ * mat.m11_ + m12_ * mat.m21_ + m13_ * mat.m31_ + m14_ * mat.m41_;
        m12_ = m11_ * mat.m12_ + m12_ * mat.m22_ + m13_ * mat.m32_ + m14_ * mat.m42_;
        m13_ = m11_ * mat.m13_ + m12_ * mat.m23_ + m13_ * mat.m33_ + m14_ * mat.m43_;
        m14_ = m11_ * mat.m14_ + m12_ * mat.m24_ + m13_ * mat.m34_ + m14_ * mat.m44_;

        m21_ = m21_ * mat.m11_ + m22_ * mat.m21_ + m23_ * mat.m31_ + m24_ * mat.m41_;
        m22_ = m21_ * mat.m12_ + m22_ * mat.m22_ + m23_ * mat.m32_ + m24_ * mat.m42_;
        m23_ = m21_ * mat.m13_ + m22_ * mat.m23_ + m23_ * mat.m33_ + m24_ * mat.m43_;
        m24_ = m21_ * mat.m14_ + m22_ * mat.m24_ + m23_ * mat.m34_ + m24_ * mat.m44_;

        m31_ = m31_ * mat.m11_ + m32_ * mat.m21_ + m33_ * mat.m31_ + m34_ * mat.m41_;
        m32_ = m31_ * mat.m12_ + m32_ * mat.m22_ + m33_ * mat.m32_ + m34_ * mat.m42_;
        m33_ = m31_ * mat.m13_ + m32_ * mat.m23_ + m33_ * mat.m33_ + m34_ * mat.m43_;
        m34_ = m31_ * mat.m14_ + m32_ * mat.m24_ + m33_ * mat.m34_ + m34_ * mat.m44_;

        m41_ = m41_ * mat.m11_ + m42_ * mat.m21_ + m43_ * mat.m31_ + m44_ * mat.m41_;
        m42_ = m41_ * mat.m12_ + m42_ * mat.m22_ + m43_ * mat.m32_ + m44_ * mat.m42_;
        m43_ = m41_ * mat.m13_ + m42_ * mat.m23_ + m43_ * mat.m33_ + m44_ * mat.m43_;
        m44_ = m41_ * mat.m14_ + m42_ * mat.m24_ + m43_ * mat.m34_ + m44_ * mat.m44_;

        return *this;
    }

    Vector2 Matrix::operator*(const Vector2& vec) const
    {
        return Vector2(
            m11_ * vec.X() + m12_ * vec.Y() + m14_,
            m21_ * vec.X() + m22_ * vec.Y() + m24_);
    }

    Vector3 Matrix::operator*(const Vector3& vec) const
    {
        return Vector3(
            m11_ * vec.X() + m12_ * vec.Y() + m13_ * vec.Z() + m14_,
            m21_ * vec.X() + m22_ * vec.Y() + m23_ * vec.Z() + m24_,
            m31_ * vec.X() + m32_ * vec.Y() + m33_ * vec.Z() + m34_);
    }

    Vector4 Matrix::operator*(const Vector4& vec) const
    {
        return Vector4(
            m11_ * vec.X() + m12_ * vec.Y() + m13_ * vec.Z() + m14_ * vec.W(),
            m21_ * vec.X() + m22_ * vec.Y() + m23_ * vec.Z() + m24_ * vec.W(),
            m31_ * vec.X() + m32_ * vec.Y() + m33_ * vec.Z() + m34_ * vec.W(),
            m41_ * vec.X() + m42_ * vec.Y() + m43_ * vec.Z() + m44_ * vec.W());
    }

    //--------------------------------------------------------------------------

    void Matrix::Transpose()
    {
        f32 t1, t2, t3, t4, t5, t6;
        t1 = m12_;
        t2 = m13_;
        t3 = m14_;
        t4 = m23_;
        t5 = m24_;
        t6 = m34_;

        m12_ = m21_;
        m13_ = m31_;
        m14_ = m41_;
        m23_ = m32_;
        m24_ = m42_;
        m34_ = m43_;

        m21_ = t1;
        m31_ = t2;
        m41_ = t3;
        m32_ = t4;
        m42_ = t5;
        m43_ = t6;
    }

    Vector2& Matrix::Transform(Vector2& vec) const
    {
        vec = Vector2(
            m11_ * vec.X() + m12_ * vec.Y() + m14_,
            m21_ * vec.X() + m22_ * vec.Y() + m24_);
        return vec;
    }

    Vector3& Matrix::Transform(Vector3& vec) const
    {
        vec = Vector3(
            m11_ * vec.X() + m12_ * vec.Y() + m13_ * vec.Z() + m14_,
            m21_ * vec.X() + m22_ * vec.Y() + m23_ * vec.Z() + m24_,
            m31_ * vec.X() + m32_ * vec.Y() + m33_ * vec.Z() + m34_);
        return vec;
    }

    Vector4& Matrix::Transform(Vector4& vec) const
    {
        vec = Vector4(
            m11_ * vec.X() + m12_ * vec.Y() + m13_ * vec.Z() + m14_ * vec.W(),
            m21_ * vec.X() + m22_ * vec.Y() + m23_ * vec.Z() + m24_ * vec.W(),
            m31_ * vec.X() + m32_ * vec.Y() + m33_ * vec.Z() + m34_ * vec.W(),
            m41_ * vec.X() + m42_ * vec.Y() + m43_ * vec.Z() + m44_ * vec.W());
        return vec;
    }

    void Matrix::Inverse()
    {
        f32 m00 = mrows_[0][0], m01 = mrows_[0][1], m02 = mrows_[0][2], m03 = mrows_[0][3];
        f32 m10 = mrows_[1][0], m11 = mrows_[1][1], m12 = mrows_[1][2], m13 = mrows_[1][3];
        f32 m20 = mrows_[2][0], m21 = mrows_[2][1], m22 = mrows_[2][2], m23 = mrows_[2][3];
        f32 m30 = mrows_[3][0], m31 = mrows_[3][1], m32 = mrows_[3][2], m33 = mrows_[3][3];

        f32 v0 = m20 * m31 - m21 * m30;
        f32 v1 = m20 * m32 - m22 * m30;
        f32 v2 = m20 * m33 - m23 * m30;
        f32 v3 = m21 * m32 - m22 * m31;
        f32 v4 = m21 * m33 - m23 * m31;
        f32 v5 = m22 * m33 - m23 * m32;

        f32 t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
        f32 t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
        f32 t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
        f32 t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

        f32 invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

        f32 d00 = t00 * invDet;
        f32 d10 = t10 * invDet;
        f32 d20 = t20 * invDet;
        f32 d30 = t30 * invDet;

        f32 d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
        f32 d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
        f32 d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
        f32 d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

        v0 = m10 * m31 - m11 * m30;
        v1 = m10 * m32 - m12 * m30;
        v2 = m10 * m33 - m13 * m30;
        v3 = m11 * m32 - m12 * m31;
        v4 = m11 * m33 - m13 * m31;
        v5 = m12 * m33 - m13 * m32;

        f32 d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
        f32 d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
        f32 d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
        f32 d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

        v0 = m21 * m10 - m20 * m11;
        v1 = m22 * m10 - m20 * m12;
        v2 = m23 * m10 - m20 * m13;
        v3 = m22 * m11 - m21 * m12;
        v4 = m23 * m11 - m21 * m13;
        v5 = m23 * m12 - m22 * m13;

        f32 d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
        f32 d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
        f32 d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
        f32 d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

        m11_ = d00;
        m12_ = d01;
        m13_ = d02;
        m14_ = d03;
        
        m21_ = d10;
        m22_ = d11;
        m23_ = d12;
        m24_ = d13;
        
        m31_ = d20;
        m32_ = d21;
        m33_ = d22;
        m34_ = d23;
        
        m41_ = d30;
        m42_ = d31;
        m43_ = d32;
        m44_ = d33;
    }

    Matrix Matrix::Inversed() const
    {
        Matrix m(*this);
        m.Inverse();
        return m;
    }
}

Simian::Matrix operator*( Simian::f32 scalar, const Simian::Matrix& mat )
{
    return mat * scalar;
}

#endif
