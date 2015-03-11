#ifndef QUATERNIONDIRECTXMATH_H
#define QUATERNIONDIRECTXMATH_H

#include "configMath.h"
#if ENGINE_MATH == ENGINE_MATH_DIRECTXMATH

#include <DirectXPackedVector.h>

namespace Math
{
    template <typename T,typename U,typename V>
	class Quaternion
        :   public Vector4<T,U>
    {
    public:

        static const Quaternion<T,U,V> Identity(DirectX::XMQuaternionIdentity());

    public:
        // x,y,z represent i,j,k basis vectors and w is the real part
        Quaternion()
            :   Vector4<T,U>(Identity)
        {
        }

        Quaternion(T x,
                   T y,
                   T z,
                   T w)
                   :    Vector4<T,U>(x,y,z,w)
        {
        }

        Quaternion(const Vector4<T,U>& val)
            :   Vector4<T,U>(val)
        {
        }
        
        Quaternion(const DirectX::XMVECTOR& val)
            :   Vector4<T,U>(val)
        {
        }

        Quaternion(T real,
                   const Vector3<T,V>& val)
            :   Vector4<T,U>(val.X(),val.Y(),val.Z(),real)
        {
        }

        Quaternion<T,U,V> Conjugate() const
        {
            return DirectX::XMQuaternionConjugate(data4D_);
        }

        Quaternion<T,U,V> Inverse() const
        {
            return Quaternion<T,U,V>(DirectX::XMQuaternionConjugate(data4D_)/DirectX::XMQuaternionLengthSq(data4D_));
        }

        static Matrix4<T> Convert(const Quaternion<T,U,V>& quat) 
        {
            U axis;
            T angle;
            DirectX::XMQuaternionToAxisAngle(&axis,&angle,quat.data4D_);
            
            Matrix4<T> rotation;
            rotation.RotateAxisAngle(axis,
                                     angle);
            return rotation;
        }

        static void Convert(const Quaternion<T,U,V>& quat,
                            Radian<T>& angle,
                            Vector3<T,V>& axis) 
        {
            U xmAxis;
            T xmAngle;
            DirectX::XMQuaternionToAxisAngle(&axis,&angle,quat.data4D_);

            angle = xmAngle;
            axis = xmAxis;
        }

        static Quaternion<T,U,V> Convert(const Radian<T>& angle,
                                         const Vector3<T,V>& axis)
        {
            if(std::abs(angle.AsRadians()) <= Math::EPSILON_DBL)
                return Quaternion<T,U,V>::Identity;
            else
            {
                T theta = angle.AsRadians() * T(0.5);
                T sinHalfTheta = std::sin(theta);
                T cosHalfTheta = std::cos(theta);

                return Quaternion<T,U,V>(cosHalfTheta, sinHalfTheta * axis);
            }
        }

        // Used for Shoemake interpolation
        static Quaternion<T,U,V> Double(const Quaternion<T>& p0,
                                        const Quaternion<T>& p1)
        {
            const DirectX::XMVECTOR& a(p0.data4D_);
            const DirectX::XMVECTOR& b(p1.data4D_);
            return Quaternion<T,U,V>((T(2) * DirectX::XMVector4Dot(a,b) * b) - a);
        }

        // Used for Shoemake interpolation
        static Quaternion<T,U,V> Bisect(const Quaternion<T>& p0,
                                        const Quaternion<T>& p1)
        {
            const DirectX::XMVECTOR& a(p0.data4D_);
            const DirectX::XMVECTOR& b(p1.data4D_);
            return Quaternion<T,U,V>(DirectX::XMQuaternionNormalize(a + b));
        }

        // Assumes that quaternions are unit length
        static Quaternion<T,U,V> Slerp(const Quaternion<T>& p0,
                                       const Quaternion<T>& p1,
                                       T t)
        {
            return Quaternion<T,U,V>(DirectX::XMQuaternionSlerp(p0.data4D_,p1.data4D_,static_cast<float>(t)));
        }

        static Quaternion<T,U,V> QuadSlerp(const Quaternion<T,U,V>& p0,
                                           const Quaternion<T,U,V>& p1,
                                           const Quaternion<T,U,V>& p2,
                                           T t)
        {
            float time = static_cast<float>(t);
            return Quaternion<T,U,V>(DirectX::XMQuaternionSlerp(
                                     DirectX::XMQuaternionSlerp(p0.data4D_,p1.data4D_,time),
                                     DirectX::XMQuaternionSlerp(p1.data4D_,p2.data4D_,time),
                                     time));
        }

        static Quaternion<T,U,V> CubicSlerp(const Quaternion<T,U,V>& p0,
                                            const Quaternion<T,U,V>& p1,
                                            const Quaternion<T,U,V>& p2,
                                            const Quaternion<T,U,V>& p3,
                                            T t)
        {
            float time = static_cast<float>(t);
            DirectX::XMVECTOR p1p2(DirectX::XMQuaternionSlerp(p1.data4D_,p2.data4D_,time));
            return DirectX::XMQuaternionSlerp(
                   DirectX::XMQuaternionSlerp(DirectX::XMQuaternionSlerp(p0.data4D_,p1.data4D_,time),p1p2,time),
                   DirectX::XMQuaternionSlerp(p1p2,DirectX::XMQuaternionSlerp(p2.data4D_,p3.data4D_,time),time),
                   time);
        }

        // Not sure if this works correctly
        static void SquadSetup(Quaternion<T,U,V>& s,
                               const Quaternion<T,U,V>& q0,
                               const Quaternion<T,U,V>& q1,
                               const Quaternion<T,U,V>& q2)
        {
            DirectX::XMVECTOR a,b,c;
            DirectX::XMQuaternionSquadSetup(&a,&b,&c,q0.data4D_,q1.data4D_,q2.data4D_,q2.data4D_);
            s = a;
        }

        // Not sure if this works correctly
        static Quaternion<T,U,V> Squad(const Quaternion<T,U,V>& q0,
                                       const Quaternion<T,U,V>& q1,
                                       const Quaternion<T,U,V>& s0,
                                       const Quaternion<T,U,V>& s1,
                                       T t)
        {
            return Quaternion<T,U,V>(DirectX::XMQuaternionSquad(q0.data4D_,q1.data4D_,s0.data4D_,s1.data4D_,static_cast<float>(t)));
        }

        static Quaternion<T,U,V> Log(const Quaternion<T>& val) 
        {
            return Quaternion<T,U,V>(DirectX::XMQuaternionLn(val.data4D_));
        }

        static Quaternion<T,U,V> Exp(const Quaternion<T,U,V>& val) 
        {
            return Quaternion<T,U,V>(DirectX::XMQuaternionExp(val.data4D_));
        }

    public:
        Vector3<T,V> Imaginary() const
        {
            return Vector3<T,V>(DirectX::XMVectorGetX(data4D_),
                                DirectX::XMVectorGetY(),
                                DirectX::XMVectorGetZ());
        }

        void Imaginary(const Vector3<T,V>& val)
        {
            data4D_ = (*XMLoadFunc)(val.Data3D());
        }

        T Real() const
        {
            return static_cast<T>(DirectX::XMVectorGetW(data4D_));
        }

        void Real(T val)
        {
            DirectX::XMVectorSetW(data4D_,static_cast<float>(val));
        }

    public:
        Quaternion<T,U,V> operator*(const Quaternion<T,U,V>& val) const
        {
            return Quaternion<T,U,V>(DirectX::XMQuaternionMultiply(data4D_,val.data4D_));
        }

        Quaternion<T,U,V> operator*(T val) const
        {
            return Quaternion<T,U,V>(data4D_ * val);
        }

        Quaternion<T,U,V> operator/(T val) const
        {
            return Quaternion<T,U,V>(data4D_ / val);
        }

        Quaternion<T,U,V>& operator*=(const Quaternion<T,U,V>& val)
        {
            *this = *this * val;
            return *this;
        }

        Quaternion<T,U,V>& operator*=(T val)
        {
            *this = *this * val;
            return *this;
        }

        Quaternion<T,U,V>& operator/=(T val)
        {
            *this = *this / val;
            return *this;
        }
	};

    // Float
    typedef Quaternion<float,DirectX::XMFLOAT4A,DirectX::XMFLOAT3A> QuatF;
    const QuatF QuatF::Identity(0,0,0,1);
    DirectX::XMVECTOR (*QuatF::XMLoadFunc)(const DirectX::XMFLOAT4A*) = &DirectX::XMLoadSInt4;
    void (*QuatF::XMStoreFunc)(DirectX::XMFLOAT4A*,DirectX::XMVECTOR) = &DirectX::XMStoreSInt4;

    // Double
    typedef QuatF QuatD;
}

#endif
#endif