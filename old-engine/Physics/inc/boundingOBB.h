#ifndef BOUNDINGOBB_H
#define BOUNDINGOBB_H

#include "configPhysicsDLL.h"
#include "vector3.h"
#include "boundingVolume.h"
#include "boundingVolumeFactory.h"

namespace Phy
{
    class ENGINE_PHYSICS_EXPORT BoundingOBB
        :   public BoundingVolume
    {
    public:
        BoundingOBB();
        BoundingOBB(const BoundingOBB& val);
        ~BoundingOBB();
        bool IsInside(const Math::Vec3F& val) const;
    public:
        void Center(const Math::Vec3F& val);
        Math::Vec3F Center() const;
        Math::Vec3F Radius(unsigned int index) const;
        void Radius(const Math::Vec3F& i,
                    const Math::Vec3F& j,
                    const Math::Vec3F& k);
        void Radius(Math::Vec3F*& axis,
                    unsigned int& count);
    private:
        Math::Vec3F center_;
        Math::Vec3F radius_[3]; 
        static Util::FactoryPlant<BoundingVolume,BoundingOBB> factoryPlant_;
    };
}

#endif