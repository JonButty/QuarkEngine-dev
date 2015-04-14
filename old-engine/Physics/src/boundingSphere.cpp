#include "boundingSphere.h"
#include "boundingVolumeTypes.h"

namespace Phy
{
    Util::FactoryPlant<BoundingVolume,BoundingSphere> BoundingSphere::factoryPlant_(BoundingVolumeFactory::InstancePtr(),BV_SPHERE);

    BoundingSphere::BoundingSphere()
        :   BoundingVolume(BV_SPHERE),
            radius_(0)
    {
    }

    BoundingSphere::BoundingSphere( const BoundingSphere& val )
        :   BoundingVolume(val),
            radius_(val.radius_)
    {
    }

    BoundingSphere::~BoundingSphere()
    {
    }

    bool BoundingSphere::IsInside( const Math::Vec3F& val ) const
    {
        float sqrRadius = radius_ * radius_;
        return Math::Vec3F(Center() - val).SquareLength() < sqrRadius;
    }

    bool BoundingSphere::Intersection( const BoundingSphere& val ) const
    {
        float sqrDist = Math::Vec3F(Center() - val.Center()).SquareLength();
        float sqrTotalRadius = (radius_ + val.radius_) * (radius_ + val.radius_);
        return sqrDist < sqrTotalRadius;
    }

    void BoundingSphere::Grow( const Math::Vec3F& pt )
    {
        Math::Vec3F distVec = pt - center_;
        float distSqr = distVec.SquareLength();

        if(distSqr > (radius_ * radius_))
        {
            float dist = std::sqrt(distSqr);
            float newRadius = (radius_ + dist) * 0.5f;
            float k = (newRadius - radius_) / dist;
            radius_ = newRadius;
            center_ += distVec * k;
        }
    }

    //--------------------------------------------------------------------------

    void BoundingSphere::Radius( float val )
    {
        radius_ = val;
    }

    float BoundingSphere::Radius() const
    {
        return radius_;
    }

    BoundingSphere& BoundingSphere::operator=( const BoundingSphere& val )
    {
        center_ = val.center_;
        radius_ = val.radius_;
        return *this;
    }
}