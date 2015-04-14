#ifndef FACE_H
#define FACE_H

#include "vector3.h"

namespace GFX
{
    struct HalfEdge;

    struct Face
    {
        HalfEdge* edge_;
        Math::Vec3F norm_;

        Face()
            :   edge_(0),
                norm_(Math::Vec3F::Zero)
        {
        }
    };
}

#endif