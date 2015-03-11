#ifndef VECTOR2NATIVE_H
#define VECTOR2NATIVE_H

#include "configMath.h"
#if ENGINE_MATH == ENGINE_MATH_NATIVE

namespace Math
{
    template <typename T>
    class Vector2
    {
    public:
        static const Vector2<T> Zero;
        static const Vector2<T> UnitX;
        static const Vector2<T> UnitY;
    public:
        Vector2()
            :   x_(0),
                y_(0)
        {
        }

        Vector2(T x,
                T y)
            :  x_(x),
               y_(y)
        {
        }

        Vector2(const Vector2<T>& val)
            :   x_(val.x_),
                y_(val.y_)
        {
        }

        Vector2<T> ProjectOnto(const Vector2<T>& val) const
        {
            return (Dot(val) / val.SquareLength()) * val;
        }

        Vector2<T> ReflectAbout(const Vector2<T>& val) const
        {
            return T(2) * (ProjectOnto(val.Normalized()) - *this ) + *this;
        }

        T Dot(const Vector2<T>& val) const
        {
            return (x_ * val.x_) + (y_ * val.y_);
        }

        void Normalize()
        {
            *this /= Length();
        }

        Vector2<T> Normalized() const
        {
            return *this / Length();
        }

        T SquareLength() const
        {
            return Dot(*this);
        }

        T Length() const
        {
            return static_cast<T>(std::sqrt(SquareLength()));
        }

    public:
        Vector2<T>& operator=(const Vector2<T>& val)
        {
            x_ = val.x_;
            y_ = val.y_;
            return *this;
        }

        Vector2<T> operator+(const Vector2<T>& val) const
        {
            Vector2<T> ret(x_ + val.x_, y_ + val.y_);
            return ret;
        }

        Vector2<T> operator-(const Vector2<T>& val) const
        {
            Vector2<T> ret(x_ - val.x_, y_ - val.y_);
            return ret;
        }

        Vector2<T> operator*(T val) const
        {
            Vector2<T> ret(x_ * val, y_ * val);
            return ret;
        }

        Vector2<T> operator/(T val) const
        {
            Vector2<T> ret(static_cast<T>(x_ / val),
                           static_cast<T>(y_ / val));
            return ret;
        }

        Vector2<T>& operator+=(const Vector2<T>& val)
        {
            x_ += val.x_;
            y_ += val.y_;
            return *this;
        }

        Vector2<T>& operator-=(const Vector2<T>& val)
        {
            x_ -= val.x_;
            y_ -= val.y_;
            return *this;
        }

        Vector2<T>& operator*=(T val)
        {
            x_ *= val;
            y_ *= val;
            return *this;
        }

        Vector2<T>& operator/=(T val)
        {
            x_ = static_cast<T>(x_ / val);
            y_ = static_cast<T>(y_ / val);
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

    private:
        union 
        {
            struct
            {
                T x_;
                T y_;
            };
            T v_[2];
        };
    };

    // Int
    typedef Vector2<int> Vec2I;
    const Vec2I Vec2I::Zero(0,0);
    const Vec2I Vec2I::UnitX(1,0);
    const Vec2I Vec2I::UnitY(0,1);

    // Float
    typedef Vector2<float> Vec2F;
    const Vec2F Vec2F::Zero(0,0);
    const Vec2F Vec2F::UnitX(1,0);
    const Vec2F Vec2F::UnitY(0,1);

    // Double
    typedef Vector2<double> Vec2D;
    const Vec2D Vec2D::Zero(0,0);
    const Vec2D Vec2D::UnitX(1,0);
    const Vec2D Vec2D::UnitY(0,1);
}

template <typename T>
Math::Vector2<T> operator*(T a,
                           const Math::Vector2<T>& b)
{
    return Math::Vector2<T>(b.X() * a,
                            b.Y() * a);
}

#endif

#endif