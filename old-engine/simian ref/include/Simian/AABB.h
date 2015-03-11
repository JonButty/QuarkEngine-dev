/************************************************************************/
/*!
\file		AABB.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_AABB_H_
#define SIMIAN_AABB_H_

#include "SimianPlatform.h"
#include "Vector3.h"
#include "Matrix.h"

namespace Simian
{
    class SIMIAN_EXPORT AABB
    {
    private:
        Vector3 min_;
        Vector3 max_;
    public:
        const Simian::Vector3& Min() const;
        void Min(const Simian::Vector3& val);

        const Simian::Vector3& Max() const;
        void Max(const Simian::Vector3& val);

        Simian::Vector3 Midpoint() const;

        Simian::Vector3 Size() const;

        float Width() const;

        float Height() const;

        float Depth() const;

        float Volume() const;

        bool Exists() const;
    public:
        // joins two aabbs
        AABB& operator+=(const AABB& other);
        AABB operator+(const AABB& other) const;
    public:
        AABB(const Vector3& min = Vector3::Zero, const Vector3& max = Vector3::Zero);

        void Join(const AABB& other);
        void TransformRigid(const Matrix& mat);
    };
}

#endif
