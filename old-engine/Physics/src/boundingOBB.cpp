#include "boundingOBB.h"

namespace Phy
{
    Util::FactoryPlant<BoundingVolume,BoundingOBB> BoundingOBB::factoryPlant_(BoundingVolumeFactory::InstancePtr(),BV_OBB);

    BoundingOBB::BoundingOBB()
        :   BoundingVolume(BV_OBB),
            center_(Math::Vec3F::Zero)
    {
        radius_[0] = Math::Vec3F::Zero;
        radius_[1] = Math::Vec3F::Zero;
        radius_[2] = Math::Vec3F::Zero;
    }

    BoundingOBB::BoundingOBB(const BoundingOBB& val)
        :   BoundingVolume(val),
            center_(val.center_)
    {
        radius_[0] = val.radius_[0];
        radius_[1] = val.radius_[1];
        radius_[2] = val.radius_[2];
    }

    BoundingOBB::~BoundingOBB()
    {
    }

    bool BoundingOBB::IsInside( const Math::Vec3F& ) const
    {
        return true;
    }

    //--------------------------------------------------------------------------

    void BoundingOBB::Center(const Math::Vec3F& val)
    {
        center_ = val;
    }

    Math::Vec3F BoundingOBB::Center() const
    {
        return center_;
    }

    Math::Vec3F BoundingOBB::Radius( unsigned int index ) const
    {
        return radius_[index];
    }

    /*void BoundingOBB::Radius( const Math::Vec3F val[3] )
    {
        radius_[0] = val[0];
        radius_[1] = val[1];
        radius_[2] = val[2];
    }*/

    void BoundingOBB::Radius( const Math::Vec3F& i,
                              const Math::Vec3F& j,
                              const Math::Vec3F& k )
    {
        radius_[0] = i;
        radius_[1] = j;
        radius_[2] = k;
    }

    void BoundingOBB::Radius( Math::Vec3F*& radius,
                              unsigned int& count )
    {
        radius = radius_;
        count = 3;
    }
}