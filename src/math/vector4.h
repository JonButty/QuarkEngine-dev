/********************************************************************
	created:	2015/01/27
	filename: 	vector4.h
	author:		Jonathan Butt
	purpose:	
*********************************************************************/

#ifndef VECTOR4_H
#define VECTOR4_H

template <typename T>
class QuarkVector4
{
public:
    
    static const QuarkVector4<T> Zero;
    static const QuarkVector4<T> UnitX;
    static const QuarkVector4<T> UnitY;
    static const QuarkVector4<T> UnitZ;
    static const QuarkVector4<T> UnitW;
    
public:

    QuarkVector4()
        :   x_(0),
            y_(0),
            z_(0),
            w_(0)
    {
    }

    QuarkVector4(T x,
            T y,
            T z,
            T w)
            :   x_(x),
                y_(y),
                z_(z),
                w_(w)
    {
    }

    QuarkVector4(const QuarkVector4<T>& val) 
        :   x_(val.x_),
            y_(val.y_),
            z_(val.z_),
            w_(val.w_)
    {
    }

    QuarkVector4<T> ProjectOnto(const QuarkVector4<T>& val) const
    {
        return (Dot(val) / val.SquareLength()) * val;
    }

    QuarkVector4<T> ReflectAbout(const QuarkVector4<T>& val) const
    {
        return *this - 2 * ProjectOnto(val.Normalized());
    }

    T Dot(const QuarkVector4<T>& val) const
    {
        return (x_ * val.x_) + (y_ * val.y_) + (z_ * val.z_) + (w_ * val.w_);
    }

    void Normalize()
    {
        *this /= Length();
    }

    QuarkVector4<T> Normalized() const
    {
        return *this / Length();
    }

    T SquareLength() const
    {
        return Dot(*this);
    }

    T Length() const
    {
        return std::sqrt(SquareLength());
    }

public:

    QuarkVector4<T>& operator=(const QuarkVector4<T>& val)
    {
        x_ = val.x_;
        y_ = val.y_;
        z_ = val.z_;
        w_ = val.w_;
        return *this;
    }

    QuarkVector4<T> operator+(const QuarkVector4<T>& val) const
    {
        return QuarkVector4(x_ + val.x_, y_ + val.y_, z_ + val.z_, w_ + val.w_);
    }

    QuarkVector4<T> operator-(const QuarkVector4<T>& val) const
    {
        return QuarkVector4(x_ - val.x_, y_ - val.y_, z_ - val.z_, w_ - val.w_);
    }

    QuarkVector4<T> operator-() const
    {
        return QuarkVector4(-x_, -y_, -z_, -w_);
    }

    QuarkVector4<T> operator*(T val) const
    {
        return QuarkVector4(x_ * val, y_ * val, z_ * val, w_ * val);
    }

    QuarkVector4<T> operator/(T val) const
    {
        return QuarkVector4(x_ / val, y_ / val, z_ / val, w_ / val);
    }

    QuarkVector4<T>& operator+=(const QuarkVector4<T>& val)
    {
        x_ += val.x_;
        y_ += val.y_;
        z_ += val.z_;
        w_ += val.w_;
        return *this;
    }

    QuarkVector4<T>& operator-=(const QuarkVector4<T>& val)
    {
        x_ -= val.x_;
        y_ -= val.y_;
        z_ -= val.z_;
        w_ -= val.w_;
        return *this;
    }

    QuarkVector4<T>& operator*=(T val)
    {
        x_ *= val;
        y_ *= val;
        z_ *= val;
        w_ *= val;
        return *this;
    }

    QuarkVector4<T>& operator/=(T val)
    {
        x_ /= val;
        y_ /= val;
        z_ /= val;
        w_ /= val;
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

    void W(T val)
    {
        w_ = val;
    }

    T W() const
    {
        return w_;
    }

    T* Vector() const
    {
        return v_;
    }

protected:
    union
    {
        struct
        {
            T x_;
            T y_;
            T z_;
            T w_;
        };
        T v_[4];
    };
};

// Int
typedef QuarkVector4<int> QuarkVec4I;
const QuarkVec4I QuarkVec4I::Zero(0,0,0,0);
const QuarkVec4I QuarkVec4I::UnitX(1,0,0,0);
const QuarkVec4I QuarkVec4I::UnitY(0,1,0,0);
const QuarkVec4I QuarkVec4I::UnitZ(0,0,1,0);
const QuarkVec4I QuarkVec4I::UnitW(0,0,0,1);

// Float
typedef QuarkVector4<float> QuarkVec4F;
const QuarkVec4F QuarkVec4F::Zero(0,0,0,0);
const QuarkVec4F QuarkVec4F::UnitX(1,0,0,0);
const QuarkVec4F QuarkVec4F::UnitY(0,1,0,0);
const QuarkVec4F QuarkVec4F::UnitZ(0,0,1,0);
const QuarkVec4F QuarkVec4F::UnitW(0,0,0,1);

// Double
typedef QuarkVector4<double> QuarkVec4D;
const QuarkVec4D QuarkVec4D::Zero(0,0,0,0);
const QuarkVec4D QuarkVec4D::UnitX(1,0,0,0);
const QuarkVec4D QuarkVec4D::UnitY(0,1,0,0);
const QuarkVec4D QuarkVec4D::UnitZ(0,0,1,0);
const QuarkVec4D QuarkVec4D::UnitW(0,0,0,1);

template <typename T>
QuarkVector4<T> operator*(T a,
                           const QuarkVector4<T>& b)
{
    return b * a;
}

#endif