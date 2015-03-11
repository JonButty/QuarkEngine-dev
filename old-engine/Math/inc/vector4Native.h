#ifndef VECTOR4NATIVE_H
#define VECTOR4NATIVE_H

#include "configMath.h"
#if ENGINE_MATH == ENGINE_MATH_NATIVE

namespace Math
{
    template <typename T>
	class Vector4
	{
    public:
        static const Vector4<T> Zero;
        static const Vector4<T> UnitX;
        static const Vector4<T> UnitY;
        static const Vector4<T> UnitZ;
        static const Vector4<T> UnitW;
    public:
        Vector4()
            :   x_(0),
                y_(0),
                z_(0),
                w_(0)
        {
        }

        Vector4(T x,
                T y,
                T z,
                T w)
                :   x_(x),
                    y_(y),
                    z_(z),
                    w_(w)
        {
        }

        Vector4(const Vector4<T>& val) 
            :   x_(val.x_),
                y_(val.y_),
                z_(val.z_),
                w_(val.w_)
        {
        }

        Vector4<T> ProjectOnto(const Vector4<T>& val) const
        {
            return (Dot(val) / val.SquareLength()) * val;
        }

        Vector4<T> ReflectAbout(const Vector4<T>& val) const
        {
            return *this - 2 * ProjectOnto(val.Normalized());
        }

        T Dot(const Vector4<T>& val) const
        {
            return (x_ * val.x_) + (y_ * val.y_) + (z_ * val.z_) + (w_ * val.w_);
        }

        void Normalize()
        {
            *this /= Length();
        }

        Vector4<T> Normalized() const
        {
            return *this / Length();
        }

        T SquareLength() const
        {
            return Dot(*this);
        }

        T Length() const
        {
            return std::sqrt(SquareLength());
        }

    public:
        Vector4<T>& operator=(const Vector4<T>& val)
        {
            x_ = val.x_;
            y_ = val.y_;
            z_ = val.z_;
            w_ = val.w_;
            return *this;
        }

        Vector4<T> operator+(const Vector4<T>& val) const
        {
            return Vector4(x_ + val.x_,
                           y_ + val.y_,
                           z_ + val.z_,
                           w_ + val.w_);
        }

        Vector4<T> operator-(const Vector4<T>& val) const
        {
            return Vector4(x_ - val.x_,
                           y_ - val.y_,
                           z_ - val.z_,
                           w_ - val.w_);
        }

        Vector4<T> operator-() const
        {
            return Vector4(-x_,
                           -y_,
                           -z_,
                           -w_);
        }

        Vector4<T> operator*(T val) const
        {
            return Vector4(x_ * val,
                           y_ * val,
                           z_ * val,
                           w_ * val);
        }

        Vector4<T> operator/(T val) const
        {
            return Vector4(static_cast<T>(x_ / val),
                           static_cast<T>(y_ / val),
                           static_cast<T>(z_ / val),
                           static_cast<T>(w_ / val));
        }

        Vector4<T>& operator+=(const Vector4<T>& val)
        {
            x_ += val.x_;
            y_ += val.y_;
            z_ += val.z_;
            w_ += val.w_;
            return *this;
        }

        Vector4<T>& operator-=(const Vector4<T>& val)
        {
            x_ -= val.x_;
            y_ -= val.y_;
            z_ -= val.z_;
            w_ -= val.w_;
            return *this;
        }

        Vector4<T>& operator*=(T val)
        {
            x_ *= val;
            y_ *= val;
            z_ *= val;
            w_ *= val;
            return *this;
        }

        Vector4<T>& operator/=(T val)
        {
            x_ = static_cast<T>(x_ / val);
            y_ = static_cast<T>(y_ / val);
            z_ = static_cast<T>(z_ / val);
            w_ = static_cast<T>(w_ / val);
            return *this;
        }

        const T& operator[](unsigned int index) const
        {
            return v_[index];
        }

        T& operator[](unsigned int index)
        {
            return v_[index];
        }

    public:
        void X(T val)
        {
            x_ = val;
        }
        
        T X() const
        {
            return x_;
        }

        void Y(T val)
        {
            y_ = val;
        }

        T Y() const
        {
            return y_;
        }

        void Z(T val)
        {
            z_ = val;
        }
        
        T Z() const
        {
            return z_;
        }

        void W(T val)
        {
            w_ = val;
        }

        T W() const
        {
            return w_;
        }

    protected:
        union
        {
            struct
            {
                T x_;
                T y_;
                T z_;
                T w_;
            };
            T v_[4];
        };
	};

    // Int
    typedef Vector4<int> Vec4I;
    const Vec4I Vec4I::Zero(0,0,0,0);
    const Vec4I Vec4I::UnitX(1,0,0,0);
    const Vec4I Vec4I::UnitY(0,1,0,0);
    const Vec4I Vec4I::UnitZ(0,0,1,0);
    const Vec4I Vec4I::UnitW(0,0,0,1);

    // Float
    typedef Vector4<float> Vec4F;
    const Vec4F Vec4F::Zero(0,0,0,0);
    const Vec4F Vec4F::UnitX(1,0,0,0);
    const Vec4F Vec4F::UnitY(0,1,0,0);
    const Vec4F Vec4F::UnitZ(0,0,1,0);
    const Vec4F Vec4F::UnitW(0,0,0,1);

    // Double
    typedef Vector4<double> Vec4D;
    const Vec4D Vec4D::Zero(0,0,0,0);
    const Vec4D Vec4D::UnitX(1,0,0,0);
    const Vec4D Vec4D::UnitY(0,1,0,0);
    const Vec4D Vec4D::UnitZ(0,0,1,0);
    const Vec4D Vec4D::UnitW(0,0,0,1);
}

template <typename T>
Math::Vector4<T> operator*(T a,
                           const Math::Vector4<T>& b)
{
    return b * a;
}

#endif
#endif