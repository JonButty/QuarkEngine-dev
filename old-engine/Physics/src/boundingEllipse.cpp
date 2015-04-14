#include "boundingEllipse.h"

namespace Phy
{
    Util::FactoryPlant<BoundingVolume,BoundingEllipse> BoundingEllipse::factoryPlant_(BoundingVolumeFactory::InstancePtr(),BV_ELLIPSE);

    BoundingEllipse::BoundingEllipse()
        :   BoundingVolume(BV_ELLIPSE),
            i_(Math::Vec3F::Zero),
            j_(Math::Vec3F::Zero),
            k_(Math::Vec3F::Zero),
            center_(Math::Vec3F::Zero)
    {
    }

    BoundingEllipse::BoundingEllipse( const BoundingEllipse& val )
        :   BoundingVolume(val),
            i_(val.i_),
            j_(val.j_),
            k_(val.k_),
            center_(val.center_)
    {
    }

    BoundingEllipse::~BoundingEllipse()
    {
    }

    bool BoundingEllipse::IsInside( const Math::Vec3F& val ) const
    {
        //Math::Vec3F pt = center_ - val;
        return true;//((pt.X() / radius_.X()) + (pt.Y() / radius_.Y()) + (pt.Z() / radius_.Z())) <= 1.0f;
    }

    bool BoundingEllipse::Intersection( const BoundingEllipse& val ) const
    {
        return true;
    }

    void BoundingEllipse::Grow( const Math::Vec3F& pt )
    {
        pt;
    }

    //--------------------------------------------------------------------------

    void BoundingEllipse::K( const Math::Vec3F& val )
    {
        k_ = val;
    }

    Math::Vec3F BoundingEllipse::K() const
    {
        return k_;
    }

    void BoundingEllipse::J( const Math::Vec3F& val )
    {
        j_ = val;
    }

    Math::Vec3F BoundingEllipse::J() const
    {
        return j_;
    }

    void BoundingEllipse::I( const Math::Vec3F& val )
    {
        i_ = val;
    }

    Math::Vec3F BoundingEllipse::I() const
    {
        return i_;
    }

    Math::Vec3F BoundingEllipse::Center() const
    {
        return center_;
    }

    void BoundingEllipse::Center( const Math::Vec3F& val )
    {
        center_ = val;
    }

    //--------------------------------------------------------------------------

    BoundingEllipse& BoundingEllipse::operator=( const BoundingEllipse& val )
    {
        i_ = val.i_;
        j_ = val.j_;
        k_ = val.k_;
        center_ = val.center_;
        return *this;
    }
}