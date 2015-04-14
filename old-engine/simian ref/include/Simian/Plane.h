/************************************************************************/
/*!
\file		Plane.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_PLANE_H_
#define SIMIAN_PLANE_H_

#include "SimianPlatform.h"
#include "Vector3.h"

namespace Simian
{
    class Ray;

    class SIMIAN_EXPORT Plane
    {
    public:
        static const Plane X;
        static const Plane Y;
        static const Plane Z;
    private:
        Vector3 normal_;
        Vector3 point_;
    public:
        const Simian::Vector3& Normal() const;
        void Normal(const Simian::Vector3& val);

        const Simian::Vector3& Point() const;
        void Point(const Simian::Vector3& val);
    public:
        Plane(const Vector3& point, const Vector3& normal);

        void Normalize();
        Plane Normalized() const;

        bool Intersect(const Ray& ray, Vector3& intersectionPoint) const;
    };
}

#endif
