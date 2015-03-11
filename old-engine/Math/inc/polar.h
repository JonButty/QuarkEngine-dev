#ifndef POLAR_H
#define POLAR_H

#include "angle.h"
#include "vector2.h"

namespace Math
{
    template <typename T>
    class Polar
    {
    public:
        Polar()
            :   radius_(0),
                angle_(0)
        {
        }

        Polar(float radius,
              const Degree<T>& angle)
            :   radius_(radius),
                angle_(angle)
        {
        }

        Polar(float radius,
              const Radian<T>& angle)
            :   radius_(val.radius_),
                angle_(val.angle_)
        {
        }

        Polar(const Polar<T>& val)
            :   radius_(val.radius_),
                angle_(val.angle_)
        {
        }

        Vector2<T> AsCartesian() const
        {
            return Vector2<T>(radius_ * std::cos(angle_.AsRadians()),
                              radius_ * std::sin(angle_.AsRadians()));
        }

        void AsCartesian(const Vector2<T>& val)
        {
            radius_ = val.Length();
            angle_.AsRadians(std::acos(val.X() / radius_));
        }

    public:
        Radian<T> Theta() const
        {
            return angle_;
        }

        void Theta(const Radian<T>& angle)
        {
            angle_ = angle.AsRadians();
        }

        void Radius(T val)
        {
            angle_ = val;
        }

        T Radius() const
        {
            return radius_;
        }

    public:
        Polar<T>& operator=(const Polar<T>& val)
        {
            radius_ = val.radius_;
            angle_ = val.angle_;
            return *this;
        }

    private:
        T radius_;
        Radian<T> angle_;
    };

    typedef Polar<float> PolarF;
    typedef Polar<double> PolarD;
}

#endif