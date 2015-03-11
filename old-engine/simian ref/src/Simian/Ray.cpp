/************************************************************************/
/*!
\file		Ray.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Ray.h"

namespace Simian
{
    Ray::Ray( const Vector3& position, const Vector3& direction )
        : position_(position),
          direction_(direction)
    {
    }

    //--------------------------------------------------------------------------

    const Simian::Vector3& Ray::Position() const
    {
        return position_;
    }

    void Ray::Position( const Simian::Vector3& val )
    {
        position_ = val;
    }

    const Simian::Vector3& Ray::Direction() const
    {
        return direction_;
    }

    void Ray::Direction( const Simian::Vector3& val )
    {
        direction_ = val;
    }

    //--------------------------------------------------------------------------

    void Ray::Normalize()
    {
        direction_.Normalize();
    }

    Simian::Ray Ray::Normalized() const
    {
        return Ray(position_, direction_.Normalized());
    }

    Simian::Ray Ray::FromPoints( const Vector3& p0, const Vector3& p1 )
    {
        return Ray(p0, p1 - p0);
    }

    Simian::Ray Ray::FromProjection( const Matrix& invView, const Matrix& projection, const Vector2& normalizedScreenCoordinates )
    {
        f32 aspect = projection[0][0]/projection[1][1];
        f32 tanfovy = 1.0f/projection[1][1];

        Vector2 dss = Vector2(
            tanfovy * normalizedScreenCoordinates.X()/aspect,
            tanfovy * normalizedScreenCoordinates.Y());

        Vector3 near = Simian::Vector3(dss.X() * 0.1f, dss.Y() * 0.1f, 0.1f);
        Vector3 far = Simian::Vector3(dss.X() * 10.0f, dss.Y() * 10.0f, 10.0f);

        invView.Transform(near);
        invView.Transform(far);

        return Ray(near, far - near);
    }
}
