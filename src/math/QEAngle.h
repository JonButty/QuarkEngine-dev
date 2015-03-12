/********************************************************************
	created:	2015/01/27
	filename: 	QEAngle.h
	author:		Jonathan Butt
	purpose:	
*********************************************************************/

#ifndef QEANGLE_H
#define QEANGLE_H

template <typename T>
class QEAngle
{
public:
    virtual T AsRadians() const = 0;
    virtual T AsDegrees() const = 0;
    virtual void Simplify() = 0;
};

template <typename T>
class QERadian;

template <typename T>
class QEDegree : public QEAngle <T>
{
public:
    QEDegree()
        : degrees_(0)
    {
    }

    QEDegree(T degrees)
        : degrees_(degrees)
    {
    }

    QEDegree(const QEDegree<T>& val)
        : degrees_(val.degrees_)
    {
    }

    QEDegree(const QERadian<T>& val)
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

    QEDegree<T> operator+(T degree) const
    {
        return QEDegree<T>(degrees_ + degree);
    }

    QEDegree<T> operator+(const QEDegree<T>& val) const
    {
        return QEDegree<T>(degrees_ + val.degrees_());
    }

    QEDegree<T> operator+(const QERadian<T>& val) const
    {
        return QEDegree<T>(degrees_ + val.AsDegrees());
    }

    QEDegree<T> operator-(T degree) const
    {
        return QEDegree<T>(degrees_ - degree);
    }

    QEDegree<T> operator-(const QEDegree<T>& val) const
    {
        return QEDegree<T>(degrees_ - val.AsDegrees());
    }

    QEDegree<T> operator-(const QERadian<T>& val) const
    {
        return QEDegree<T>(degrees_ - val.AsDegrees());
    }

    QEDegree<T> operator*(T val) const
    {
        return QEDegree<T>(degrees_ * val);
    }

    QEDegree<T> operator/(T val) const
    {
        return QEDegree<T>(degrees_ / val);
    }

    QEDegree<T>& operator+=(T degree)
    {
        degrees_ += degree;
        return *this;
    }

    QEDegree<T>& operator+=(const QEDegree<T>& val)
    {
        degrees_ += val.degrees_;
        return *this;
    }

    QEDegree<T>& operator+=(const QERadian<T>& val)
    {
        degrees_ += val.AsDegrees();
        return *this;
    }

    QEDegree<T>& operator-=(T degree)
    {
        degrees_ -= degree;
        return *this;
    }

    QEDegree<T>& operator-=(const QEDegree<T>& val)
    {
        degrees_ -= val.degrees_;
        return *this;
    }

    QEDegree<T>& operator-=(const QERadian<T>& val)
    {
        degrees_ -= val.AsDegrees();
        return *this;
    }

    QEDegree<T>& operator*=(T val)
    {
        degrees_ *= val;
        return *this;
    }

    QEDegree<T>& operator/=(T val)
    {
        degrees_ /= val;
        return *this;
    }

    QEDegree<T>& operator=(T degree)
    {
        degrees_ = degree;
        return *this;
    }

    QEDegree<T>& operator=(const QEDegree<T>& val)
    {
        degrees_ = val.degrees_;
        return *this;
    }

    QEDegree& operator=(const QERadian<T>& val)
    {
        degrees_ = val.AsRadians();
        return *this;
    }

private:

    T degrees_;
};

template <typename T>
class QERadian : public QEAngle <T>
{
public:

    QERadian()
        : radians_(0)
    {
    }

    QERadian(T radian)
        : radians_(radian)
    {
    }

    QERadian(const QEDegree<T>& val)
        : radians_(val.AsRadians())
    {
    }

    QERadian(const QERadian<T>& val)
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

    QERadian<T> operator+(T radian) const
    {
        return QERadian<T>(radians_ + radian);
    }

    QERadian<T> operator+(const QEDegree<T>& val) const
    {
        return QERadian<T>(radians_ + val.AsRadians());
    }

    QERadian<T> operator+(const QERadian<T>& val) const
    {
        return QERadian<T>(radians_ + val.radians_);
    }

    QERadian<T> operator-(T radian) const
    {
        return QERadian<T>(radians_ - radian);
    }

    QERadian<T> operator-(const QEDegree<T>& val) const
    {
        return QERadian<T>(radians_ - val.AsRadians());
    }

    QERadian<T> operator-(const QERadian<T>& val) const
    {
        return QERadian(radians_ - val.AsRadians());
    }

    QERadian<T> operator*(T val) const
    {
        return QERadian<T>(radians_ * val);
    }

    QERadian<T> operator/(T val) const
    {
        return QERadian<T>(radians_ / val);
    }

    QERadian<T>& operator+=(T radian)
    {
        radians_ += radian;
        return *this;
    }

    QERadian<T>& operator+=(const QEDegree<T>& val)
    {
        radians_ += val.AsRadians();
        return *this;
    }

    QERadian<T>& operator+=(const QERadian<T>& val)
    {
        radians_ += val.radians_;
        return *this;
    }

    QERadian<T>& operator-=(T radian)
    {
        radians_ -= radian;
        return *this;
    }

    QERadian<T>& operator-=(const QEDegree<T>& val)
    {
        radians_ -= val.AsRadians();
        return *this;
    }

    QERadian<T>& operator-=(const QERadian<T>& val)
    {
        radians_ -= val.radians_;
        return *this;
    }

    QERadian<T>& operator*=(T val)
    {
        radians_ *= val;
        return *this;
    }

    QERadian<T>& operator/=(T val)
    {
        radians_ /= val;
        return *this;
    }

    QERadian<T>& operator=(T radian)
    {
        radians_ = radian;
        return *this;
    }

    QERadian<T>& operator=(const QEDegree<T>& val)
    {
        radians_ = val.AsRadians();
        return *this;
    }

    QERadian<T>& operator=(const QERadian<T>& val)
    {
        radians_ = val.radians_;
        return *this;
    }

private:

    T radians_;
};

typedef QERadian<float> QERadF;
typedef QERadian<double> QERadD;
typedef QEDegree<float> QEDegF;
typedef QEDegree<double> QEDegD;

#endif