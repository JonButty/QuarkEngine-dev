#ifndef BOUNDINGELLIPSE_H
#define BOUNDINGELLIPSE_H

#include "configPhysicsDLL.h"
#include "boundingVolume.h"
#include "boundingVolumeFactory.h"
#include "vector3.h"

namespace Phy
{
    class ENGINE_PHYSICS_EXPORT BoundingEllipse
        :   public BoundingVolume
    {
    public:
        BoundingEllipse();
        BoundingEllipse(const BoundingEllipse& val);
        ~BoundingEllipse();
        bool IsInside(const Math::Vec3F& val) const;
        bool Intersection(const BoundingEllipse& val) const;
        void Grow(const Math::Vec3F& pt);
    public:
        Math::Vec3F I() const;
        void I(const Math::Vec3F& val);
        Math::Vec3F J() const;
        void J(const Math::Vec3F& val);
        Math::Vec3F K() const;
        void K(const Math::Vec3F& val);
        Math::Vec3F Center() const;
        void Center(const Math::Vec3F& val);
    public:
        BoundingEllipse& operator=(const BoundingEllipse& val);
    private:
        Math::Vec3F i_;
        Math::Vec3F j_;
        Math::Vec3F k_;
        Math::Vec3F center_;
        static Util::FactoryPlant<BoundingVolume,BoundingEllipse> factoryPlant_;
    };
}

#endif