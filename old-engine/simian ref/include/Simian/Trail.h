/************************************************************************/
/*!
\file		Trail.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_RIBBONTRAIL_H_
#define SIMIAN_RIBBONTRAIL_H_

#include "SimianPlatform.h"
#include "Vector3.h"
#include "VertexBuffer.h"

#include <vector>

namespace Simian
{
    class SIMIAN_EXPORT Trail
    {
    public:
        struct Segment
        {
            Vector3 Position;
            f32 Twist;
            f32 Length;

            Segment(): Twist(0), Length(0) {}
        };
    private:
        std::vector<Segment> segments_;
        
        f32 timer_;
        f32 interpolationInterval_;
        Vector3 lastPosition_;
        f32 segmentTwist_;
        f32 segmentTwistVelocity_;

        f32 trailLength_;
    public:
        const Vector3& Position() const;
        void Position(const Vector3& position);

        f32 Twist() const;
        void Twist(f32 twist);

        Simian::f32 InterpolationInterval() const;
        void InterpolationInterval(Simian::f32 val);

        Simian::f32 SegmentTwist() const;
        void SegmentTwist(Simian::f32 val);

        Simian::f32 SegmentTwistVelocity() const;
        void SegmentTwistVelocity(Simian::f32 val);

        Simian::f32 TrailLength() const;

        const std::vector<Segment>& Segments() const;
    public:
        Trail(u32 segments = 2);

        void Resize(u32 segments);
        void Reset();
        void Update(float dt);
    };
}

#endif
