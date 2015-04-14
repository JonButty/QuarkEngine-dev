/************************************************************************/
/*!
\file		VertexBuffer.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/VertexBuffer.h"

namespace Simian
{
    static VertexFormat PositionColorFormat()
    {
        VertexFormat format;

        format.AddAttribute(VertexAttribute::VFU_POSITION, VertexAttribute::VFT_FLOAT3);
        format.AddAttribute(VertexAttribute::VFU_COLOR, VertexAttribute::VFT_COLOR);

        return format;
    }

    static VertexFormat PositionColorUVFormat()
    {
        VertexFormat format;

        format.AddAttribute(VertexAttribute::VFU_POSITION, VertexAttribute::VFT_FLOAT3);
        format.AddAttribute(VertexAttribute::VFU_COLOR, VertexAttribute::VFT_COLOR);
        format.AddAttribute(VertexAttribute::VFU_TEXTURE, VertexAttribute::VFT_FLOAT2);

        return format;
    }

    static VertexFormat PositionNormalColorUVFormat()
    {
        VertexFormat format;

        format.AddAttribute(VertexAttribute::VFU_POSITION, VertexAttribute::VFT_FLOAT3);
        format.AddAttribute(VertexAttribute::VFU_NORMAL, VertexAttribute::VFT_FLOAT3);
        format.AddAttribute(VertexAttribute::VFU_COLOR, VertexAttribute::VFT_COLOR);
        format.AddAttribute(VertexAttribute::VFU_TEXTURE, VertexAttribute::VFT_FLOAT2);

        return format;
    }

    const VertexFormat PositionColorVertex::Format(PositionColorFormat());
    const VertexFormat PositionColorUVVertex::Format(PositionColorUVFormat());
    const VertexFormat PositionNormalColorUVVertex::Format(PositionNormalColorUVFormat());
}
