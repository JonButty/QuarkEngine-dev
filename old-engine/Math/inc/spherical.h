#ifndef SPHERICAL_H
#define SPHERICAL_H

#include "vector3.h"
#include "angle.h"

namespace Math
{
    template <typename T, typename U>
    class Spherical
    {
    public:

        Spherical()
            :   radius_(static_cast<T>(1)),
                thetaVert_(0),
                thetaHoriz_(0)
        {
        }

        Spherical(const Degree<T>& alpha,
                  const Degree<T>& beta,
                  T radius = static_cast<T>(1))
                  : radius_(radius),
                    thetaVert_(alpha.AsRadians()),
                    thetaHoriz_(beta.AsRadians())
        {
        }

        Spherical(const Radian<T>& alpha,
                  const Radian<T>& beta,
                  T radius = static_cast<T>(1))
                  : radius_(radius),
                    thetaVert_(alpha),
                    thetaHoriz_(beta)
        {
        }

        Spherical(const U& pos)
        {
            AsCartesian(pos);
        }

        U AsCartesian() const
        {
             U ret(radius_ * static_cast<T>(std::cos(thetaVert_.AsRadians()) * std::sin(thetaHoriz_.AsRadians())),
                   radius_ * static_cast<T>(std::sin(thetaVert_.AsRadians())),
                   radius_ * static_cast<T>(std::cos(thetaVert_.AsRadians()) * std::cos(thetaHoriz_.AsRadians())));
             return ret;
        }

        void AsCartesian(const U& pos)
        {
            radius_ = static_cast<T>(std::sqrt(std::pow(pos.X(),2) + std::pow(pos.Y(),2) + std::pow(pos.Z(),2)));
            thetaVert_ = static_cast<T>(std::asin(pos.Y()/radius_));
            thetaHoriz_ = static_cast<T>(-std::atan2(pos.X(),pos.Z()));
        }

    public:

        T Radius() const
        {
            return radius_;
        }

        void Radius(float val)
        {
            radius_ = val;
        }

        Radian<T> ThetaVert() const
        {
            return thetaVert_;
        }

        void ThetaVert(const Radian<T>& val)
        {
            thetaVert_ = val;
        }

        Radian<T> ThetaHoriz() const
        {
            return thetaHoriz_;
        }

        void ThetaHoriz(const Radian<T>& val)
        {
            thetaHoriz_ = val;
        }

    public:

        Spherical<T,U> operator+(const Spherical<T,U>& val) const
        {
            return Spherical<T,U>(thetaVert_ + val.thetaVert_,
                                  thetaHoriz_ + val.thetaHoriz_,
                                  radius_ + val.radius_);
        }

        Spherical<T,U> operator-(const Spherical<T,U>& val) const
        {
            return Spherical<T,U>(thetaVert_ - val.thetaVert_,
                                  thetaHoriz_ - val.thetaHoriz_,
                                  radius_ - val.radius_);
        }

        Spherical<T,U>& operator+=(const Spherical<T,U>& val)
        {
            radius_     += val.radius_;
            thetaVert_  += val.thetaVert_;
            thetaHoriz_ += val.thetaHoriz_;
            return *this;
        }

        Spherical<T,U>& operator-=(const Spherical<T,U>& val)
        {
            radius_     -= val.radius_;
            thetaVert_  -= val.thetaVert_;
            thetaHoriz_ -= val.thetaHoriz_;
            return *this;
        }

        Spherical<T,U>& operator=(const Spherical<T,U>& val)
        {
            radius_     = val.radius_;
            thetaVert_  = val.thetaVert_;
            thetaHoriz_ = val.thetaHoriz_;
            return *this;
        }

    private:

        T radius_;
        Radian<T> thetaVert_;
        Radian<T> thetaHoriz_;

    };

    // Float
    typedef Spherical<float,Vec3F> SphericalF;

    // Double
    typedef Spherical<double,Vec3D> SphericalD;
}

#endif