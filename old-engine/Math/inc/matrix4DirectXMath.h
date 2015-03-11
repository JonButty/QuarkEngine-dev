#ifndef MATRIX4DIRECTXMATH_H
#define MATRIX4DIRECTXMATH_H

#include "configMath.h"
#if ENGINE_MATH == ENGINE_MATH_DIRECTXMATH

#include "vector2.h"
#include "vector3.h"
#include "angle.h"

namespace Math
{
    template <typename T>
    class Matrix4
    {
    public:
        static Matrix4<T> Zero;
        static Matrix4<T> Identity;
    public:
        Matrix4()
        {
            *this = Identity;
        }

        Matrix4(T m00, T m01, T m02, T m03,
            T m10, T m11, T m12, T m13,
            T m20, T m21, T m22, T m23,
            T m30, T m31, T m32, T m33)
        {
            m_[0][0] = m00; m_[0][1] = m01; m_[0][2] = m02; m_[0][3] = m03;
            m_[1][0] = m10; m_[1][1] = m11; m_[1][2] = m12; m_[1][3] = m13;
            m_[2][0] = m20; m_[2][1] = m21; m_[2][2] = m22; m_[2][3] = m23;
            m_[3][0] = m30; m_[3][1] = m31; m_[3][2] = m32; m_[3][3] = m33;
        }

        Matrix4(const Matrix4<T>& val)
        {
            *this = val;
        }

        void Set(unsigned int r,
            unsigned int c,
            T val)
        {
            m_[c][r] = val;
        }

        void SetRow(unsigned int r,
            const T* const val)
        {
            for(unsigned int i = 0; i < 4; ++i)
                m_[i][r] = val[i];
        }

        void SetCol(unsigned int c,
            const T* const val)
        {
            for(unsigned int i = 0; i < 4; ++i)
                m_[c][i] = val[i];
        }

        T Get(unsigned int r,
            unsigned int c) const
        {
            return m_[c][r];
        }

        void GetRow(unsigned int r,
            T* val) const
        {
            for(unsigned int i = 0; i < 4; ++i)
                val[i] = m_[i][r];
        }

        void GetCol(unsigned int c,
            T* val) const
        {
            for(unsigned int i = 0; i < 4; ++i)
                val[i] = m_[c][i];
        }

        void Transpose()
        {
            std::swap(m_[0][1],m_[1][0]);
            std::swap(m_[2][0],m_[0][2]);
            std::swap(m_[2][1],m_[1][2]);
            std::swap(m_[3][0],m_[0][3]);
            std::swap(m_[3][1],m_[1][3]);
            std::swap(m_[3][2],m_[2][3]);
        }

        Matrix4<T> Transposed() const
        {
            return Matrix4<T>(m_[0][0], m_[1][0], m_[2][0], m_[3][0],
                m_[0][1], m_[1][1], m_[2][1], m_[3][1],
                m_[0][2], m_[1][2], m_[2][2], m_[3][2],
                m_[0][3], m_[1][3], m_[2][3], m_[3][3]);
        }

        Matrix4<T> Translated(const Vector2<T>& vec) const
        {
            return Matrix4<T>(1, 0, 0, vec.X(),
                0, 1, 0, vec.Y(),
                0, 0, 1, 0,
                0, 0, 0, 1);
        }

        Matrix4<T> Translated(const Vector3<T>& vec) const
        {
            return Matrix4<T>(1, 0, 0, vec.X(),
                0, 1, 0, vec.Y(),
                0, 0, 1, vec.Z(),
                0, 0, 0, 1);
        }

        void Translate(const Vector2<T>& vec) 
        {
            for(unsigned int i = 0; i < 2; ++i)
                m_[3][i] = vec[i];
        }

        void Translate(const Vector3<T>& vec)
        {
            for(unsigned int i = 0; i < 3; ++i)
                m_[3][i] = vec[i];
        }

        Matrix4<T> Scaled(const Vector2<T>& vec) const
        {
            return Matrix4<T>(vec.X(), 0, 0, 0,
                0, vec.Y(), 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1);
        }

        Matrix4<T> Scaled(const Vector3<T>& vec) const
        {
            return Matrix4<T>(vec.X(), 0, 0, 0,
                0, vec.Y(), 0, 0,
                0, 0, vec.Z(), 0,
                0, 0, 0, 1);
        }

        void Scale(const Vector2<T>& vec)
        {
            for(unsigned int i = 0; i < 2; ++i)
                m_[i][i] = vec[i];
        }

        void Scaled(const Vector3<T>& vec) 
        {
            for(unsigned int i = 0; i < 3; ++i)
                m_[i][i] = vec[i];
        }

        Matrix4<T> RotatedXAxis(const Angle<T>& angle) const
        {
            T cosTheta = std::cos(angle.AsRadians());
            T sinTheta = std::sin(angle.AsRadians());
            return Matrix4<T>(1, 0, 0, 0,
                0, cosTheta, -sinTheta, 0,
                0, sinTheta,  cosTheta, 0,
                0, 0, 0, 1);
        }

        Matrix4<T> RotatedYAxis(const Angle<T>& angle) const
        {
            T cosTheta = std::cos(angle.AsRadians());
            T sinTheta = std::sin(angle.AsRadians());
            return Matrix4<T>(cosTheta, 0, sinTheta, 0,
                0, 1, 0, 0,
                -sinTheta, 0, cosTheta, 0,
                0, 0, 0, 1);
        }

        Matrix4<T> RotatedZAxis(const Angle<T>& angle) const
        {
            T cosTheta = std::cos(angle.AsRadians());
            T sinTheta = std::sin(angle.AsRadians());
            return Matrix4<T>(cosTheta, -sinTheta, 0, 0,
                sinTheta,  cosTheta, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1);
        }

