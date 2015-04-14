/************************************************************************/
/*!
\file		Plane.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Plane.h"
#include "Simian/Ray.h"
#include "Simian/Math.h"

namespace Simian
{
    const Plane Plane::X(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
    const Plane Plane::Y(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f));
    const Plane Plane::Z(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
    
    //--------------------------------------------------------------------------

    Plane::Plane( const Vector3& point, const Vector3& normal )
        : point_(point),
          normal_(normal)
    {
    }

    //--------------------------------------------------------------------------

    const Simian::Vector3& Plane::Normal() const
    {
        return normal_;
    }

    void Plane::Normal( const Simian::Vector3& val )
    {
        normal_ = val;
    }

    const Simian::Vector3& Plane::Point() const
    {
        return point_;
    }

    void Plane::Point( const Simian::Vector3& val )
    {
        point_ = val;
    }

    //--------------------------------------------------------------------------

    void Plane::Normalize()
    {
        normal_.Normalize();
    }

    Simian::Plane Plane::Normalized() const
    {
        return Plane(point_, normal_.Normalized());
    }

    bool Plane::Intersect( const Ray& ray, Vector3& intersectionPoint ) const
    {
        f32 denom = normal_.Dot(ray.Direction());
        if (denom > -Math::EPSILON && denom < Math::EPSILON)
            return false;
        f32 t = normal_.Dot(point_ - ray.Position())/denom;
        intersectionPoint = ray.Position() + t * ray.Direction();
        return true;
    }
}
