/************************************************************************/
/*!
\file		Trail.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Trail.h"
#include "Simian/Math.h"
#include "Simian/LogManager.h"
#include "Simian/Interpolation.h"

namespace Simian
{
    Trail::Trail( u32 segments )
        : segments_(segments),
          timer_(0.0f),
          interpolationInterval_(0.1f),
          segmentTwist_(0.0f),
          segmentTwistVelocity_(0.0f)
    {
    }

    //--------------------------------------------------------------------------

    const Vector3& Trail::Position() const
    {
        return segments_[0].Position;
    }

    void Trail::Position( const Vector3& position )
    {
        segments_[0].Position = position;
    }

    Simian::f32 Trail::Twist() const
    {
        return segments_[0].Twist;
    }

    void Trail::Twist( f32 twist )
    {
        segments_[0].Twist = Simian::Math::WrapAngle(Simian::Radian(twist)).Radians();
    }

    Simian::f32 Trail::InterpolationInterval() const
    {
        return interpolationInterval_;
    }

    void Trail::InterpolationInterval( Simian::f32 val )
    {
        interpolationInterval_ = val;
    }

    const std::vector<Trail::Segment>& Trail::Segments() const
    {
        return segments_;
    }

    Simian::f32 Trail::SegmentTwist() const
    {
        return segmentTwist_;
    }

    void Trail::SegmentTwist( Simian::f32 val )
    {
        segmentTwist_ = val;
    }

    Simian::f32 Trail::SegmentTwistVelocity() const
    {
        return segmentTwistVelocity_;
    }

    void Trail::SegmentTwistVelocity( Simian::f32 val )
    {
        segmentTwistVelocity_ = val;
    }

    Simian::f32 Trail::TrailLength() const
    {
        return trailLength_;
    }

    //--------------------------------------------------------------------------

    void Trail::Resize( u32 segments )
    {
        segments_.resize(segments);
        Reset();
    }

    void Trail::Reset()
    {
        // copy the first element to all elements
        std::fill(segments_.begin() + 1, segments_.end(), segments_[0]);
        lastPosition_ = segments_[0].Position;
    }

    void Trail::Update( float dt )
    {
        trailLength_ = 0.0f;

        timer_ += dt;
        f32 intp = timer_/interpolationInterval_;
        intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
        Segment& tail = segments_[segments_.size() - 1];
        Segment& tailFront = segments_[segments_.size() - 2];
        tail.Position = Simian::Interpolation::Interpolate(lastPosition_, tailFront.Position, intp, Simian::Interpolation::Linear);

        if (intp >= 1.0f)
        {
            // reset timer
            timer_ = 0.0f;

            // set last position
            lastPosition_ = tailFront.Position;
            // shift everything back by 1
            std::copy(segments_.rbegin() + 1, segments_.rend(), segments_.rbegin());
        }

        // update all segments (except the head)
        for (u32 i = 1; i < segments_.size(); ++i)
        {
            // update trail length
            segments_[i].Length = (segments_[i - 1].Position - segments_[i].Position).Length();
            trailLength_ += segments_[i].Length;

            // update twist
            f32 twistDelta = segmentTwistVelocity_ * dt;
            f32 twistDiff = Simian::Math::NearestAngleDifference(
                Simian::Radian(segments_[i].Twist),
                Simian::Radian(segments_[i - 1].Twist)).Radians();
            f32 direction = twistDiff > 0.0f ? 1.0f : -1.0f;
            twistDiff = std::abs(twistDiff);
            if (twistDiff < twistDelta)
                twistDelta = twistDiff;
            else if (twistDiff > segmentTwist_)
                twistDelta = twistDiff - segmentTwist_;
            segments_[i].Twist += direction * twistDelta;
        }
    }
}
