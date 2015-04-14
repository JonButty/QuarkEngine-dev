#include "boundingVolume.h"

namespace Phy
{
    BoundingVolume::BoundingVolume( unsigned int type )
        :   type_(type),
            center_(Math::Vec3F::Zero),
            vertexCount_(0)
    {
    }

    BoundingVolume::BoundingVolume( const BoundingVolume& val )
        :   type_(val.type_),
            center_(val.center_),
            vertexCount_(val.vertexCount_)
    {
    }

    BoundingVolume::~BoundingVolume()
    {
    }

    //--------------------------------------------------------------------------

    unsigned int BoundingVolume::Type() const
    {
        return type_;
    }

    void BoundingVolume::VertCount(unsigned int val)
    {
        vertexCount_ = val;
    }

    unsigned int BoundingVolume::VertCount() const
    {
        return vertexCount_;
    }

    Math::Vec3F BoundingVolume::Center() const
    {
        return center_;
    }

    void BoundingVolume::Center( const Math::Vec3F& val )
    {
        center_ = val;
    }
}