#include "boundingAABB.h"
#include "boundingVolumeTypes.h"

#include <complex>

namespace Phy
{
    Util::FactoryPlant<BoundingVolume,BoundingAABB> BoundingAABB::factoryPlant_(BoundingVolumeFactory::InstancePtr(),BV_AABB);

    BoundingAABB::BoundingAABB()
        :   BoundingVolume(BV_AABB),
            minPt_(Math::Vec3F::Zero),
			maxPt_(Math::Vec3F::Zero)
    {
    }

    BoundingAABB::BoundingAABB( const BoundingAABB& val )
        :   BoundingVolume(val),
            minPt_(val.minPt_),
			maxPt_(val.maxPt_)
    {
    }

    BoundingAABB::~BoundingAABB()
    {
    }

    Math::Vec3F BoundingAABB::Radius() const
	{
		return Math::Vec3F(std::abs(maxPt_.X() - minPt_.X()) / 2.0f,
						   std::abs(maxPt_.Y() - minPt_.Y()) / 2.0f,
						   std::abs(maxPt_.Z() - minPt_.Z()) / 2.0f);
	}

	void BoundingAABB::Radius(const Math::Vec3F& val)
	{
		Math::Vec3F center(center_);
		Math::Vec3F radius(std::abs(val.X()),std::abs(val.Y()),std::abs(val.Z()));
		maxPt_ = center + val;
		minPt_ = center - val;
	}

	Math::Vec3F BoundingAABB::Center() const
	{
		return Math::Vec3F((maxPt_.X() + minPt_.X()) / 2.0f,
						   (maxPt_.Y() + minPt_.Y()) / 2.0f,
						   (maxPt_.Z() + minPt_.Z()) / 2.0f);
	}

	void BoundingAABB::Center(const Math::Vec3F& val)
	{
		Math::Vec3F radius(Radius());
		maxPt_ = val + radius;
		minPt_ = val - radius;
        center_ = val;
	}

	Math::Vec3F BoundingAABB::MinPt() const
	{
		return minPt_;
	}

	void BoundingAABB::MinPt(const Math::Vec3F& val)
	{
		minPt_ = val;
	}

	Math::Vec3F BoundingAABB::MaxPt() const
	{
		return maxPt_;
	}

	void BoundingAABB::MaxPt(const Math::Vec3F& val)
	{
		maxPt_ = val;
	}

    bool BoundingAABB::IsInside( const Math::Vec3F& val ) const
    {
		Math::Vec3F radius(Radius());
        if (std::abs(Center().X() - val.X()) > radius.X())
            return false;
        if (std::abs(Center().Y() - val.Y()) > radius.Y())
            return false;
        if (std::abs(Center().Z() - val.Z()) > radius.Z())
            return false;
        return true;
    }

    bool BoundingAABB::Intersection( const BoundingAABB& val ) const
    {
		Math::Vec3F center1(Center());
		Math::Vec3F center2(val.Center());
		Math::Vec3F radius1(Radius());
		Math::Vec3F radius2(val.Radius());
        if (std::abs(center1.X() - center2.X()) > (radius1.X() + radius2.X()))
            return false;
        if (std::abs(center1.Y() - center2.Y()) > (radius1.Y() + radius2.Y())) 
            return false;
        if (std::abs(center1.Z() - center2.Z()) > (radius1.Z() + radius2.Z()))
            return false;
        return true;
    }

    //--------------------------------------------------------------------------

    BoundingAABB& BoundingAABB::operator=( const BoundingAABB& val )
    {
		minPt_ = val.minPt_;
		maxPt_ = val.maxPt_;
        return *this;
    }
}