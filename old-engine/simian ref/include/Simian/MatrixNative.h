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

#ifndef SIMIAN_MATRIX3NATIVE_H_
#define SIMIAN_MATRIX3NATIVE_H_

#include "SimianPlatform.h"
#if SIMIAN_SSE_SUPPORT == 0

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Simian
{
    class Angle;

    /** Implementation of a 4x4 matrix. 
        @remarks
            The preprocessor flag SIMIAN_SSE_SUPPORT controls whether the matrix
            is optimized to use SSE4.
    */
    class SIMIAN_EXPORT Matrix
    {
    public:
        /** The definition of an identity matrix. */
        static const Matrix Identity;
        /** The definition of an additive indentity matrix. */
        static const Matrix Zero;
    private:
        union
        {
            struct 
            {
                f32 m11_, m12_, m13_, m14_;
                f32 m21_, m22_, m23_, m24_;
                f32 m31_, m32_, m33_, m34_;
                f32 m41_, m42_, m43_, m44_;
            };
            f32 m_[16];
            f32 mrows_[4][4];
        };
    public:
        /** Creates a translation matrix by x, y and z. */
        static Matrix Translation(f32 x, f32 y, f32 z);
        /** Creates a translation matrix in 2D with a Vector2. */
        static Matrix Translation(const Vector2& vec);
        /** Creates a translation matrix in 3D with a Vector3. */
        static Matrix Translation(const Vector3& vec);
        /** Creates a scale matrix by sx, sy and sz. */
        static Matrix Scale(f32 sx, f32 sy, f32 sz);
        /** Creates a scale matrix in 2D with a Vector2 */
        static Matrix Scale(const Vector2& vec);
        /** Creates a scale matrix in 3D with a Vector3 */
        static Matrix Scale(const Vector3& vec);
        /** Creates a 2D rotation matrix with an Angle. 
            @remarks
                This is matrix is represents a rotation on the Z axis.
        */
        static Matrix Rotation2D(const Angle& angle);
        /** Creates a 3D rotation matrix with an axis and angle. */
        static Matrix RotationAxisAngle(const Vector3& axis, const Angle& angle);
        /** Creates a 3D euler matrix. */
        static Matrix RotationEuler(const Angle& x, const Angle& y, const Angle& z);
        /** Creates a 3D rotation matrix from a quaternion. */
        static Matrix RotationQuaternion(const Vector4& quaternion);
        /** Creates a perspective matrix. */
        static Matrix PerspectiveFOV(const Angle& fovy, f32 aspect, f32 near, f32 far);
        /** Creates an orthographic matrix. */
        static Matrix Ortho(f32 width, f32 height, f32 near, f32 far);
        /** Creates a look at view matrix. */
        static Matrix LookAt(const Simian::Vector3& eye, const Simian::Vector3& lookat, const Simian::Vector3& up);

        /** Creates a transposed copy of the Matrix. */
        Matrix Transposed() const;

        /** Return the determinant of the matrix. */
        f32 Determinant() const;

        /** Position vector in the matrix (Homogeneous). */
        Vector3 Position() const;

        /** Right vector of the matrix (1st column). */
        Vector3 Right() const;
        /** Up vector of the matrix (2nd column). */
        Vector3 Up() const;
        /** Forward vector of the matrix (3rd column). */
        Vector3 Forward() const;
    public:
        /** Arithmetic adding of matrices. */
        Matrix operator+(const Matrix& mat) const;
        /** Arithmetic adding of matrices in place. */
        Matrix& operator+=(const Matrix& mat);

        /** Arithmetic subtracting of matrices. */
        Matrix operator-(const Matrix& mat) const;
        /** Arithmetic subtracting of matrices in place. */
        Matrix& operator-=(const Matrix& mat);

        /** Perform scalar matrix multiplication. */
        Matrix operator*(f32 scalar) const;
        Matrix& operator*=(f32 scalar);

        /** Performs matrix multiplication. */
        Matrix operator*(const Matrix& mat) const;
        /** Performs matrix multiplication in place. */
        Matrix& operator*=(const Matrix& mat);

        /** Transforms a Vector2 by the matrix. */
        Vector2 operator*(const Vector2& vec) const;
        /** Transforms a Vector2 by the matrix. */
        Vector3 operator*(const Vector3& vec) const;
        /** Transforms a Vector2 by the matrix. */
        Vector4 operator*(const Vector4& vec) const;

        /** Accesses the elements of the matrix. 
            @remarks
                Matrix is accessed as a 2D array of floats. e.g. To access the
                first element of the matrix, use matrix[0][0].
        */
        f32* const operator[](s32 index);

        /** Accesses the elements of the matrix (constant access).
            @remarks
                Matrix is accessed as a 2D array of floats. e.g. To access the
                first element of the matrix, use matrix[0][0].
        */
        const f32* const operator[](s32 index) const;
    public:
        /** Initialize a Matrix using floats. */
        Matrix(f32 m11 = 1, f32 m12 = 0, f32 m13 = 0, f32 m14 = 0, 
            f32 m21 = 0, f32 m22 = 1, f32 m23 = 0, f32 m24 = 0,
            f32 m31 = 0, f32 m32 = 0, f32 m33 = 1, f32 m34 = 0,
            f32 m41 = 0, f32 m42 = 0, f32 m43 = 0, f32 m44 = 1);
        /** Initialize the Matrix with an array of 16 floats. */
        Matrix(f32* m);
        /** Initialize the Matrix with a 4 by 4 2D array of floats. */
        Matrix(f32 (*m)[4]);

        /** Transposes the matrix.
            @remarks
                This function transposes the matrix in place. To obtain a
                transposed copy of the matrix, use Matrix::Transposed.
            @see
                Matrix::Transposed
        */
        void Transpose();
        /** Transforms a Vector2 by the matrix.
            @remarks
                This is faster then the multiplication operator as it uses
                references. However, it can only transform defined Vector2s.
        */
        Vector2& Transform(Vector2& vec) const;
        /** Transforms a Vector3 by the matrix.
            @remarks
                This is faster then the multiplication operator as it uses
                references. However, it can only transform defined Vector2s.
        */
        Vector3& Transform(Vector3& vec) const;
        /** Transforms a Vector4 by the matrix.
            @remarks
                This is faster then the multiplication operator as it uses
                references. However, it can only transform defined Vector2s.
        */
        Vector4& Transform(Vector4& vec) const;
        /** Inverses the matrix. */
        void Inverse();
        /** Gets the inversed matrix. */
        Matrix Inversed() const;

        /** Get the translation, rotation and scale of an affine matrix. */
        void Decompose(Simian::Vector3& translation, Simian::Vector4& rotation, Simian::Vector3& scale) const;
    };
}

Simian::Matrix operator*(Simian::f32 scalar, const Simian::Matrix& mat);

#endif

#endif
