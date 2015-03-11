/********************************************************************
	created:	2015/01/27
	filename: 	vector3.h
	author:		Jonathan Butt
	purpose:	
*********************************************************************/

#ifndef VECTOR3_H
#define VECTOR3_H

template <typename T>
class QuarkVector3
{
public:

    static const QuarkVector3<T> Zero;
    static const QuarkVector3<T> UnitX;
    static const QuarkVector3<T> UnitY;
    static const QuarkVector3<T> UnitZ;
    
public:
    QuarkVector3()
        :   x_(0),
            y_(0),
            z_(0)
    {
    }

    QuarkVector3(T x,
            T y,
            T z)
            :   x_(x),
                y_(y),
                z_(z)
    {
    }

    QuarkVector3(const QuarkVector3<T>& val)
        :   x_(val.x_),
            y_(val.y_),
            z_(val.z_)
    {
    }

    QuarkVector3<T> ProjectOnto(const QuarkVector3<T>& val) const
    {
        return (Dot(val) / val.SquareLength()) * val;
    }

    QuarkVector3<T> ReflectAbout(const QuarkVector3<T>& val) const
    {
        return *this - T(2) * ProjectOnto(val.Normalized());
    }

    T Dot(const QuarkVector3<T>& val) const
    {
        return (x_ * val.x_) + (y_ * val.y_) + (z_ * val.z_);
    }

    QuarkVector3<T> Cross(const QuarkVector3<T>& val) const
    {
        return QuarkVector3<T>((y_ * val.z_) - (z_ * val.y_),
                            (z_ * val.x_) - (x_ * val.z_),
                            (x_ * val.y_) - (y_ * val.x_));
    }

    void Normalize()
    {
        *this /= Length();
    }

    QuarkVector3<T> Normalized() const
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
    QuarkVector3<T>& operator=(const QuarkVector3<T>& val)
    {
        x_ = val.x_;
        y_ = val.y_;
        z_ = val.z_;
        return *this;
    }

    QuarkVector3<T> operator+(const QuarkVector3<T>& val) const
    {
        QuarkVector3<T> ret(x_ + val.x_, y_ + val.y_, z_ + val.z_);
        return ret;
    }

    QuarkVector3<T> operator-(const QuarkVector3<T>& val) const
    {
        QuarkVector3<T> ret(x_ - val.x_, y_ - val.y_, z_ - val.z_);
        return ret;
    }

    QuarkVector3<T> operator-() const
    {
        QuarkVector3<T> ret(-x_,-y_,-z_);
        return ret;
    }

    QuarkVector3<T> operator*(T val) const
    {
        QuarkVector3<T> ret(x_ * val, y_ * val, z_ * val);
        return ret;
    }

    QuarkVector3<T> operator/(T val) const
    {
        QuarkVector3<T> ret(x_ / val, y_ / val, z_ / val);
        return ret;
    }

    QuarkVector3<T> operator+=(const QuarkVector3<T>& val)
    {
        x_ += val.x_;
        y_ += val.y_;
        z_ += val.z_;
        return *this;
    }

    QuarkVector3<T> operator-=(const QuarkVector3<T>& val)
    {
        x_ -= val.x_;
        y_ -= val.y_;
        z_ -= val.z_;
        return *this;
    }

    QuarkVector3<T> operator*=(T val)
    {
        x_ *= val;
        y_ *= val;
        z_ *= val;
        return *this;
    }

    QuarkVector3<T> operator/=(T val)
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
typedef QuarkVector3<int> QuarkVec3I;
const QuarkVec3I QuarkVec3I::Zero(0,0,0);
const QuarkVec3I QuarkVec3I::UnitX(1,0,0);
const QuarkVec3I QuarkVec3I::UnitY(0,1,0);
const QuarkVec3I QuarkVec3I::UnitZ(0,0,1);

// Float
typedef QuarkVector3<float> QuarkVec3F;
const QuarkVec3F QuarkVec3F::Zero(0,0,0);
const QuarkVec3F QuarkVec3F::UnitX(1,0,0);
const QuarkVec3F QuarkVec3F::UnitY(0,1,0);
const QuarkVec3F QuarkVec3F::UnitZ(0,0,1);

// Double
typedef QuarkVector3<double> QuarkVec3D;
const QuarkVec3D QuarkVec3D::Zero(0,0,0);
const QuarkVec3D QuarkVec3D::UnitX(1,0,0);
const QuarkVec3D QuarkVec3D::UnitY(0,1,0);
const QuarkVec3D QuarkVec3D::UnitZ(0,0,1);

template <typename T>
QuarkVector3<T> operator*(T a,
                          const QuarkVector3<T>& b)
{
    return QuarkVector3<T>(b.X() * a,
                           b.Y() * a,
                           b.Z() * a);
}

#endif