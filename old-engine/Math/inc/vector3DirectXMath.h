#ifndef VECTOR3DIRECTXMATH_H
#define VECTOR3DIRECTXMATH_H

#include "configMath.h"
#if ENGINE_MATH == ENGINE_MATH_DIRECTXMATH

#include <DirectXPackedVector.h>

namespace Math
{
    template <typename T, typename U>
    class Vector3
    {
    public:
        
        static const Vector3<T,U> Zero;
        static const Vector3<T,U> UnitX;
        static const Vector3<T,U> UnitY;
        static const Vector3<T,U> UnitZ;
        static DirectX::XMVECTOR (*XMLoadFunc)(const U*);
        static void (*XMStoreFunc)(U*,DirectX::XMVECTOR);

    public:

        Vector3()
            :   data3D_(0,0,0)
        {
        }

        Vector3(T x,
                T y,
                T z)
                :   data3D_(x,y,z)
        {
        }

        Vector3(const Vector3<T,U>& val)
            :   data3D_(data3D_.x,data3D_.y,data3D_.z)
        {
        }
        
        Vector3<T,U>(const DirectX::XMVECTOR& val)
        {
            (*XMStoreFunc)(&data3D_, val);
        }

        Vector3<T,U> ProjectOnto(const Vector3<T,U>& val) const
        {
            DirectX::XMVECTOR b((*XMLoadFunc)(&val.data3D_));
            return Vector2<T,U>(DirectX::XMVectorMultiply(DirectX::XMVectorDivide(DirectX::XMVector3Dot((*XMLoadFunc)(&data3D_),b),DirectX::XMVector3LengthSq(b)),b));
        }

        Vector3<T,U> ReflectAbout(const Vector3<T,U>& val) const
        {
            return Vector3<T,U>(DirectX::XMVector3Reflect((*XMLoadFunc)(&data3D_),(*XMLoadFunc)(&val.data3D_)));
        }

        T Dot(const Vector3<T,U>& val) const
        {
            T vec;
            DirectX::XMVectorGetByIndexPtr(&vec,DirectX::XMVector3Dot((*XMLoadFunc)(&data3D_),(*XMLoadFunc)(&val.data3D_)),0);
            return vec;
        }

        Vector3<T,U> Cross(const Vector3<T,U>& val) const
        {
            return Vector3<T,U>(DirectX::XMVector3Cross((*XMLoadFunc)(&data3D_),(*XMLoadFunc)(&val.data3D_)));
        }

        void Normalize()
        {
            (*XMStoreFunc)(&data3D_,DirectX::XMVector3Normalize((*XMLoadFunc)(&data3D_)));
        }

        Vector3<T,U> Normalized() const
        {
            return Vector3<T,U>(DirectX::XMVector3Normalize((*XMLoadFunc)(&data3D_)));
        }

        T SquareLength() const
        {
            T val = 0;
            DirectX::XMVectorGetByIndexPtr(&val,DirectX::XMVector3LengthSq((*XMLoadFunc)(&data3D_)),0);
            return val;
        }

        T Length() const
        {
            T val = 0;
            DirectX::XMVectorGetByIndexPtr(&val,DirectX::XMVector3Length((*XMLoadFunc)(&data3D_)),0);
            return val;
        }

    public:
        Vector3<T,U>& operator=(const Vector3<T,U>& val)
        {
            data3D_ = val.data3D_;
            return *this;
        }
        
        Vector3<T,U>& operator=(const DirectX::XMVECTOR& val)
        {
            (*XMStoreFunc)(&data3D_, val);
            return *this;
        }

        Vector3<T,U> operator+(const Vector3<T,U>& val) const
        {
            return Vector3<T,U>(DirectX::XMVectorAdd((*XMLoadFunc)(&data3D_),(*XMLoadFunc)(&val.data3D_)));
        }

        Vector3<T,U> operator-(const Vector3<T,U>& val) const
        {
            return Vector3<T,U>(DirectX::XMVectorSubtract((*XMLoadFunc)(&data3D_),(*XMLoadFunc)(&val.data3D_)));
        }

        Vector3<T,U> operator-() const
        {
            return Vector3<T,U>(DirectX::XMVectorScale((*XMLoadFunc)(&data3D_),-1.f));
        }

        Vector3<T,U> operator*(T val) const
        {
            return Vector3<T,U>(DirectX::XMVectorScale((*XMLoadFunc)(&data3D_),val));
        }

        Vector3<T,U> operator/(T val) const
        {
            return Vector3<T,U>(DirectX::XMVectorScale((*XMLoadFunc)(&data3D_),1.f/val));
        }

        Vector3<T,U> operator+=(const Vector3<T,U>& val)
        {
            *this = *this + val;
            return *this;
        }

        Vector3<T,U> operator-=(const Vector3<T,U>& val)
        {
            *this = *this - val;
            return *this;
        }

        Vector3<T,U> operator*=(T val)
        {
            *this = *this * val;
            return *this;
        }

        Vector3<T,U> operator/=(T val)
        {
            *this = *this / val;
            return *this;
        }

    public:

        void X(T val)
        {
            data3D_.x = val;
        }

        T X() const
        {
            return data3D_.x;
        }

        void Y(T val)
        {
            data3D_.y = val;
        }

        T Y() const
        {
            return data3D_.y;
        }

        void Z(T val)
        {
            data3D_.z = val;
        }

        T Z() const
        {
            return data3D_.z;
        }

        void Set(T x,
                 T y,
                 T z)
        {
            data3D_.x = x;
            data3D_.y = y;
            data3D_.z = z;
        }

        U Data3D() const
        {
            return data3D_;
        }

    private:
        
        U data3D_;
    };

    // Int
    typedef Vector3<int,DirectX::XMINT3> Vec3I;
    const Vec3I Vec3I::Zero(0,0,0);
    const Vec3I Vec3I::UnitX(1,0,0);
    const Vec3I Vec3I::UnitY(0,1,0);
    const Vec3I Vec3I::UnitZ(0,0,1);
    DirectX::XMVECTOR (*Vec3I::XMLoadFunc)(const DirectX::XMINT3*) = &DirectX::XMLoadSInt3;
    void (*Vec3I::XMStoreFunc)(DirectX::XMINT3*,DirectX::XMVECTOR) = &DirectX::XMStoreSInt3;

    // Float
    typedef Vector3<float,DirectX::XMFLOAT3A> Vec3F;
    const Vec3F Vec3F::Zero(0,0,0);
    const Vec3F Vec3F::UnitX(1,0,0);
    const Vec3F Vec3F::UnitY(0,1,0);
    const Vec3F Vec3F::UnitZ(0,0,1);
    DirectX::XMVECTOR (*Vec3F::XMLoadFunc)(const DirectX::XMFLOAT3A*) = &DirectX::XMLoadFloat3A;
    void (*Vec3F::XMStoreFunc)(DirectX::XMFLOAT3A*,DirectX::XMVECTOR) = &DirectX::XMStoreFloat3A;

    // Double
    typedef Vec3F Vec3D;
}

template <typename T,typename U>
Math::Vector3<T,U> operator*(T a,
                             const Math::Vector3<T,U>& b)
{
    return b * a;
}

#endif
#endif