/********************************************************************
	created:	2015/01/27
	filename: 	vector3.h
	author:		Jonathan Butt
	purpose:	
*********************************************************************/

#ifndef VECTOR3_H
#define VECTOR3_H

namespace Math
{
    template <typename T>
    class Vector3
    {
    public:

        static const Vector3<T> Zero;
        static const Vector3<T> UnitX;
        static const Vector3<T> UnitY;
        static const Vector3<T> UnitZ;
    
    public:
        Vector3()
            :   x_(0),
                y_(0),
                z_(0)
        {
        }

        Vector3(T x,
                T y,
                T z)
                :   x_(x),
                    y_(y),
                    z_(z)
        {
        }

        Vector3(const Vector3<T>& val)
            :   x_(val.x_),
                y_(val.y_),
                z_(val.z_)
        {
        }

        Vector3<T> ProjectOnto(const Vector3<T>& val) const
        {
            return (Dot(val) / val.SquareLength()) * val;
        }

        Vector3<T> ReflectAbout(const Vector3<T>& val) const
        {
            return *this - T(2) * ProjectOnto(val.Normalized());
        }

        T Dot(const Vector3<T>& val) const
        {
            return (x_ * val.x_) + (y_ * val.y_) + (z_ * val.z_);
        }

        Vector3<T> Cross(const Vector3<T>& val) const
        {
            return Vector3<T>((y_ * val.z_) - (z_ * val.y_),
                              (z_ * val.x_) - (x_ * val.z_),
                              (x_ * val.y_) - (y_ * val.x_));
        }

        void Normalize()
        {
            *this /= Length();
        }

        Vector3<T> Normalized() const
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
        Vector3<T>& operator=(const Vector3<T>& val)
        {
            x_ = val.x_;
            y_ = val.y_;
            z_ = val.z_;
            return *this;
        }

        Vector3<T> operator+(const Vector3<T>& val) const
        {
            Vector3<T> ret(x_ + val.x_, y_ + val.y_, z_ + val.z_);
            return ret;
        }

        Vector3<T> operator-(const Vector3<T>& val) const
        {
            Vector3<T> ret(x_ - val.x_, y_ - val.y_, z_ - val.z_);
            return ret;
        }

        Vector3<T> operator-() const
        {
            Vector3<T> ret(-x_,-y_,-z_);
            return ret;
        }

        Vector3<T> operator*(T val) const
        {
            Vector3<T> ret(x_ * val, y_ * val, z_ * val);
            return ret;
        }

        Vector3<T> operator/(T val) const
        {
            Vector3<T> ret(x_ / val, y_ / val, z_ / val);
            return ret;
        }

        Vector3<T> operator+=(const Vector3<T>& val)
        {
            x_ += val.x_;
            y_ += val.y_;
            z_ += val.z_;
            return *this;
        }

        Vector3<T> operator-=(const Vector3<T>& val)
        {
            x_ -= val.x_;
            y_ -= val.y_;
            z_ -= val.z_;
            return *this;
        }

        Vector3<T> operator*=(T val)
        {
            x_ *= val;
            y_ *= val;
            z_ *= val;
            return *this;
        }

        Vector3<T> operator/=(T val)
        {
            x_ /= val;
            y_ /= val;
            z_ /= val;
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

        T* Vector() const
        {
            return v_;
        }

    private:
        union
        {
            struct
            {
                T x_;
                T y_;
                T z_;
            };
            T v_[3];
        };
    };

    // Int
    typedef Vector3<int> Vec3I;
    const Vec3I Vec3I::Zero(0,0,0);
    const Vec3I Vec3I::UnitX(1,0,0);
    const Vec3I Vec3I::UnitY(0,1,0);
    const Vec3I Vec3I::UnitZ(0,0,1);

    // Float
    typedef Vector3<float> Vec3F;
    const Vec3F Vec3F::Zero(0,0,0);
    const Vec3F Vec3F::UnitX(1,0,0);
    const Vec3F Vec3F::UnitY(0,1,0);
    const Vec3F Vec3F::UnitZ(0,0,1);

    // Double
    typedef Vector3<double> Vec3D;
    const Vec3D Vec3D::Zero(0,0,0);
    const Vec3D Vec3D::UnitX(1,0,0);
    const Vec3D Vec3D::UnitY(0,1,0);
    const Vec3D Vec3D::UnitZ(0,0,1);
}

template <typename T>
Math::Vector3<T> operator*(T a,
                           const Math::Vector3<T>& b)
{
    return Math::Vector3<T>(b.X() * a,
                            b.Y() * a,
                            b.Z() * a);
}

#endif