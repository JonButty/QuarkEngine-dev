/*!*****************************************************************************

\class  QEQuaternion
\date 	2015/03/28
\author	Jonathan Butt
\brief	Quaternion class

*******************************************************************************/
#ifndef QEQUATERNION_H
#define QEQUATERNION_H

#include "QEVector4.h"test

template <typename T>
class QEQuaternion : public QEVector4 <T>
{
public:

    static const QEQuaternion<T> Identity;
    
public:

    // x,y,z represent i,j,k basis vectors and w is the real part
    QEQuaternion()
        : Vector4<T>(Identity)
    {
    }

    QEQuaternion(T x,
                T y,
                T z, 
                T w)
                : Vector4<T>(x, y, z, w)
    {
    }

    QEQuaternion(const Vector4<T>& val)
        : Vector4<T>(val)
    {
    }

    QEQuaternion(T real, 
                const Vector3<T>& val)
                : Vector4<T>(val.X(), val.Y(), val.Z(), real)
    {
    }

    QEQuaternion<T> Conjugate() const
    {
        return QEQuaternion<T>(-x_, -y_, -z_, w_);
    }

    QEQuaternion<T> Inverse() const
    {
        return Conjugate() / SquareLength();
    }

    static Matrix4<T> Convert(const QEQuaternion<T>& quat)
    {
        Radian<T> angle;
        Vector3<T> axis;
        Convert(quat,angle,axis);

        Vector3<T> dxAxis(axis.X(), axis.Y(), axis.Z());

        Matrix4<T> rotation;
        rotation.RotateAxisAngle(dxAxis,angle);
        return rotation;
    }

    static void Convert(const QEQuaternion<T>& quat,
                        Radian<T>& angle,
                        Vector3<T>& axis)
    {
        angle = Radian<T>(T(2) * std::acos(quat.W()));

        if(std::abs(angle.AsRadians()) < Math::EPSILON_DBL)
            axis = Vector3<T>::UnitX;
        else
        {
            T wSqr(quat.W() * quat.W());
            axis.X(quat.X() / std::sqrt(T(1) - wSqr));
            axis.Y(quat.Y() / std::sqrt(T(1) - wSqr));
            axis.Z(quat.Z() / std::sqrt(T(1) - wSqr));
        }
    }

    static QEQuaternion<T> Convert(const Radian<T>& angle,
                                    const Vector3<T>& axis)
    {
        if(std::abs(angle.AsRadians()) <= Math::EPSILON_DBL)
            return QEQuaternion<T>::Identity;
        else
        {
            T theta = angle.AsRadians() * T(0.5);
            T sinHalfTheta = std::sin(theta);
            T cosHalfTheta = std::cos(theta);

            return QEQuaternion<T>(cosHalfTheta,sinHalfTheta * axis);
        }
    }

    // Used for Shoemake interpolation
    static QEQuaternion<T> Double(const QEQuaternion<T>& p0,
                                const QEQuaternion<T>& p1)
    {
        return T(2) * p0.Dot(p1) * p1 - p0;
    }

    // Used for Shoemake interpolation
    static QEQuaternion<T> Bisect(const QEQuaternion<T>& p0,
                                const QEQuaternion<T>& p1)
    {
        QEQuaternion<T> ret(p0 + p1);
        return ret.Normalized();
    }

    // Assumes that quaternions are unit length
    static QEQuaternion<T> Slerp(const QEQuaternion<T>& p0,
                                const QEQuaternion<T>& p1,
                                T t)
    {
        if(std::abs(t) <= Math::EPSILON_DBL)
            return p0;

        if(std::abs(T(1) - t) <= Math::EPSILON_DBL)
            return p1;

        T cosTheta = p0.Dot(p1);

        // If angle is tiny, lerp
        if((T(1) - cosTheta) <= Math::EPSILON_DBL)
        {
            return QEQuaternion<T>(Interpolate(p0.X(),p1.X(),t,Interpolation<T>::Linear),
                                    Interpolate(p0.Y(),p1.Y(),t,Interpolation<T>::Linear),
                                    Interpolate(p0.Z(),p1.Z(),t,Interpolation<T>::Linear),
                                    Interpolate(p0.W(),p1.W(),t,Interpolation<T>::Linear));
        }
        T theta = std::acos(cosTheta);
        T a = std::sin((T(1) - t) * theta) / std::sin(theta);
        T b = std::sin(t * theta) / std::sin(theta);

        return a * p0 + b * p1;
    }

