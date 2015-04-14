/************************************************************************/
/*!
\file		PointTrail.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_POINTTRAIL_H_
#define SIMIAN_POINTTRAIL_H_

#include "SimianPlatform.h"
#include "Vector3.h"

#include <vector>

namespace Simian
{
    class SIMIAN_EXPORT PointTrail
    {
    public:
        struct Segment
        {
            Vector3 Top;
            Vector3 Bottom;
            f32 Opacity;
        };
    private:
        f32 timer_;
        f32 duration_;
        Segment lastSegment_;
        std::vector<Segment> segments_;
    public:
        Simian::f32 Duration() const;
        void Duration(Simian::f32 val);

        const std::vector<PointTrail::Segment>& Segments() const;
    public:
        PointTrail(u32 segmentCount = 2);

        void Resize(u32 segments);
        void Reset(const Vector3& top, const Vector3& bottom, f32 opacity);
        void Update(f32 dt, const Vector3& top, const Vector3& bottom, f32 opacity);
    };
}

#endif
