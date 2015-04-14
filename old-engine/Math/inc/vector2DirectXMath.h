#ifndef VECTOR2DIRECTXMATH_H
#define VECTOR2DIRECTXMATH_H

#include "configMath.h"
#if ENGINE_MATH == ENGINE_MATH_DIRECTXMATH

#include <DirectXPackedVector.h>

namespace Math
{
    template <typename T, typename U>
    class Vector2
    {
    public:
    
        static const Vector2<T,U> Zero;
        static const Vector2<T,U> UnitX;
        static const Vector2<T,U> UnitY;
        static DirectX::XMVECTOR (*XMLoadFunc)(const U*);
        static void (*XMStoreFunc)(U*,DirectX::XMVECTOR);
    
    public:

        Vector2()
            :   data2D_(0,0)
        {
        }

        Vector2(T x,
                T y)
            :  data2D_(x,y)
        {
        }

        Vector2(const Vector2<T,U>& val)
            :  data2D_(val.data2D_.x,val.data2D_.y)
        {
        }

        Vector2<T,U>(const DirectX::XMVECTOR& val)
        {
            (*XMStoreFunc)(&data2D_, val);
        }

        Vector2<T,U> ProjectOnto(const Vector2<T,U>& val) const
        {
            DirectX::XMVECTOR b((*XMLoadFunc)(&val.data2D_));
            return Vector2<T,U>(DirectX::XMVectorMultiply(DirectX::XMVectorDivide(DirectX::XMVector2Dot((*XMLoadFunc)(&data2D_),b),DirectX::XMVector2LengthSq(b)),b));
        }

        Vector2<T,U> ReflectAbout(const Vector2<T,U>& val) const
        {
            return Vector2<T,U>(DirectX::XMVector2Reflect((*XMLoadFunc)(&data2D_),(*XMLoadFunc)(&val.data2D_)));
        }

        T Dot(const Vector2<T,U>& val) const
        {
            T vec;
            DirectX::XMVectorGetByIndexPtr(&vec,DirectX::XMVector2Dot((*XMLoadFunc)(&data2D_),(*XMLoadFunc)(&val.data2D_)),0);
            return vec;
        }

        void Normalize()
        {
            (*XMStoreFunc)(&data2D_,DirectX::XMVector2Normalize((*XMLoadFunc)(&data2D_)));
        }

        Vector2<T,U> Normalized() const
        {
            return Vector2<T,U>(DirectX::XMVector2Normalize((*XMLoadFunc)(&data2D_)));
        }

        T SquareLength() const
        {
            T val = 0;
            DirectX::XMVectorGetByIndexPtr(&val,DirectX::XMVector2LengthSq((*XMLoadFunc)(&data2D_)),0);
            return val;
        }

        T Length() const
        {
            T val = 0;
            DirectX::XMVectorGetByIndexPtr(&val,DirectX::XMVector2Length((*XMLoadFunc)(&data2D_)),0);
            return val;
        }

    public:

        Vector2<T,U>& operator=(const Vector2<T,U>& val)
        {
            data2D_ = val.data2D_;
            return *this;
        }

        Vector2<T,U>& operator=(const DirectX::XMVECTOR& val)
        {
            (*XMStoreFunc)(&data2D_, val);
            return *this;
        }

        Vector2<T,U> operator+(const Vector2<T,U>& val) const
        {
            return Vector2<T,U>(DirectX::XMVectorAdd((*XMLoadFunc)(&data2D_),(*XMLoadFunc)(&val.data2D_)));
        }

        Vector2<T,U> operator-(const Vector2<T,U>& val) const
        {
            return Vector2<T,U>(DirectX::XMVectorSubtract((*XMLoadFunc)(&data2D_),(*XMLoadFunc)(&val.data2D_)));
        }

        Vector2<T,U> operator-() const
        {
            return Vector2<T,U>(DirectX::XMVectorScale((*XMLoadFunc)(&data2D_),-1.f));
        }

        Vector2<T,U> operator*(T val) const
        {
            return Vector2<T,U>(DirectX::XMVectorScale((*XMLoadFunc)(&data2D_),val));
        }

        Vector2<T,U> operator/(T val) const
        {
            return Vector2<T,U>(DirectX::XMVectorScale((*XMLoadFunc)(&data2D_),1.f/val));
        }

        Vector2<T,U>& operator+=(const Vector2<T,U>& val)
        {
            *this = *this + val;
            return *this;
        }

        Vector2<T,U>& operator-=(const Vector2<T,U>& val)
        {
            *this = *this - val;
            return *this;
        }

        Vector2<T,U>& operator*=(T val)
        {
            *this = *this * val;
            return *this;
        }

        Vector2<T,U>& operator/=(T val)
        {
            *this = *this / val;
            return *this;
        }

    public:

        void X(T val)
        {
            data2D_.x = val;
        }

        T X() const
        {
            return data2D_.x;
        }

        void Y(T val)
        {
            data2D_.y = val;
        }

        T Y() const
        {
            return data2D_.y;
        }

        void Set(T x,
                 T y)
        {
            data2D_.x = x;
            data2D_.y = y;
        }

        U Data2D() const
        {
            return data2D_;
        }

    private:

        U data2D_;
    };

    typedef Vector2<int,DirectX::XMINT2> Vec2I;
    const Vec2I Vec2I::Zero(0,0);
    const Vec2I Vec2I::UnitX(1,0);
    const Vec2I Vec2I::UnitY(0,1);
    DirectX::XMVECTOR (*Vec2I::XMLoadFunc)(const DirectX::XMINT2*) = &DirectX::XMLoadSInt2;
    void (*Vec2I::XMStoreFunc)(DirectX::XMINT2*,DirectX::XMVECTOR) = &DirectX::XMStoreSInt2;

    typedef Vector2<float,DirectX::XMFLOAT2A> Vec2F;
    const Vec2F Vec2F::Zero(0,0);
    const Vec2F Vec2F::UnitX(1,0);
    const Vec2F Vec2F::UnitY(0,1);
    DirectX::XMVECTOR (*Vec2F::XMLoadFunc)(const DirectX::XMFLOAT2A*) = &DirectX::XMLoadFloat2A;
    void (*Vec2F::XMStoreFunc)(DirectX::XMFLOAT2A*,DirectX::XMVECTOR) = &DirectX::XMStoreFloat2A;

    typedef Vec2F Vec2D;
}

template <typename T, typename U>
U operator*(T a,
            const U& b)
{
    return b * a;
}

#endif
#endif