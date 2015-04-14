#ifndef BOUNDINGVOLUME_H
#define BOUNDINGVOLUME_H

#include "configPhysicsDLL.h"
#include "vector3.h"

namespace Phy
{
    class ENGINE_PHYSICS_EXPORT BoundingVolume
    {
    public:
        BoundingVolume(unsigned int type);
        BoundingVolume(const BoundingVolume& val);
        virtual ~BoundingVolume();
        virtual bool IsInside(const Math::Vec3F& val) const = 0;
    public:
        unsigned int Type() const;
        unsigned int VertCount() const;
        void VertCount(unsigned int val);
        Math::Vec3F Center() const;
        void Center(const Math::Vec3F& val);
    protected:
        unsigned int type_;
        unsigned int vertexCount_;
        Math::Vec3F center_;
    };
}

#endif