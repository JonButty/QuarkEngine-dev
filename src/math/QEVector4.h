/*!*****************************************************************************

\class 	QEVector4
\date 	2015/03/28
\author	Jonathan Butt
\brief	

*******************************************************************************/
#ifndef QEVECTOR4_H
#define QEVECTOR4_H

template <typename T>
class QEVector4
{
public:
    
    static const QEVector4<T> Zero;
    static const QEVector4<T> UnitX;
    static const QEVector4<T> UnitY;
    static const QEVector4<T> UnitZ;
    static const QEVector4<T> UnitW;
    
public:

    QEVector4()
        :   x_(0),
            y_(0),
            z_(0),
            w_(0)
    {
    }

    QEVector4(T x,
            T y,
            T z,
            T w)
            :   x_(x),
                y_(y),
                z_(z),
                w_(w)
    {
    }

    QEVector4(const QEVector4<T>& val) 
        :   x_(val.x_),
            y_(val.y_),
            z_(val.z_),
            w_(val.w_)
    {
    }

    QEVector4<T> ProjectOnto(const QEVector4<T>& val) const
    {
        return (Dot(val) / val.SquareLength()) * val;
    }

    QEVector4<T> ReflectAbout(const QEVector4<T>& val) const
    {
        return *this - 2 * ProjectOnto(val.Normalized());
    }

    T Dot(const QEVector4<T>& val) const
    {
        return (x_ * val.x_) + (y_ * val.y_) + (z_ * val.z_) + (w_ * val.w_);
    }

    void Normalize()
    {
        *this /= Length();
    }

    QEVector4<T> Normalized() const
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

    QEVector4<T>& operator=(const QEVector4<T>& val)
    {
        x_ = val.x_;
        y_ = val.y_;
        z_ = val.z_;
        w_ = val.w_;
        return *this;
    }

    QEVector4<T> operator+(const QEVector4<T>& val) const
    {
        return QEVector4(x_ + val.x_, y_ + val.y_, z_ + val.z_, w_ + val.w_);
    }

    QEVector4<T> operator-(const QEVector4<T>& val) const
    {
        return QEVector4(x_ - val.x_, y_ - val.y_, z_ - val.z_, w_ - val.w_);
    }

    QEVector4<T> operator-() const
    {
        return QEVector4(-x_, -y_, -z_, -w_);
    }

    QEVector4<T> operator*(T val) const
    {
        return QEVector4(x_ * val, y_ * val, z_ * val, w_ * val);
    }

    QEVector4<T> operator/(T val) const
    {
        return QEVector4(x_ / val, y_ / val, z_ / val, w_ / val);
    }

    QEVector4<T>& operator+=(const QEVector4<T>& val)
    {
        x_ += val.x_;
        y_ += val.y_;
        z_ += val.z_;
        w_ += val.w_;
        return *this;
    }

    QEVector4<T>& operator-=(const QEVector4<T>& val)
    {
        x_ -= val.x_;
        y_ -= val.y_;
        z_ -= val.z_;
        w_ -= val.w_;
        return *this;
    }

    QEVector4<T>& operator*=(T val)
    {
        x_ *= val;
        y_ *= val;
        z_ *= val;
        w_ *= val;
        return *this;
    }

    QEVector4<T>& operator/=(T val)
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
typedef QEVector4<int> QEVec4I;
const QEVec4I QEVec4I::Zero(0,0,0,0);
const QEVec4I QEVec4I::UnitX(1,0,0,0);
const QEVec4I QEVec4I::UnitY(0,1,0,0);
const QEVec4I QEVec4I::UnitZ(0,0,1,0);
const QEVec4I QEVec4I::UnitW(0,0,0,1);

// Float
typedef QEVector4<float> QEVec4F;
const QEVec4F QEVec4F::Zero(0,0,0,0);
const QEVec4F QEVec4F::UnitX(1,0,0,0);
const QEVec4F QEVec4F::UnitY(0,1,0,0);
const QEVec4F QEVec4F::UnitZ(0,0,1,0);
const QEVec4F QEVec4F::UnitW(0,0,0,1);

// Double
typedef QEVector4<double> QEVec4D;
const QEVec4D QEVec4D::Zero(0,0,0,0);
const QEVec4D QEVec4D::UnitX(1,0,0,0);
const QEVec4D QEVec4D::UnitY(0,1,0,0);
const QEVec4D QEVec4D::UnitZ(0,0,1,0);
const QEVec4D QEVec4D::UnitW(0,0,0,1);

template <typename T>
QEVector4<T> operator*(T a,
                       const QEVector4<T>& b)
{
    return b * a;
}

#endif