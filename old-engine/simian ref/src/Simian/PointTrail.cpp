/************************************************************************/
/*!
\file		ParticleRendererRibbonTrail.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/PointTrail.h"
#include "Simian/Math.h"
#include "Simian/Interpolation.h"

namespace Simian
{
    PointTrail::PointTrail( u32 segmentCount )
        : timer_(0.0f),
          duration_(1.0f)
    {
        segments_.resize(segmentCount);
    }

    //--------------------------------------------------------------------------

    Simian::f32 PointTrail::Duration() const
    {
        return duration_;
    }

    void PointTrail::Duration( Simian::f32 val )
    {
        duration_ = val;
    }

    const std::vector<PointTrail::Segment>& PointTrail::Segments() const
    {
        return segments_;
    }

    //--------------------------------------------------------------------------

    void PointTrail::Resize( u32 segments )
    {
        segments_.resize(segments);
    }

    void PointTrail::Reset( const Vector3& top, const Vector3& bottom, f32 opacity )
    {
        timer_ = 0.0f;
        for (u32 i = 0; i < segments_.size(); ++i)
        {
            segments_[i].Top = top;
            segments_[i].Bottom = bottom;
            segments_[i].Opacity = opacity;
        }
    }

    void PointTrail::Update( f32 dt, const Vector3& top, const Vector3& bottom, f32 opacity )
    {
        // animate the first segment
        segments_[0].Top = top;
        segments_[0].Bottom = bottom;
        segments_[0].Opacity = opacity;

        // add to timer
        timer_ += dt;

        // move the back nodes closer
        f32 intp = timer_/duration_;
        intp = Math::Clamp(intp, 0.0f, 1.0f);

        // animate the last segment
        u32 lastSeg = segments_.size() - 1;
        segments_[lastSeg].Top = Simian::Interpolation::Interpolate(lastSegment_.Top, 
            segments_[lastSeg - 1].Top, intp, Simian::Interpolation::Linear);
        segments_[lastSeg].Bottom = Simian::Interpolation::Interpolate(lastSegment_.Bottom, 
            segments_[lastSeg - 1].Bottom, intp, Simian::Interpolation::Linear);
        segments_[lastSeg].Opacity = Simian::Interpolation::Interpolate(lastSegment_.Opacity, 
            segments_[lastSeg - 1].Opacity, intp, Simian::Interpolation::Linear);

        // is it time to put a new head in?
        if (timer_ > duration_)
        {
            // copy everything back
            std::copy(segments_.rbegin() + 1, segments_.rend(), segments_.rbegin());
            lastSegment_ = segments_.back();
            timer_ = 0.0f;
        }
    }
}
