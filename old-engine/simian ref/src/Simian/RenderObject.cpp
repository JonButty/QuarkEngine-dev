/************************************************************************/
/*!
\file		RenderObject.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/RenderQueue.h"
#include "Simian/Material.h"

namespace Simian
{
    RenderObject::RenderObject( Simian::Material& material, u8 layer, const Matrix& world, const Delegate& drawCallback, void* userData )
        : ParentQueue(0),
          Key(0),
          Layer(layer),
          Material(&material),
          World(world),
          DrawCallback(drawCallback),
          UserData(userData)
    {
    }

    bool RenderObject::operator<( const RenderObject& other ) const
    {
        return Key < other.Key;
    }

    void RenderObject::ComputeSortKey(const Matrix& view)
    {
        // sort key format: layer id(4 bits) : geomtype (2 bits) : depth (28 bits) <-> material (30 bits)
        u64 currentMask = static_cast<u64>(Layer) << 60;
        Key = 0;
        Key |= currentMask;
        currentMask = static_cast<u64>(Material->AlphaBlended()) << 58;
        Key |= currentMask;

        // calculate distance from active camera
        Simian::Vector3 position = Simian::Vector3(World[0][3], World[1][3], World[2][3]);
        view.Transform(position);
        f32 dist = position.Z();

        // convert dist to fixed point.. 14 : 14
        u64 depth = (u64)((1 << 14) * (dist ? dist : 0));
        depth &= 0x0FFFFFFF;
        depth = ~depth;
        depth &= 0x0FFFFFFF;

        u64 material = Material->Id();
        material &= 0x3FFFFFFF;

        if (Material->AlphaBlended())
            // alpha blended sort by depth then material
            depth <<= 30;
        else
            material <<= 28;

        Key |= depth;
        Key |= material;
    }
}
