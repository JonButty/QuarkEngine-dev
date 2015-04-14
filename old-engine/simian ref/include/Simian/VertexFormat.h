/************************************************************************/
/*!
\file		VertexFormat.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_VERTEXFORMAT_H_
#define SIMIAN_VERTEXFORMAT_H_

#include "SimianPlatform.h"
#include <vector>

namespace Simian
{
    struct SIMIAN_EXPORT VertexAttribute
    {
        enum VertexUsage
        {
            VFU_POSITION,
            VFU_NORMAL,
            VFU_COLOR,
            VFU_TEXTURE,
            VFU_BONEINDEX,
            VFU_BONEWEIGHT
        } Usage;

        enum VertexType
        {
            VFT_FLOAT2,
            VFT_FLOAT3,
            VFT_FLOAT4,
            VFT_COLOR,
            VFT_UBYTE4
        } Type;

        u32 Index;
        u32 Offset;

        u32 Size() const
        {
            return Type == VFT_FLOAT2 ? sizeof(f32) * 2 :
                Type == VFT_FLOAT3 ? sizeof(f32) * 3 :
                Type == VFT_FLOAT4 ? sizeof(f32) * 4 :
                Type == VFT_COLOR ? sizeof(u32) :
                Type == VFT_UBYTE4 ? sizeof(u8) * 4 :
                0;
        }
    };

    class SIMIAN_EXPORT VertexFormat
    {
    private:
        std::vector<VertexAttribute> attributes_;
        u32 size_;
    public:
        const std::vector<VertexAttribute>& Attributes() const;

        u32 Size() const;
    public:
        VertexFormat();

        void AddAttribute(VertexAttribute::VertexUsage usage, VertexAttribute::VertexType type, u32 index = 0, u32 offset = 0);
    };
}

#endif
