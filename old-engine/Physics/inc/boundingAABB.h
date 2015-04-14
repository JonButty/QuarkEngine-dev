#ifndef BOUNDINGAABB_H
#define BOUNDINGAABB_H

#include "configPhysicsDLL.h"
#include "vector3.h"
#include "boundingVolume.h"
#include "boundingVolumeFactory.h"

namespace Phy
{
    class ENGINE_PHYSICS_EXPORT BoundingAABB
        :   public BoundingVolume
    {
    public:
        BoundingAABB();
        BoundingAABB(const BoundingAABB& val);
        ~BoundingAABB();
		inline Math::Vec3F Radius()const;
		void Radius(const Math::Vec3F& val);
		inline Math::Vec3F Center()const;
		void Center(const Math::Vec3F& val);
	public:
		Math::Vec3F MinPt() const;
		void MinPt(const Math::Vec3F& val);
		Math::Vec3F MaxPt() const;
		void MaxPt(const Math::Vec3F& val);
        bool IsInside(const Math::Vec3F& val) const;
        bool Intersection(const BoundingAABB& val) const;
    public:
        BoundingAABB& operator=(const BoundingAABB& val);
    private:
        Math::Vec3F minPt_;
		Math::Vec3F maxPt_;
        static Util::FactoryPlant<BoundingVolume,BoundingAABB> factoryPlant_;
    };
}

#endif