        void RotateXAxis(const Angle<T>& angle)
        {
            *this = Identity;

            T cosTheta = std::cos(angle.AsRadians());
            T sinTheta = std::sin(angle.AsRadians());
            m_[1][1] = m_[2][2] = cosTheta;
            m_[1][2] = sinTheta;
            m_[2][1] = -sinTheta;
        }

        void RotateYAxis(const Angle<T>& angle)
        {
            *this = Identity;

            T cosTheta = std::cos(angle.AsRadians());
            T sinTheta = std::sin(angle.AsRadians());
            m_[0][0] = m_[3][3] = cosTheta;
            m_[3][0] = sinTheta;
            m_[0][3] = -sinTheta;
        }

        void RotateZAxis(const Angle<T>& angle) 
        {
            *this = Identity;

            T cosTheta = std::cos(angle.AsRadians());
            T sinTheta = std::sin(angle.AsRadians());
            m_[0][0] = m_[1][1] = cosTheta;
            m_[0][1] = sinTheta;
            m_[1][0] = -sinTheta;
        }

        Matrix4<T> RotatedAxisAngle(const Vector3<T>& axis,
            const Angle<T>& angle) const
        {
            T cosTheta = std::cos(angle.AsRadians());
            T sinTheta = std::sin(angle.AsRadians());
            T t = 1 - cosTheta;
            Vector3<T> n = axis.Normalized();

            return Matrix4<T>(t * n.X() * n.X() + c, t * n.X() * n.Y() - n.Z() * s, t * n.X() * n.Z() + n.Y() * s, 0,
                t * n.X() * n.Y() + n.Z() * s, t * n.Y() * n.Y() + c, t * n.Y() * n.Z() - n.X() * s, 0,
                t * n.X() * n.Z() - n.Y() * s, t * n.Y() * n.Z() + n.X() * s, t * n.Z() * n.Z() + c, 0,
                0, 0, 0, 1);
        }

        void RotateAxisAngle(const Vector3<T>& axis,
            const Angle<T>& angle) 
        {
            Matrix4<T> rotMtx(RotatedAxisAngle(axis,angle));
            *this = rotMtx * (*this);
        }

    public:
        Matrix4<T> operator-() const
        {
            Matrix4<T> result = Matrix4<T>::Zero;
            for(unsigned int r = 0; r < 4; ++r)
                for(unsigned int c = 0; c < 4; ++c)
                    result[c][r] = m_[c][r] * -1;
            return result;
        }

        Matrix4<T> operator-(const Matrix4<T>& val) const
        {
            for(unsigned int r = 0; r < 4; ++r)
                for(unsigned int c = 0; c < 4; ++c)
                    result[c][r] = m_[c][r] - val[c][r];
            return result;
        }

        Matrix4<T> operator+(const Matrix4<T>& val) const
        {
            for(unsigned int r = 0; r < 4; ++r)
                for(unsigned int c = 0; c < 4; ++c)
                    result[c][r] = m_[c][r] + val[c][r];
            return result;
        }

        Matrix4<T> operator*(T val) const
        {
            Matrix4<T> result = Matrix4<T>::Zero;
            for(unsigned int r = 0; r < 4; ++r)
                for(unsigned int c = 0; c < 4; ++c)
                    result[c][r] = m_[c][r] * val;
            return result;
        }

        Matrix4<T> operator*(const Matrix4<T>& val) const
        {
            Matrix4<T> result = Matrix4<T>::Zero;
            for(unsigned int r = 0; r < 4; ++r)
                for(unsigned int c = 0; c < 4; ++c)
                    for(unsigned int i = 0; i < 4; ++i)
                        result[c][r] += m_[i][r] * val.m_[c][i];
            return result;
        }

        Matrix4<T>& operator-=(const Matrix4<T>& val) 
        {
            for(unsigned int r = 0; r < 4; ++r)
                for(unsigned int c = 0; c < 4; ++c)
                    m_[c][r] -= val[c][r];
            return *this;
        }

        Matrix4<T>& operator+=(const Matrix4<T>& val) 
        {
            for(unsigned int r = 0; r < 4; ++r)
                for(unsigned int c = 0; c < 4; ++c)
                    m_[c][r] += val[c][r];
            return *this;
        }

        Matrix4<T>& operator*=(T val) const
        {
            for(unsigned int r = 0; r < 4; ++r)
                for(unsigned int c = 0; c < 4; ++c)
                    m_[c][r] *= val;
            return *this;
        }

        Matrix4<T>& operator*=(const Matrix4<T>& val) const
        {
            Matrix4<T> result = Matrix4<T>::Zero;
            for(unsigned int r = 0; r < 4; ++r)
                for(unsigned int c = 0; c < 4; ++c)
                    for(unsigned int i = 0; i < 4; ++i)
                        result[c][r] += m_[i][r] * val.m_[c][i];
            *this = result;
            return *this;
        }

        Matrix4<T>& operator=(const Matrix4<T>& val)
        {
            for(unsigned int r = 0; r < 4; ++r)
                for(unsigned int c = 0; c < 4; ++c)
                    m_[c][r] = val.m_[c][r];
            return *this;
        }

        T* operator[](unsigned int index) 
        {
            return m_[index];
        }

    private:
        T m_[4][4];
    };

    typedef Matrix4<float> Mtx4F;
    Mtx4F Mtx4F::Zero(0,0,0,0,
        0,0,0,0,
        0,0,0,0,
        0,0,0,0);

    Mtx4F Mtx4F::Identity(1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1);
}

#endif

#endif