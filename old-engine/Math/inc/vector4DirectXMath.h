#ifndef VECTOR4DIRECTXMATH_H
#define VECTOR4DIRECTXMATH_H

#include "configMath.h"
#if ENGINE_MATH == ENGINE_MATH_DIRECTXMATH

#include <DirectXPackedVector.h>

namespace Math
{
    template <typename T, typename U>
    class Vector4
    {
    public:

        static const Vector4<T,U> Zero;
        static const Vector4<T,U> UnitX;
        static const Vector4<T,U> UnitY;
        static const Vector4<T,U> UnitZ;
        static const Vector4<T,U> UnitW;
        static DirectX::XMVECTOR (*XMLoadFunc)(const U*);
        static void (*XMStoreFunc)(U*,DirectX::XMVECTOR);

    public:
        Vector4()
        {
            U val(0,0,0,0);
            data4D_ = (*XMLoadFunc)(&val);
        }

        Vector4(T x,
                T y,
                T z,
                T w)
        {
            U val(x,y,z,w);
            data4D_ = (*XMLoadFunc)(&val);
        }

        Vector4(const Vector4<T,U>& val) 
        {
            data4D_ = val.data4D_;
        }

        Vector4(const DirectX::XMVECTOR& val) 
        {
            data4D_ = val;
        }

        Vector4<T,U> ProjectOnto(const Vector4<T,U>& val) const
        {
            return Vector4<T,U>(DirectX::XMVectorMultiply(DirectX::XMVectorDivide(DirectX::XMVector4Dot(data4D_,val.data4D_),DirectX::XMVector4LengthSq(val.data4D_)),val.data4D_));
        }

        Vector4<T,U> ReflectAbout(const Vector4<T,U>& val) const
        {
            return Vector4<T,U>(DirectX::XMVector4Reflect(data4D_,val.data4D_));
        }

        T Dot(const Vector4<T,U>& val) const
        {
            T vec;
            DirectX::XMVectorGetByIndexPtr(&vec,DirectX::XMVector4Dot(data4D_,val.data4D_),0);
            return vec;
        }

        void Normalize()
        {
            (*XMStoreFunc)(&data4D_,DirectX::XMVector4Normalize((*XMLoadFunc)(&data4D_)));
        }

        Vector4<T,U> Normalized() const
        {
            return Vector4<T,U>(DirectX::XMVector4Normalize((*XMLoadFunc)(&data4D_)));
        }

        T SquareLength() const
        {
            T val = 0;
            DirectX::XMVectorGetByIndexPtr(&val,DirectX::XMVector4LengthSq((*XMLoadFunc)(&data4D_)),0);
            return val;
        }

        T Length() const
        {
            T val = 0;
            DirectX::XMVectorGetByIndexPtr(&val,DirectX::XMVector4Length((*XMLoadFunc)(&data4D_))a,0);
            return val;
        }

    public:
        Vector4<T,U>& operator=(const Vector4<T,U>& val)
        {
            data4D_ = val.data4D_;
            return *this;
        }

        Vector4<T,U>& operator=(const DirectX::XMVECTOR& val)
        {
            (*XMStoreFunc)(&data4D_, val);
            return *this;
        }

        Vector4<T,U> operator+(const Vector4<T,U>& val) const
        {
            return Vector4<T,U>(DirectX::XMVectorSubtract((*XMLoadFunc)(&data4D_),(*XMLoadFunc)(&val.data4D_)));
        }

        Vector4<T,U> operator-(const Vector4<T,U>& val) const
        {
            return Vector4<T,U>(DirectX::XMVectorSubtract((*XMLoadFunc)(&data4D_),(*XMLoadFunc)(&val.data4D_)));
        }

        Vector4<T,U> operator-() const
        {
            return Vector4<T,U>(DirectX::XMVectorScale((*XMLoadFunc)(&data4D_),-1.f));
        }

        Vector4<T,U> operator*(T val) const
        {
            return Vector4<T,U>(DirectX::XMVectorScale((*XMLoadFunc)(&data4D_),val));
        }

        Vector4<T,U> operator/(T val) const
        {
            return Vector4<T,U>(DirectX::XMVectorScale((*XMLoadFunc)(&data4D_),1.f/val));
        }

        Vector4<T,U>& operator+=(const Vector4<T,U>& val)
        {
            *this = *this + val;
            return *this;
        }

        Vector4<T,U>& operator-=(const Vector4<T,U>& val)
        {
            *this = *this - val;
            return *this;
        }

        Vector4<T,U>& operator*=(T val)
        {
            *this = *this * val;
            return *this;
        }

        Vector4<T,U>& operator/=(T val)
        {
            *this = *this / val;
            return *this;
        }

    public:

        void X(T val)
        {
            data4D_ = DirectX::XMVectorSetByIndex(data4D_,static_cast<float>(val),0);
        }

        T X() const
        {
            return static_cast<T>(DirectX::XMVectorGetByIndex(data4D_,0));
        }

        void Y(T val)
        {
            data4D_ = DirectX::XMVectorSetByIndex(data4D_,static_cast<float>(val),1);
        }

        T Y() const
        {
            return static_cast<T>(DirectX::XMVectorGetByIndex(data4D_,1));
        }

        void Z(T val)
        {
            data4D_ = DirectX::XMVectorSetByIndex(data4D_,static_cast<float>(val),2);
        }

        T Z() const
        {
            return static_cast<T>(DirectX::XMVectorGetByIndex(data4D_,2));
        }

        void W(T val)
        {
            data4D_ = DirectX::XMVectorSetByIndex(data4D_,static_cast<float>(val),3);
        }

        T W() const
        {
            return static_cast<T>(DirectX::XMVectorGetByIndex(data4D_,3));
        }

        void Set(T x,
                 T y,
                 T z,
                 T w)
        {
            data4D_ = DirectX::XMVectorSet(x,y,z,w);
        }

        U Data4D() const
        {
            U data;
            (*XMStoreFunc)(&data,data4D_);
            return data;
        }

    protected:

        DirectX::XMVECTOR data4D_;
    };

    // Int
    typedef Vector4<int,DirectX::XMINT4> Vec4I;
    const Vec4I Vec4I::Zero(0,0,0,0);
    const Vec4I Vec4I::UnitX(1,0,0,0);
    const Vec4I Vec4I::UnitY(0,1,0,0);
    const Vec4I Vec4I::UnitZ(0,0,1,0);
    const Vec4I Vec4I::UnitW(0,0,0,1);
    DirectX::XMVECTOR (*Vec4I::XMLoadFunc)(const DirectX::XMINT4*) = &DirectX::XMLoadSInt4;
    void (*Vec4I::XMStoreFunc)(DirectX::XMINT4*,DirectX::XMVECTOR) = &DirectX::XMStoreSInt4;

    // Float
    typedef Vector4<float,DirectX::XMFLOAT4A> Vec4F;
    const Vec4F Vec4F::Zero(0,0,0,0);
    const Vec4F Vec4F::UnitX(1,0,0,0);
    const Vec4F Vec4F::UnitY(0,1,0,0);
    const Vec4F Vec4F::UnitZ(0,0,1,0);
    const Vec4F Vec4F::UnitW(0,0,0,1);
    DirectX::XMVECTOR (*Vec4F::XMLoadFunc)(const DirectX::XMFLOAT4A*) = &DirectX::XMLoadFloat4A;
    void (*Vec4F::XMStoreFunc)(DirectX::XMFLOAT4A*,DirectX::XMVECTOR) = &DirectX::XMStoreFloat4A;

    // Double
    typedef Vec4F Vec4D;
}

#endif
#endif