/*****************************************************************************/
/*!
\file		Matrix.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/Matrix.h"
#include "Simian/Angle.h"
#include "Simian/Debug.h"
#include "Simian/Math.h"

#include <cmath>

namespace Simian
{
    const Matrix Matrix::Identity(1, 0, 0, 0, 
                                  0, 1, 0, 0, 
                                  0, 0, 1, 0,
                                  0, 0, 0, 1);
    const Matrix Matrix::Zero(0, 0, 0, 0, 
                              0, 0, 0, 0, 
                              0, 0, 0, 0,
                              0, 0, 0, 0);

    Matrix Matrix::Translation(f32 x, f32 y, f32 z)
    {
        return Matrix(
            1, 0, 0, x,
            0, 1, 0, y,
            0, 0, 1, z,
            0, 0, 0, 1);
    }

    Matrix Matrix::Translation(const Vector2& vec)
    {
        return Matrix(
            1, 0, 0, vec.X(),
            0, 1, 0, vec.Y(),
            0, 0, 1, 0,
            0, 0, 0, 1);
    }

    Matrix Matrix::Translation(const Vector3& vec)
    {
        return Matrix(
            1, 0, 0, vec.X(),
            0, 1, 0, vec.Y(),
            0, 0, 1, vec.Z(),
            0, 0, 0, 1);
    }

    Matrix Matrix::Scale(f32 x, f32 y, f32 z)
    {
        return Matrix(
            x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, 0,
            0, 0, 0, 1);
    }

    Matrix Matrix::Scale(const Vector2& vec)
    {
        return Matrix(
            vec.X(), 0, 0, 0,
            0, vec.Y(), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1);
    }

    Matrix Matrix::Scale(const Vector3& vec)
    {
        return Matrix(
            vec.X(), 0, 0, 0,
            0, vec.Y(), 0, 0,
            0, 0, vec.Z(), 0,
            0, 0, 0, 1);
    }

    Matrix Matrix::Rotation2D(const Angle& angle)
    {
        return Matrix(
            cos(angle.Radians()), -sin(angle.Radians()), 0, 0,
            sin(angle.Radians()), cos(angle.Radians()), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1);
    }

    Matrix Matrix::RotationAxisAngle(const Vector3& axis, const Angle& angle)
    {
        f32 c = std::cos(angle.Radians());
        f32 s = std::sin(angle.Radians());
        f32 t = 1 - c;
        Vector3 n = axis.Normalized();

        return Matrix(
            t*n.X()*n.X()+c, t*n.X()*n.Y()-n.Z()*s, t*n.X()*n.Z()+n.Y()*s, 0,
            t*n.X()*n.Y()+n.Z()*s, t*n.Y()*n.Y()+c, t*n.Y()*n.Z()-n.X()*s, 0,
            t*n.X()*n.Z()-n.Y()*s, t*n.Y()*n.Z()+n.X()*s, t*n.Z()*n.Z()+c, 0,
            0, 0, 0, 1);
    }

    Matrix Matrix::RotationQuaternion( const Vector4& quaternion )
    {
        f32 x2 = quaternion.X() * quaternion.X();
        f32 y2 = quaternion.Y() * quaternion.Y();
        f32 z2 = quaternion.Z() * quaternion.Z();

        f32 xy = quaternion.X() * quaternion.Y();
        f32 xz = quaternion.X() * quaternion.Z();
        f32 yz = quaternion.Y() * quaternion.Z();

        f32 wx = quaternion.W() * quaternion.X();
        f32 wy = quaternion.W() * quaternion.Y();
        f32 wz = quaternion.W() * quaternion.Z();

        return Matrix( 1.0f - 2.0f * (y2 + z2), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
            2.0f * (xy + wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz - wx), 0.0f,
            2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (x2 + y2), 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
    }

    Matrix Matrix::RotationEuler(const Angle& x, const Angle& y, const Angle& z)
    {
        return  Matrix::RotationAxisAngle(Vector3::UnitZ, z) *
                Matrix::RotationAxisAngle(Vector3::UnitY, y) *
                Matrix::RotationAxisAngle(Vector3::UnitX, x);
    }

    Matrix Matrix::PerspectiveFOV(const Angle& fovy, f32 aspect, f32 near, f32 far)
    {
        // from msdn page for d3d's fov lh
        f32 yScale = std::cosf(fovy.Radians() * 0.5f)/std::sinf(fovy.Radians() * 0.5f);
        f32 xScale = yScale/aspect;

        return Matrix(
            xScale, 0, 0, 0,
            0, yScale, 0, 0,
            0, 0, far/(far - near), -near * far/(far - near),
            0, 0, 1, 0);
    }

    Matrix Matrix::Ortho( f32 width, f32 height, f32 near, f32 far )
    {
        return Matrix(
            2.0f/width, 0, 0, 0,
            0, 2.0f/height, 0, 0,
            0, 0, 1.0f/(far-near), near/(near-far),
            0, 0, 0, 1);
    }

    Matrix Matrix::LookAt(const Vector3& eye, const Vector3& lookat, const Vector3& up)
    {
        Vector3 z = (lookat - eye);
        z.Normalize();
        Vector3 x = up.Cross(z);
        x.Normalize();
        Vector3 y = z.Cross(x);
        y.Normalize();

        return Matrix(
            x.X(), x.Y(), x.Z(), -x.Dot(eye),
            y.X(), y.Y(), y.Z(), -y.Dot(eye),
            z.X(), z.Y(), z.Z(), -z.Dot(eye),
            0, 0, 0, 1);
    }

    Matrix Matrix::Transposed() const
    {
        Matrix mat(*this);
        mat.Transpose();
        return mat;
    }

    Simian::f32 Matrix::Determinant() const
    {
        return 
            mrows_[0][0] * mrows_[1][1] * mrows_[2][2] +
            mrows_[0][1] * mrows_[1][2] * mrows_[2][0] +
            mrows_[0][2] * mrows_[1][0] * mrows_[2][1] -
            mrows_[0][2] * mrows_[1][1] * mrows_[2][0] -
            mrows_[0][1] * mrows_[1][0] * mrows_[2][2] -
            mrows_[0][0] * mrows_[1][2] * mrows_[2][0];
    }

    Vector3 Matrix::Position() const
    {
        return Vector3(
            mrows_[0][3],
            mrows_[1][3],
            mrows_[2][3]);
    }

    Vector3 Matrix::Right() const
    {
        return Vector3(
            mrows_[0][0],
            mrows_[1][0],
            mrows_[2][0]);
    }

    Vector3 Matrix::Up() const
    {
        return Vector3(
            mrows_[0][1],
            mrows_[1][1],
            mrows_[2][1]);
    }

    Vector3 Matrix::Forward() const
    {
        return Vector3(
            mrows_[0][2],
            mrows_[1][2],
            mrows_[2][2]);
    }

    void Matrix::Decompose( Simian::Vector3& translation, Simian::Vector4& rotation, Simian::Vector3& scale ) const
    {
        translation = Simian::Vector3(
            mrows_[0][3],
            mrows_[1][3],
            mrows_[2][3]);
        
        // QDU decomposition
        f32 fInvLength = mrows_[0][0]*mrows_[0][0] + mrows_[1][0]*mrows_[1][0] + mrows_[2][0]*mrows_[2][0];
        if (fInvLength < -0.0001 || fInvLength > 0.0001) 
            fInvLength = 1.0f/std::sqrt(fInvLength);

        Simian::Matrix kQ;
        kQ[0][0] = mrows_[0][0]*fInvLength;
        kQ[1][0] = mrows_[1][0]*fInvLength;
        kQ[2][0] = mrows_[2][0]*fInvLength;

        f32 fDot = kQ[0][0]*mrows_[0][1] + kQ[1][0]*mrows_[1][1] +
            kQ[2][0]*mrows_[2][1];
        kQ[0][1] = mrows_[0][1]-fDot*kQ[0][0];
        kQ[1][1] = mrows_[1][1]-fDot*kQ[1][0];
        kQ[2][1] = mrows_[2][1]-fDot*kQ[2][0];
        fInvLength = kQ[0][1]*kQ[0][1] + kQ[1][1]*kQ[1][1] + kQ[2][1]*kQ[2][1];
        if (fInvLength < -0.0001 || fInvLength > 0.0001) 
            fInvLength = 1.0f/std::sqrt(fInvLength);

        kQ[0][1] *= fInvLength;
        kQ[1][1] *= fInvLength;
        kQ[2][1] *= fInvLength;

        fDot = kQ[0][0]*mrows_[0][2] + kQ[1][0]*mrows_[1][2] +
            kQ[2][0]*mrows_[2][2];
        kQ[0][2] = mrows_[0][2]-fDot*kQ[0][0];
        kQ[1][2] = mrows_[1][2]-fDot*kQ[1][0];
        kQ[2][2] = mrows_[2][2]-fDot*kQ[2][0];
        fDot = kQ[0][1]*mrows_[0][2] + kQ[1][1]*mrows_[1][2] +
            kQ[2][1]*mrows_[2][2];
        kQ[0][2] -= fDot*kQ[0][1];
        kQ[1][2] -= fDot*kQ[1][1];
        kQ[2][2] -= fDot*kQ[2][1];
        fInvLength = kQ[0][2]*kQ[0][2] + kQ[1][2]*kQ[1][2] + kQ[2][2]*kQ[2][2];
        if (fInvLength < -0.0001 || fInvLength > 0.0001) 
            fInvLength = 1.0f/std::sqrt(fInvLength);

        kQ[0][2] *= fInvLength;
        kQ[1][2] *= fInvLength;
        kQ[2][2] *= fInvLength;

        // guarantee that orthogonal matrix has determinant 1 (no reflections)
        f32 fDet = kQ[0][0]*kQ[1][1]*kQ[2][2] + kQ[0][1]*kQ[1][2]*kQ[2][0] +
            kQ[0][2]*kQ[1][0]*kQ[2][1] - kQ[0][2]*kQ[1][1]*kQ[2][0] -
            kQ[0][1]*kQ[1][0]*kQ[2][2] - kQ[0][0]*kQ[1][2]*kQ[2][1];

        if ( fDet < 0.0 )
        {
            for (size_t iRow = 0; iRow < 3; iRow++)
                for (size_t iCol = 0; iCol < 3; iCol++)
                    kQ[iRow][iCol] = -kQ[iRow][iCol];
        }

        // build "right" matrix R
        Simian::Matrix kR;
        kR[0][0] = kQ[0][0]*mrows_[0][0] + kQ[1][0]*mrows_[1][0] +
            kQ[2][0]*mrows_[2][0];
        kR[0][1] = kQ[0][0]*mrows_[0][1] + kQ[1][0]*mrows_[1][1] +
            kQ[2][0]*mrows_[2][1];
        kR[1][1] = kQ[0][1]*mrows_[0][1] + kQ[1][1]*mrows_[1][1] +
            kQ[2][1]*mrows_[2][1];
        kR[0][2] = kQ[0][0]*mrows_[0][2] + kQ[1][0]*mrows_[1][2] +
            kQ[2][0]*mrows_[2][2];
        kR[1][2] = kQ[0][1]*mrows_[0][2] + kQ[1][1]*mrows_[1][2] +
            kQ[2][1]*mrows_[2][2];
        kR[2][2] = kQ[0][2]*mrows_[0][2] + kQ[1][2]*mrows_[1][2] +
            kQ[2][2]*mrows_[2][2];

        // the scaling component
        scale[0] = kR[0][0];
        scale[1] = kR[1][1];
        scale[2] = kR[2][2];

        // convert kR into quaternion
        f32 fTrace = kQ[0][0]+kQ[1][1]+kQ[2][2];
        f32 fRoot;

        if ( fTrace > 0.0 )
        {
            fRoot = std::sqrt(fTrace + 1.0f);
            rotation[3] = 0.5f*fRoot;
            fRoot = 0.5f/fRoot;
            rotation[0] = (kQ[2][1]-kQ[1][2])*fRoot;
            rotation[1] = (kQ[0][2]-kQ[2][0])*fRoot;
            rotation[2] = (kQ[1][0]-kQ[0][1])*fRoot;
        }
        else
        {
            static size_t s_iNext[3] = { 1, 2, 0 };
            size_t i = 0;
            if ( kQ[1][1] > kQ[0][0] )
                i = 1;
            if ( kQ[2][2] > kQ[i][i] )
                i = 2;
            size_t j = s_iNext[i];
            size_t k = s_iNext[j];

            fRoot = std::sqrt(kQ[i][i]-kQ[j][j]-kQ[k][k] + 1.0f);
            f32* apkQuat[3] = { &rotation[0], &rotation[1], &rotation[2] };
            *apkQuat[i] = 0.5f*fRoot;
            fRoot = 0.5f/fRoot;
            rotation[3] = (kQ[k][j]-kQ[j][k])*fRoot;
            *apkQuat[j] = (kQ[j][i]+kQ[i][j])*fRoot;
            *apkQuat[k] = (kQ[k][i]+kQ[i][k])*fRoot;
        }
    }

    //--------------------------------------------------------------------------

    f32* const Matrix::operator [](s32 index)
    {
        SAssert(index >= 0 && index < 4, "Matrix subscript out of bounds.");
        return mrows_[index];
    }

    const f32* const Matrix::operator [](s32 index) const
    {
        SAssert(index >= 0 && index < 4, "Matrix subscript out of bounds.");
        return mrows_[index];
    }
}
