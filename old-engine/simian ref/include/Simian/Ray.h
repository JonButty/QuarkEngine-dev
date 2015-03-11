/************************************************************************/
/*!
\file		Ray.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_RAY_H_
#define SIMIAN_RAY_H_

#include "SimianPlatform.h"

#include "Vector3.h"
#include "Matrix.h"
#include "Angle.h"

namespace Simian
{
    class SIMIAN_EXPORT Ray
    {
    private:
        Vector3 position_;
        Vector3 direction_;
    public:
        const Simian::Vector3& Position() const;
        void Position(const Simian::Vector3& val);

        const Simian::Vector3& Direction() const;
        void Direction(const Simian::Vector3& val);
    public:
        Ray(const Vector3& position, const Vector3& direction);

        void Normalize();
        Ray Normalized() const;
        
        static Ray FromPoints(const Vector3& p0, const Vector3& p1);
        static Ray FromProjection(const Matrix& invView, const Matrix& projection, const Vector2& normalizedScreenCoordinates);
    };
}

#endif