    static QEQuaternion<T> QuadSlerp(const QEQuaternion<T>& p0,
                                    const QEQuaternion<T>& p1,
                                    const QEQuaternion<T>& p2,
                                    T t)
    {
        return Slerp(Slerp(p0,p1,t),Slerp(p1,p2,t),t);
    }

    static QEQuaternion<T> CubicSlerp(const QEQuaternion<T>& p0,
                                    const QEQuaternion<T>& p1,
                                    const QEQuaternion<T>& p2,
                                    const QEQuaternion<T>& p3,
                                    T t)
    {
        Math::QEQuatD p1p2(Math::QEQuatD::Slerp(p1,p2,t));
        return Slerp(Slerp(Slerp(p0,p1,t),p1p2,t),Slerp(p1p2,Slerp(p2,p3,t),t),t);
    }

    static void SquadSetup(QEQuaternion<T>& s,
                            const QEQuaternion<T>& q0,
                            const QEQuaternion<T>& q1,
                            const QEQuaternion<T>& q2)
    {
        QEQuaternion<T> a(Log(q1.Inverse() * q2));
        QEQuaternion<T> b(Log(q1.Inverse() * q0));

        s = q1 * Exp(T(-0.25) * (a + b));
    }

    static QEQuaternion<T> Squad(const QEQuaternion<T>& q0,
                                const QEQuaternion<T>& q1,
                                const QEQuaternion<T>& s0,
                                const QEQuaternion<T>& s1,
                                T t)
    {
        return Slerp(Slerp(q0,q1,t),Slerp(s0,s1,t),T(2) * t * (T(1) - t));
    }

    static QEQuaternion<T> Log(const QEQuaternion<T>& val)
    {
        QEQuaternion<T> quat(0,val.Imaginary());
        T theta = std::acos(val.W());
        T sinTheta = std::sin(theta);

        if(std::abs(sinTheta) > Math::EPSILON_DBL)
        {
            theta = theta / sinTheta;
            quat *= theta;
        }
        return quat;
    }

    static QEQuaternion<T> Exp(const QEQuaternion<T>& val)
    {
        T angle = val.Length();
        QEQuaternion<T> quat(0,val.Imaginary());

        if(abs(angle) > Math::EPSILON_DBL)
        {
            quat.W(std::cos(angle));
            angle = sin(angle) / angle;
            quat.Imaginary(quat.Imaginary() * angle);
        }
        return quat;
    }

public:

    Vector3<T> Imaginary() const
    {
        return Vector3<T>(x_,y_,z_);
    }

    void Imaginary(const Vector3<T>& val)
    {
        x_ = val.x_;
        y_ = val.y_;
        z_ = val.z_;
    }

    T Real() const
    {
        return w_;
    }

    void Real(T val)
    {
        w_ = val;
    }

public:

    QEQuaternion<T> operator*(const QEQuaternion<T>& val) const
    {
        return QEQuaternion<T>((w_ * val.x_) + (x_ * val.w_) + (y_ * val.z_) - (z_ * val.y_),
                                (w_ * val.y_) - (x_ * val.z_) + (y_ * val.w_) + (z_ * val.x_),
                                (w_ * val.z_) + (x_ * val.y_) - (y_ * val.x_) + (z_ * val.w_),
                                (w_ * val.w_) - (x_ * val.x_) - (y_ * val.y_) - (z_ * val.z_));
    }

    QEQuaternion<T> operator*(T val) const
    {
        return QEQuaternion<T>(x_ * val,
                                y_ * val,
                                z_ * val,
                                w_ * val);
    }

    QEQuaternion<T> operator/(T val) const
    {
        return QEQuaternion<T>(x_ / val,
                                y_ / val,
                                z_ / val,
                                w_ / val);
    }

    QEQuaternion<T>& operator*=(const QEQuaternion<T>& val)
    {
        *this = *this * val;
        return *this;
    }

    QEQuaternion<T>& operator*=(T val)
    {
        *this = *this * val;
        return *this;
    }

    QEQuaternion<T>& operator/=(T val)
    {
        *this = *this / val;
        return *this;
    }
};

// Float
typedef QEQuaternion<float> QEQuatF;
const QEQuatF QEQuatF::Identity(0,0,0,1);

// Double
typedef QEQuaternion<double> QEQuatD;
const QEQuatD QEQuatD::Identity(0,0,0,1);

#endif