/********************************************************************
	created:	2015/01/27
	filename: 	QEVector3.h
	author:		Jonathan Butt
	purpose:	
*********************************************************************/

#ifndef QEVECTOR3_H
#define QEVECTOR3_H

template <typename T>
class QEVector3
{
public:

    static const QEVector3<T> Zero;
    static const QEVector3<T> UnitX;
    static const QEVector3<T> UnitY;
    static const QEVector3<T> UnitZ;
    
public:
    QEVector3()
        :   x_(0),
            y_(0),
            z_(0)
    {
    }

    QEVector3(T x,
            T y,
            T z)
            :   x_(x),
                y_(y),
                z_(z)
    {
    }

    QEVector3(const QEVector3<T>& val)
        :   x_(val.x_),
            y_(val.y_),
            z_(val.z_)
    {
    }

    QEVector3<T> ProjectOnto(const QEVector3<T>& val) const
    {
        return (Dot(val) / val.SquareLength()) * val;
    }

    QEVector3<T> ReflectAbout(const QEVector3<T>& val) const
    {
        return *this - T(2) * ProjectOnto(val.Normalized());
    }

    T Dot(const QEVector3<T>& val) const
    {
        return (x_ * val.x_) + (y_ * val.y_) + (z_ * val.z_);
    }

    QEVector3<T> Cross(const QEVector3<T>& val) const
    {
        return QEVector3<T>((y_ * val.z_) - (z_ * val.y_),
                            (z_ * val.x_) - (x_ * val.z_),
                            (x_ * val.y_) - (y_ * val.x_));
    }

    void Normalize()
    {
        *this /= Length();
    }

    QEVector3<T> Normalized() const
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
    QEVector3<T>& operator=(const QEVector3<T>& val)
    {
        x_ = val.x_;
        y_ = val.y_;
        z_ = val.z_;
        return *this;
    }

    QEVector3<T> operator+(const QEVector3<T>& val) const
    {
        QEVector3<T> ret(x_ + val.x_, y_ + val.y_, z_ + val.z_);
        return ret;
    }

    QEVector3<T> operator-(const QEVector3<T>& val) const
    {
        QEVector3<T> ret(x_ - val.x_, y_ - val.y_, z_ - val.z_);
        return ret;
    }

    QEVector3<T> operator-() const
    {
        QEVector3<T> ret(-x_,-y_,-z_);
        return ret;
    }

    QEVector3<T> operator*(T val) const
    {
        QEVector3<T> ret(x_ * val, y_ * val, z_ * val);
        return ret;
    }

    QEVector3<T> operator/(T val) const
    {
        QEVector3<T> ret(x_ / val, y_ / val, z_ / val);
        return ret;
    }

    QEVector3<T> operator+=(const QEVector3<T>& val)
    {
        x_ += val.x_;
        y_ += val.y_;
        z_ += val.z_;
        return *this;
    }

    QEVector3<T> operator-=(const QEVector3<T>& val)
    {
        x_ -= val.x_;
        y_ -= val.y_;
        z_ -= val.z_;
        return *this;
    }

    QEVector3<T> operator*=(T val)
    {
        x_ *= val;
        y_ *= val;
        z_ *= val;
        return *this;
    }

    QEVector3<T> operator/=(T val)
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
typedef QEVector3<int> QEVec3I;
const QEVec3I QEVec3I::Zero(0,0,0);
const QEVec3I QEVec3I::UnitX(1,0,0);
const QEVec3I QEVec3I::UnitY(0,1,0);
const QEVec3I QEVec3I::UnitZ(0,0,1);

// Float
typedef QEVector3<float> QEVec3F;
const QEVec3F QEVec3F::Zero(0,0,0);
const QEVec3F QEVec3F::UnitX(1,0,0);
const QEVec3F QEVec3F::UnitY(0,1,0);
const QEVec3F QEVec3F::UnitZ(0,0,1);

// Double
typedef QEVector3<double> QEVec3D;
const QEVec3D QEVec3D::Zero(0,0,0);
const QEVec3D QEVec3D::UnitX(1,0,0);
const QEVec3D QEVec3D::UnitY(0,1,0);
const QEVec3D QEVec3D::UnitZ(0,0,1);

template <typename T>
QEVector3<T> operator*(T a,
                          const QEVector3<T>& b)
{
    return QEVector3<T>(b.X() * a,
                           b.Y() * a,
                           b.Z() * a);
}

#endif