/********************************************************************
	created:	2015/01/27
	filename: 	vector2.h
	author:		Jonathan Butt
	purpose:	
*********************************************************************/

#ifndef VECTOR2_H
#define VECTOR2_H

template <typename T>
class QuarkVector2
{
public:

    static const QuarkVector2<T> Zero;
    static const QuarkVector2<T> UnitX;
    static const QuarkVector2<T> UnitY;
    
public:
    QuarkVector2()
        :   x_(0),
            y_(0)
    {
    }

    QuarkVector2(T x,
            T y)
            :   x_(x),
                y_(y)
    {
    }

    QuarkVector2(const QuarkVector2<T>& val)
        :   x_(val.x_),
            y_(val.y_)
    {
    }

    QuarkVector2<T> ProjectOnto(const QuarkVector2<T>& val) const
    {
        return (Dot(val) / val.SquareLength()) * val;
    }

    QuarkVector2<T> ReflectAbout(const QuarkVector2<T>& val) const
    {
        return *this - T(2) * ProjectOnto(val.Normalized());
    }

    T Dot(const QuarkVector2<T>& val) const
    {
        return (x_ * val.x_) + (y_ * val.y_);
    }

    void Normalize()
    {
        *this /= Length();
    }

    QuarkVector2<T> Normalized() const
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
    QuarkVector2<T>& operator=(const QuarkVector2<T>& val)
    {
        x_ = val.x_;
        y_ = val.y_;
        return *this;
    }

    QuarkVector2<T> operator+(const QuarkVector2<T>& val) const
    {
        QuarkVector2<T> ret(x_ + val.x_,y_ + val.y_);
        return ret;
    }

    QuarkVector2<T> operator-(const QuarkVector2<T>& val) const
    {
        QuarkVector2<T> ret(x_ - val.x_,y_ - val.y_);
        return ret;
    }

    QuarkVector2<T> operator*(T val) const
    {
        QuarkVector2<T> ret(x_ * val,y_ * val);
        return ret;
    }

    QuarkVector2<T> operator/(T val) const
    {
        QuarkVector2<T> ret(x_ / val,y_ / val);
        return ret;
    }

    QuarkVector2<T>& operator+=(const QuarkVector2<T>& val)
    {
        x_ += val.x_;
        y_ += val.y_;
        return *this;
    }

    QuarkVector2<T>& operator-=(const QuarkVector2<T>& val)
    {
        x_ -= val.x_;
        y_ -= val.y_;
        return *this;
    }

    QuarkVector2<T>& operator*=(T val)
    {
        x_ *= val;
        y_ *= val;
        return *this;
    }

    QuarkVector2<T>& operator/=(T val)
    {
        x_ /= val;
        y_ /= val;
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
        };
        T v_[2];
    };
};

// Int
typedef QuarkVector2<int> QuarkVec2I;
const QuarkVec2I QuarkVec2I::Zero(0,0);
const QuarkVec2I QuarkVec2I::UnitX(1,0);
const QuarkVec2I QuarkVec2I::UnitY(0,1);

// Float
typedef QuarkVector2<float> QuarkVec2F;
const QuarkVec2F QuarkVec2F::Zero(0,0);
const QuarkVec2F QuarkVec2F::UnitX(1,0);
const QuarkVec2F QuarkVec2F::UnitY(0,1);

// Double
typedef QuarkVector2<double> QuarkVec2D;
const QuarkVec2D QuarkVec2D::Zero(0,0);
const QuarkVec2D QuarkVec2D::UnitX(1,0);
const QuarkVec2D QuarkVec2D::UnitY(0,1);

template <typename T>
QuarkVector2<T> operator*(T a,
                          const QuarkVector2<T>& b)
{
    return QuarkVector2<T>(b.X() * a,
                            b.Y() * a);
}

#endif