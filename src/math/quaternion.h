/********************************************************************
	created:	2015/01/27
	filename: 	quaternion.h
	author:		Jonathan Butt
	purpose:	
*********************************************************************/

#ifndef QUATERNION_H
#define QUATERNION_H

namespace Math
{
    template <typename T>
    class Quaternion : public Vector4 <T>
    {
    public:

        static const Quaternion<T> Identity;
    
    public:
        // x,y,z represent i,j,k basis vectors and w is the real part
        Quaternion()
            : Vector4<T>(Identity)
        {
        }

        Quaternion(T x,
                   T y,
                   T z, 
                   T w)
                   : Vector4<T>(x, y, z, w)
        {
        }

        Quaternion(const Vector4<T>& val)
            : Vector4<T>(val)
        {
        }

        Quaternion(T real, 
                   const Vector3<T>& val)
                   : Vector4<T>(val.X(), val.Y(), val.Z(), real)
        {
        }

        Quaternion<T> Conjugate() const
        {
            return Quaternion<T>(-x_, -y_, -z_, w_);
        }

        Quaternion<T> Inverse() const
        {
            return Conjugate() / SquareLength();
        }

        static Matrix4<T> Convert(const Quaternion<T>& quat)
        {
            Radian<T> angle;
            Vector3<T> axis;
            Convert(quat,angle,axis);

            Vector3<T> dxAxis(axis.X(), axis.Y(), axis.Z());

            Matrix4<T> rotation;
            rotation.RotateAxisAngle(dxAxis,angle);
            return rotation;
        }

        static void Convert(const Quaternion<T>& quat,
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

        static Quaternion<T> Convert(const Radian<T>& angle,
                                     const Vector3<T>& axis)
        {
            if(std::abs(angle.AsRadians()) <= Math::EPSILON_DBL)
                return Quaternion<T>::Identity;
            else
            {
                T theta = angle.AsRadians() * T(0.5);
                T sinHalfTheta = std::sin(theta);
                T cosHalfTheta = std::cos(theta);

                return Quaternion<T>(cosHalfTheta,sinHalfTheta * axis);
            }
        }

        // Used for Shoemake interpolation
        static Quaternion<T> Double(const Quaternion<T>& p0,
                                    const Quaternion<T>& p1)
        {
            return T(2) * p0.Dot(p1) * p1 - p0;
        }

        // Used for Shoemake interpolation
        static Quaternion<T> Bisect(const Quaternion<T>& p0,
                                    const Quaternion<T>& p1)
        {
            Quaternion<T> ret(p0 + p1);
            return ret.Normalized();
        }

        // Assumes that quaternions are unit length
        static Quaternion<T> Slerp(const Quaternion<T>& p0,
                                   const Quaternion<T>& p1,
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
                return Quaternion<T>(Interpolate(p0.X(),p1.X(),t,Interpolation<T>::Linear),
                                     Interpolate(p0.Y(),p1.Y(),t,Interpolation<T>::Linear),
                                     Interpolate(p0.Z(),p1.Z(),t,Interpolation<T>::Linear),
                                     Interpolate(p0.W(),p1.W(),t,Interpolation<T>::Linear));
            }
            T theta = std::acos(cosTheta);
            T a = std::sin((T(1) - t) * theta) / std::sin(theta);
            T b = std::sin(t * theta) / std::sin(theta);

            return a * p0 + b * p1;
        }

        static Quaternion<T> QuadSlerp(const Quaternion<T>& p0,
                                       const Quaternion<T>& p1,
                                       const Quaternion<T>& p2,
                                       T t)
        {
            return Slerp(Slerp(p0,p1,t),Slerp(p1,p2,t),t);
        }

        static Quaternion<T> CubicSlerp(const Quaternion<T>& p0,
                                        const Quaternion<T>& p1,
                                        const Quaternion<T>& p2,
                                        const Quaternion<T>& p3,
                                        T t)
        {
            Math::QuatD p1p2(Math::QuatD::Slerp(p1,p2,t));
            return Slerp(Slerp(Slerp(p0,p1,t),p1p2,t),Slerp(p1p2,Slerp(p2,p3,t),t),t);
        }

        static void SquadSetup(Quaternion<T>& s,
                               const Quaternion<T>& q0,
                               const Quaternion<T>& q1,
                               const Quaternion<T>& q2)
        {
            Quaternion<T> a(Log(q1.Inverse() * q2));
            Quaternion<T> b(Log(q1.Inverse() * q0));

            s = q1 * Exp(T(-0.25) * (a + b));
        }

        static Quaternion<T> Squad(const Quaternion<T>& q0,
                                   const Quaternion<T>& q1,
                                   const Quaternion<T>& s0,
                                   const Quaternion<T>& s1,
                                   T t)
        {
            return Slerp(Slerp(q0,q1,t),Slerp(s0,s1,t),T(2) * t * (T(1) - t));
        }

        static Quaternion<T> Log(const Quaternion<T>& val)
        {
            Quaternion<T> quat(0,val.Imaginary());
            T theta = std::acos(val.W());
            T sinTheta = std::sin(theta);

            if(std::abs(sinTheta) > Math::EPSILON_DBL)
            {
                theta = theta / sinTheta;
                quat *= theta;
            }
            return quat;
        }

        static Quaternion<T> Exp(const Quaternion<T>& val)
        {
            T angle = val.Length();
            Quaternion<T> quat(0,val.Imaginary());

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

        Quaternion<T> operator*(const Quaternion<T>& val) const
        {
            return Quaternion<T>((w_ * val.x_) + (x_ * val.w_) + (y_ * val.z_) - (z_ * val.y_),
                                 (w_ * val.y_) - (x_ * val.z_) + (y_ * val.w_) + (z_ * val.x_),
                                 (w_ * val.z_) + (x_ * val.y_) - (y_ * val.x_) + (z_ * val.w_),
                                 (w_ * val.w_) - (x_ * val.x_) - (y_ * val.y_) - (z_ * val.z_));
        }

        Quaternion<T> operator*(T val) const
        {
            return Quaternion<T>(x_ * val,
                                 y_ * val,
                                 z_ * val,
                                 w_ * val);
        }

        Quaternion<T> operator/(T val) const
        {
            return Quaternion<T>(x_ / val,
                                 y_ / val,
                                 z_ / val,
                                 w_ / val);
        }

        Quaternion<T>& operator*=(const Quaternion<T>& val)
        {
            *this = *this * val;
            return *this;
        }

        Quaternion<T>& operator*=(T val)
        {
            *this = *this * val;
            return *this;
        }

        Quaternion<T>& operator/=(T val)
        {
            *this = *this / val;
            return *this;
        }
    };

    // Float
    typedef Quaternion<float> QuatF;
    const QuatF QuatF::Identity(0,0,0,1);

    // Double
    typedef Quaternion<double> QuatD;
    const QuatD QuatD::Identity(0,0,0,1);
}

#endif