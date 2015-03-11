/********************************************************************
	created:	2015/01/27
	filename: 	Angle.h
	author:		Jonathan Butt
	purpose:	
*********************************************************************/

#ifndef ANGLE_H
#define ANGLE_H

template <typename T>
class QuarkAngle
{
public:
    virtual T AsRadians() const = 0;
    virtual T AsDegrees() const = 0;
    virtual void Simplify() = 0;
};

template <typename T>
class QuarkRadian;

template <typename T>
class QuarkDegree : public QuarkAngle <T>
{
public:
    QuarkDegree()
        : degrees_(0)
    {
    }

    QuarkDegree(T degrees)
        : degrees_(degrees)
    {
    }

    QuarkDegree(const QuarkDegree<T>& val)
        : degrees_(val.degrees_)
    {
    }

    QuarkDegree(const QuarkRadian<T>& val)
        : degrees_(val.AsDegrees())
    {
    }

    T AsRadians() const
    {
        return degrees_* static_cast<T>(0.0174532925);
    }

    T AsDegrees() const
    {
        return degrees_;
    }

    void Simplify()
    {
        degrees_ = degrees_ % T(360);
    }

public:

    QuarkDegree<T> operator+(T degree) const
    {
        return QuarkDegree<T>(degrees_ + degree);
    }

    QuarkDegree<T> operator+(const QuarkDegree<T>& val) const
    {
        return QuarkDegree<T>(degrees_ + val.degrees_());
    }

    QuarkDegree<T> operator+(const QuarkRadian<T>& val) const
    {
        return QuarkDegree<T>(degrees_ + val.AsDegrees());
    }

    QuarkDegree<T> operator-(T degree) const
    {
        return QuarkDegree<T>(degrees_ - degree);
    }

    QuarkDegree<T> operator-(const QuarkDegree<T>& val) const
    {
        return QuarkDegree<T>(degrees_ - val.AsDegrees());
    }

    QuarkDegree<T> operator-(const QuarkRadian<T>& val) const
    {
        return QuarkDegree<T>(degrees_ - val.AsDegrees());
    }

    QuarkDegree<T> operator*(T val) const
    {
        return QuarkDegree<T>(degrees_ * val);
    }

    QuarkDegree<T> operator/(T val) const
    {
        return QuarkDegree<T>(degrees_ / val);
    }

    QuarkDegree<T>& operator+=(T degree)
    {
        degrees_ += degree;
        return *this;
    }

    QuarkDegree<T>& operator+=(const QuarkDegree<T>& val)
    {
        degrees_ += val.degrees_;
        return *this;
    }

    QuarkDegree<T>& operator+=(const QuarkRadian<T>& val)
    {
        degrees_ += val.AsDegrees();
        return *this;
    }

    QuarkDegree<T>& operator-=(T degree)
    {
        degrees_ -= degree;
        return *this;
    }

    QuarkDegree<T>& operator-=(const QuarkDegree<T>& val)
    {
        degrees_ -= val.degrees_;
        return *this;
    }

    QuarkDegree<T>& operator-=(const QuarkRadian<T>& val)
    {
        degrees_ -= val.AsDegrees();
        return *this;
    }

    QuarkDegree<T>& operator*=(T val)
    {
        degrees_ *= val;
        return *this;
    }

    QuarkDegree<T>& operator/=(T val)
    {
        degrees_ /= val;
        return *this;
    }

    QuarkDegree<T>& operator=(T degree)
    {
        degrees_ = degree;
        return *this;
    }

    QuarkDegree<T>& operator=(const QuarkDegree<T>& val)
    {
        degrees_ = val.degrees_;
        return *this;
    }

    QuarkDegree& operator=(const QuarkRadian<T>& val)
    {
        degrees_ = val.AsRadians();
        return *this;
    }

private:

    T degrees_;
};

template <typename T>
class QuarkRadian : public QuarkAngle <T>
{
public:

    QuarkRadian()
        : radians_(0)
    {
    }

    QuarkRadian(T radian)
        : radians_(radian)
    {
    }

    QuarkRadian(const QuarkDegree<T>& val)
        : radians_(val.AsRadians())
    {
    }

    QuarkRadian(const QuarkRadian<T>& val)
        : radians_(val.radians_)
    {
    }

    T AsRadians() const
    {
        return radians_;
    }

    T AsDegrees() const
    {
        return radians_ * static_cast<T>(57.2957795);
    }

public:

    QuarkRadian<T> operator+(T radian) const
    {
        return QuarkRadian<T>(radians_ + radian);
    }

    QuarkRadian<T> operator+(const QuarkDegree<T>& val) const
    {
        return QuarkRadian<T>(radians_ + val.AsRadians());
    }

    QuarkRadian<T> operator+(const QuarkRadian<T>& val) const
    {
        return QuarkRadian<T>(radians_ + val.radians_);
    }

    QuarkRadian<T> operator-(T radian) const
    {
        return QuarkRadian<T>(radians_ - radian);
    }

    QuarkRadian<T> operator-(const QuarkDegree<T>& val) const
    {
        return QuarkRadian<T>(radians_ - val.AsRadians());
    }

    QuarkRadian<T> operator-(const QuarkRadian<T>& val) const
    {
        return QuarkRadian(radians_ - val.AsRadians());
    }

    QuarkRadian<T> operator*(T val) const
    {
        return QuarkRadian<T>(radians_ * val);
    }

    QuarkRadian<T> operator/(T val) const
    {
        return QuarkRadian<T>(radians_ / val);
    }

    QuarkRadian<T>& operator+=(T radian)
    {
        radians_ += radian;
        return *this;
    }

    QuarkRadian<T>& operator+=(const QuarkDegree<T>& val)
    {
        radians_ += val.AsRadians();
        return *this;
    }

    QuarkRadian<T>& operator+=(const QuarkRadian<T>& val)
    {
        radians_ += val.radians_;
        return *this;
    }

    QuarkRadian<T>& operator-=(T radian)
    {
        radians_ -= radian;
        return *this;
    }

    QuarkRadian<T>& operator-=(const QuarkDegree<T>& val)
    {
        radians_ -= val.AsRadians();
        return *this;
    }

    QuarkRadian<T>& operator-=(const QuarkRadian<T>& val)
    {
        radians_ -= val.radians_;
        return *this;
    }

    QuarkRadian<T>& operator*=(T val)
    {
        radians_ *= val;
        return *this;
    }

    QuarkRadian<T>& operator/=(T val)
    {
        radians_ /= val;
        return *this;
    }

    QuarkRadian<T>& operator=(T radian)
    {
        radians_ = radian;
        return *this;
    }

    QuarkRadian<T>& operator=(const QuarkDegree<T>& val)
    {
        radians_ = val.AsRadians();
        return *this;
    }

    QuarkRadian<T>& operator=(const QuarkRadian<T>& val)
    {
        radians_ = val.radians_;
        return *this;
    }

private:

    T radians_;
};

typedef QuarkRadian<float> QuarkRadF;
typedef QuarkRadian<double> QuarkRadD;
typedef QuarkDegree<float> QuarkDegF;
typedef QuarkDegree<double> QuarkDegD;

#endif