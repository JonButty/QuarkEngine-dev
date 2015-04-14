/************************************************************************/
/*!
\file		AABB.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/AABB.h"

#include <algorithm>
#include "Simian/Math.h"

namespace Simian
{
    AABB::AABB( const Vector3& min, const Vector3& max )
        : min_(min),
          max_(max)
    {
        // per component swap
        if (max_[0] < min_[0]) std::swap(min_[0], max_[0]);
        if (max_[1] < min_[1]) std::swap(min_[1], max_[1]);
        if (max_[2] < min_[2]) std::swap(min_[2], max_[2]);
    }

    //--------------------------------------------------------------------------

    const Simian::Vector3& AABB::Min() const
    {
        return min_;
    }

    void AABB::Min( const Simian::Vector3& val )
    {
        min_ = val;
    }

    const Simian::Vector3& AABB::Max() const
    {
        return max_;
    }

    void AABB::Max( const Simian::Vector3& val )
    {
        max_ = val;
    }

    Simian::Vector3 AABB::Midpoint() const
    {
        return 0.5f * (min_ + max_);
    }

    Simian::Vector3 AABB::Size() const
    {
        return max_ - min_;
    }

    float AABB::Width() const
    {
        return max_.X() - min_.X();
    }

    float AABB::Height() const
    {
        return max_.Y() - min_.Y();
    }

    float AABB::Depth() const
    {
        return max_.Z() - min_.Z();
    }

    float AABB::Volume() const
    {
        Simian::Vector3 size = Size();
        return size.X() * size.Y() * size.Z();
    }

    bool AABB::Exists() const
    {
        return Size().Dot(Simian::Vector3(1.0f, 1.0f, 1.0f)) > Simian::Math::EPSILON;
    }

    //--------------------------------------------------------------------------

    AABB& AABB::operator+=( const AABB& other )
    {
        Join(other);
        return *this;
    }

    Simian::AABB AABB::operator+( const AABB& other ) const
    {
        AABB ret(*this);
        ret += other;
        return ret;
    }

    //--------------------------------------------------------------------------

    void AABB::Join( const AABB& other )
    {
        min_.X(other.min_.X() < min_.X() ? other.min_.X() : min_.X());
        min_.Y(other.min_.Y() < min_.Y() ? other.min_.Y() : min_.Y());
        min_.Z(other.min_.Z() < min_.Z() ? other.min_.Z() : min_.Z());
        max_.X(other.max_.X() > max_.X() ? other.max_.X() : max_.X());
        max_.Y(other.max_.Y() > max_.Y() ? other.max_.Y() : max_.Y());
        max_.Z(other.max_.Z() > max_.Z() ? other.max_.Z() : max_.Z());
    }

    void AABB::TransformRigid( const Matrix& mat )
    {
        // FROM GRAPHICS GEMS
        Vector3 min = min_;
        Vector3 max = max_;

        f32 av, bv;

        min_ = mat.Position();
        max_ = mat.Position();

        for (u32 j = 0; j < 3; ++j)
        {
            for (u32 i = 0; i < 3; ++i)
            {
                av = mat[j][i] * min[i];
                bv = mat[j][i] * max[i];

                if(av < bv)
                {
                    min_[j] += av;
                    max_[j] += bv;
                }
                else
                {
                    min_[j] += bv;
                    max_[j] += av;
                }
            }
        }
    }
}
