/********************************************************************
	created:	2015/01/27
	filename: 	Angle.h
	author:		Jonathan Butt
	purpose:	
*********************************************************************/

#ifndef ANGLE_H
#define ANGLE_H

namespace Math
{
    template <typename T>
    class Angle
    {
    public:
        virtual T AsRadians() const = 0;
        virtual T AsDegrees() const = 0;
    };

    template <typename T>
    class Radian;

    template <typename T>
    class Degree : public Angle <T>
    {
    public:
        Degree()
            : degrees_(0)
        {
        }

        Degree(T degrees)
            : degrees_(degrees)
        {
        }

        Degree(const Degree<T>& val)
            : degrees_(val.degrees_)
        {
        }

        Degree(const Radian<T>& val)
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

    public:

        Degree<T> operator+(T degree) const
        {
            return Degree<T>(degrees_ + degree);
        }

        Degree<T> operator+(const Degree<T>& val) const
        {
            return Degree<T>(degrees_ + val.degrees_());
        }

        Degree<T> operator+(const Radian<T>& val) const
        {
            return Degree<T>(degrees_ + val.AsDegrees());
        }

        Degree<T> operator-(T degree) const
        {
            return Degree<T>(degrees_ - degree);
        }

        Degree<T> operator-(const Degree<T>& val) const
        {
            return Degree<T>(degrees_ - val.AsDegrees());
        }

        Degree<T> operator-(const Radian<T>& val) const
        {
            return Degree<T>(degrees_ - val.AsDegrees());
        }

        Degree<T> operator*(T val) const
        {
            return Degree<T>(degrees_ * val);
        }

        Degree<T> operator/(T val) const
        {
            return Degree<T>(degrees_ / val);
        }

        Degree<T>& operator+=(T degree)
        {
            degrees_ += degree;
            return *this;
        }

        Degree<T>& operator+=(const Degree<T>& val)
        {
            degrees_ += val.degrees_;
            return *this;
        }

        Degree<T>& operator+=(const Radian<T>& val)
        {
            degrees_ += val.AsDegrees();
            return *this;
        }

        Degree<T>& operator-=(T degree)
        {
            degrees_ -= degree;
            return *this;
        }

        Degree<T>& operator-=(const Degree<T>& val)
        {
            degrees_ -= val.degrees_;
            return *this;
        }

        Degree<T>& operator-=(const Radian<T>& val)
        {
            degrees_ -= val.AsDegrees();
            return *this;
        }

        Degree<T>& operator*=(T val)
        {
            degrees_ *= val;
            return *this;
        }

        Degree<T>& operator/=(T val)
        {
            degrees_ /= val;
            return *this;
        }

        Degree<T>& operator=(T degree)
        {
            degrees_ = degree;
            return *this;
        }

        Degree<T>& operator=(const Degree<T>& val)
        {
            degrees_ = val.degrees_;
            return *this;
        }

        Degree& operator=(const Radian<T>& val)
        {
            degrees_ = val.AsRadians();
            return *this;
        }

    private:

        T degrees_;
    };

    template <typename T>
    class Radian : public Angle <T>
    {
    public:

        Radian()
            : radians_(0)
        {
        }

        Radian(T radian)
            : radians_(radian)
        {
        }

        Radian(const Degree<T>& val)
            : radians_(val.AsRadians())
        {
        }

        Radian(const Radian<T>& val)
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

        Radian<T> operator+(T radian) const
        {
            return Radian<T>(radians_ + radian);
        }

        Radian<T> operator+(const Degree<T>& val) const
        {
            return Radian<T>(radians_ + val.AsRadians());
        }

        Radian<T> operator+(const Radian<T>& val) const
        {
            return Radian<T>(radians_ + val.radians_);
        }

        Radian<T> operator-(T radian) const
        {
            return Radian<T>(radians_ - radian);
        }

        Radian<T> operator-(const Degree<T>& val) const
        {
            return Radian<T>(radians_ - val.AsRadians());
        }

        Radian<T> operator-(const Radian<T>& val) const
        {
            return Radian(radians_ - val.AsRadians());
        }

        Radian<T> operator*(T val) const
        {
            return Radian<T>(radians_ * val);
        }

        Radian<T> operator/(T val) const
        {
            return Radian<T>(radians_ / val);
        }

        Radian<T>& operator+=(T radian)
        {
            radians_ += radian;
            return *this;
        }

        Radian<T>& operator+=(const Degree<T>& val)
        {
            radians_ += val.AsRadians();
            return *this;
        }

        Radian<T>& operator+=(const Radian<T>& val)
        {
            radians_ += val.radians_;
            return *this;
        }

        Radian<T>& operator-=(T radian)
        {
            radians_ -= radian;
            return *this;
        }

        Radian<T>& operator-=(const Degree<T>& val)
        {
            radians_ -= val.AsRadians();
            return *this;
        }

        Radian<T>& operator-=(const Radian<T>& val)
        {
            radians_ -= val.radians_;
            return *this;
        }

        Radian<T>& operator*=(T val)
        {
            radians_ *= val;
            return *this;
        }

        Radian<T>& operator/=(T val)
        {
            radians_ /= val;
            return *this;
        }

        Radian<T>& operator=(T radian)
        {
            radians_ = radian;
            return *this;
        }

        Radian<T>& operator=(const Degree<T>& val)
        {
            radians_ = val.AsRadians();
            return *this;
        }

        Radian<T>& operator=(const Radian<T>& val)
        {
            radians_ = val.radians_;
            return *this;
        }

    private:

        T radians_;
    };

    typedef Radian<float> RadianF;
    typedef Radian<double> RadianD;
    typedef Degree<float> DegreeF;
    typedef Degree<double> DegreeD;
}

#endif