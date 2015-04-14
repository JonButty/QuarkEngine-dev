#ifndef BOUNDINGSPHERE_H
#define BOUNDINGSPHERE_H

#include "configPhysicsDLL.h"
#include "boundingVolume.h"
#include "boundingVolumeFactory.h"
#include "vector3.h"

namespace Phy
{
    class ENGINE_PHYSICS_EXPORT BoundingSphere
        :   public BoundingVolume
    {
    public:
        BoundingSphere();
        BoundingSphere(const BoundingSphere& val);
        ~BoundingSphere();
        bool IsInside(const Math::Vec3F& val) const;
        bool Intersection(const BoundingSphere& val) const;
        void Grow(const Math::Vec3F& pt);
    public:
        float Radius() const;
        void Radius(float val);
    public:
        BoundingSphere& operator=(const BoundingSphere& val);
    private:
        float radius_;
        static Util::FactoryPlant<BoundingVolume,BoundingSphere> factoryPlant_;
    };
}

#endif