/************************************************************************/
/*!
\file		VertexBufferDX.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/VertexBufferDX.h"
#include "Simian/GraphicsDevice.h"

#include <vector>
#include "Simian/LogManager.h"

namespace Simian
{
    VertexBuffer::VertexBuffer(GraphicsDevice* device)
        : VertexBufferBase(device),
          vertexDeclaration_(0),
          vertexBuffer_(0)
    {
    }

    //--------------------------------------------------------------------------

    void VertexBuffer::update_(u32 offset, u32 size)
    {
        if (!Size() || !Data<u8*>())
            return;

        // lock the vertex buffer and upload the vertices
        void* data;
        vertexBuffer_->Lock(offset, size, &data, (Dynamic() ? D3DLOCK_DISCARD : 0));
        memcpy(data, Data<u8*>() + offset, !size ? Format().Size() * Size() : size);
        vertexBuffer_->Unlock();
    }

    bool VertexBuffer::load_()
    {
        std::vector<D3DVERTEXELEMENT9> elements;
        const std::vector<VertexAttribute>& attributes = Format().Attributes();
        u16 offset = 0;

        for (std::vector<VertexAttribute>::const_iterator i = attributes.begin();
            i != attributes.end(); ++i)
        {
            const VertexAttribute& attribute = *i;
            offset += static_cast<u16>(attribute.Offset);

            D3DVERTEXELEMENT9 element;
            element.Stream = 0;
            element.Offset = offset;
            element.Type = static_cast<BYTE>(
                attribute.Type == VertexAttribute::VFT_FLOAT2 ? D3DDECLTYPE_FLOAT2 :
                attribute.Type == VertexAttribute::VFT_FLOAT3 ? D3DDECLTYPE_FLOAT3 :
                attribute.Type == VertexAttribute::VFT_FLOAT4 ? D3DDECLTYPE_FLOAT4 :
                attribute.Type == VertexAttribute::VFT_COLOR ? D3DDECLTYPE_D3DCOLOR :
                attribute.Type == VertexAttribute::VFT_UBYTE4 ? D3DDECLTYPE_UBYTE4 :
                D3DDECLTYPE_FLOAT3);
            element.Method = D3DDECLMETHOD_DEFAULT;
            element.Usage = static_cast<BYTE>(
                attribute.Usage == VertexAttribute::VFU_POSITION ? D3DDECLUSAGE_POSITION :
                attribute.Usage == VertexAttribute::VFU_NORMAL ? D3DDECLUSAGE_NORMAL :
                attribute.Usage == VertexAttribute::VFU_COLOR ? D3DDECLUSAGE_COLOR :
                attribute.Usage == VertexAttribute::VFU_TEXTURE ? D3DDECLUSAGE_TEXCOORD :
                attribute.Usage == VertexAttribute::VFU_BONEINDEX ? D3DDECLUSAGE_BLENDINDICES :
                attribute.Usage == VertexAttribute::VFU_BONEWEIGHT ? D3DDECLUSAGE_BLENDWEIGHT :
                D3DDECLUSAGE_POSITION);
            element.UsageIndex = static_cast<BYTE>(attribute.Index);

            offset += static_cast<u16>(attribute.Size());

            elements.push_back(element);
        }
        D3DVERTEXELEMENT9 elementEnd = D3DDECL_END();
        elements.push_back(elementEnd);

        // create a vertex declaration
        HRESULT result = Device().D3DDevice()->CreateVertexDeclaration(&elements[0], &vertexDeclaration_);

        if (FAILED(result))
            return false;

        if (!loadVertexBuffer_())
        {
            vertexDeclaration_->Release();
            vertexDeclaration_ = 0;
            return false;
        }

        return true;
    }

    void VertexBuffer::unload_()
    {
        unloadVertexBuffer_();

        if (vertexDeclaration_)
            vertexDeclaration_->Release();
        vertexDeclaration_ = 0;
    }

    bool VertexBuffer::loadVertexBuffer_()
    {
        // don't load if theres no size
        if (!Size() || !Data<u8*>())
            return false;

        // create vertex buffer
        HRESULT result = Device().D3DDevice()->CreateVertexBuffer(Format().Size() * Size(), 
            (Dynamic() ? D3DUSAGE_DYNAMIC : 0) | D3DUSAGE_WRITEONLY, 0, 
            D3DPOOL_DEFAULT, &vertexBuffer_, NULL);

        if (FAILED(result))
        {
            SVerbose("Failed to load");
        }

        return FAILED(result) ? false : true;
    }

    void VertexBuffer::unloadVertexBuffer_()
    {
        if (vertexBuffer_)
            vertexBuffer_->Release();
        vertexBuffer_ = 0;
    }

    void VertexBuffer::onLostDevice_()
    {
        // release the vertex buffer
        unloadVertexBuffer_();
    }

    void VertexBuffer::onResetDevice_()
    {
        // load and fill the vertex buffer
        loadVertexBuffer_();
        update_(0, 0);
    }

    //--------------------------------------------------------------------------

    void VertexBuffer::Set(u32 offset)
    {
        Device().D3DDevice()->SetVertexDeclaration(vertexDeclaration_);
        Device().D3DDevice()->SetStreamSource(0, vertexBuffer_, offset, Format().Size());
    }

    void VertexBuffer::Unset()
    {
        Device().D3DDevice()->SetStreamSource(0, 0, 0, 0);
    }
}
