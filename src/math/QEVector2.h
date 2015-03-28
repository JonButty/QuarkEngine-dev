/*!*****************************************************************************

\class  QEVector
\date 	2015/03/28
\author	Jonathan Butt
\brief	2D Vector class

*******************************************************************************/
#ifndef QEVECTOR2_H
#define QEVECTOR2_H

#include "common/QEDefines.h"

template <typename T>
class QEVector2
{
public:

    static const QEVector2<T> Zero;
    static const QEVector2<T> UnitX;
    static const QEVector2<T> UnitY;
    
public:

    QE_API QEVector2 ()
            :   x_(0),
                y_(0)
    {
    }

    QE_API QEVector2 (T x,
                      T y)
            :   x_ (x),
                y_ (y)
    {
    }

    QE_API QEVector2 (QE_IN const QEVector2<T>& val)
            :   x_ (val.x_),
                y_ (val.y_)
    {
    }

    QE_API QEVector2<T> ProjectOnto (QE_IN const QEVector2<T>& val) const
    {
        return (Dot (val) / val.SquareLength () ) * val;
    }

    QE_API QEVector2<T> ReflectAbout (QE_IN const QEVector2<T>& val) const
    {
        return *this - T(2) * ProjectOnto(val.Normalized());
    }

    QE_API T Dot (QE_IN const QEVector2<T>& val) const
    {
        return (x_ * val.x_) + (y_ * val.y_);
    }

    QE_API void Normalize ()
    {
        *this /= Length ();
    }

    QE_API QEVector2<T> Normalized () const
    {
        return *this / Length ();
    }

    QE_API T SquareLength () const
    {
        return Dot (*this);
    }

    QE_API T Length () const
    {
        return static_cast<T>(std::sqrt (SquareLength ()));
    }

public:

    QE_API QEVector2<T>& operator= (QE_IN const QEVector2<T>& val)
    {
        x_ = val.x_;
        y_ = val.y_;
        return *this;
    }

    QE_API QEVector2<T> operator+ (QE_IN const QEVector2<T>& val) const
    {
        QEVector2<T> ret (x_ + val.x_,y_ + val.y_);
        return ret;
    }

    QE_API QEVector2<T> operator- (QE_IN const QEVector2<T>& val) const
    {
        QEVector2<T> ret (x_ - val.x_,y_ - val.y_);
        return ret;
    }

    QE_API QEVector2<T> operator* (QE_IN T val) const
    {
        QEVector2<T> ret(x_ * val,y_ * val);
        return ret;
    }

    QE_API QEVector2<T> operator/ (QE_IN T val) const
    {
        QEVector2<T> ret (x_ / val,y_ / val);
        return ret;
    }

    QE_API QEVector2<T>& operator+= (QE_IN const QEVector2<T>& val)
    {
        x_ += val.x_;
        y_ += val.y_;
        return *this;
    }

    QE_API QEVector2<T>& operator-= (QE_IN const QEVector2<T>& val)
    {
        x_ -= val.x_;
        y_ -= val.y_;
        return *this;
    }

    QE_API QEVector2<T>& operator*= (QE_IN T val)
    {
        x_ *= val;
        y_ *= val;
        return *this;
    }

    QE_API QEVector2<T>& operator/= (QE_IN T val)
    {
        x_ /= val;
        y_ /= val;
        return *this;
    }

    QE_API const T& operator[] (QE_IN unsigned int index) const
    {
        return v_[index];
    }

    QE_API T& operator[] (QE_IN unsigned int index)
    {
        return v_[index];
    }

public:

    QE_API void X (QE_IN T val)
    {
        x_ = val;
    }

    QE_API T X () const
    {
        return x_;
    }

    QE_API void Y (QE_IN T val)
    {
        y_ = val;
    }

    QE_API T Y () const
    {
        return y_;
    }

    QE_API T* Vector () const
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
        };
        T v_[2];
    };
};

// Int
typedef QEVector2<int> QEVec2I;
const QEVec2I QEVec2I::Zero(0,0);
const QEVec2I QEVec2I::UnitX(1,0);
const QEVec2I QEVec2I::UnitY(0,1);

// Float
typedef QEVector2<float> QEVec2F;
const QEVec2F QEVec2F::Zero(0,0);
const QEVec2F QEVec2F::UnitX(1,0);
const QEVec2F QEVec2F::UnitY(0,1);

// Double
typedef QEVector2<double> QEVec2D;
const QEVec2D QEVec2D::Zero(0,0);
const QEVec2D QEVec2D::UnitX(1,0);
const QEVec2D QEVec2D::UnitY(0,1);

template <typename T>
QE_API QEVector2<T> operator* (QE_IN T a,
                               QE_IN const QEVector2<T>& b)
{
    return QEVector2<T> ( b.X() * a, b.Y() * a);
}

#endif