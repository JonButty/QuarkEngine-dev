/************************************************************************/
/*!
\file		IndexBufferDX.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/SimianPlatform.h"
#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX

#include "Simian/IndexBufferDX.h"
#include "Simian/GraphicsDevice.h"

namespace Simian
{
    IndexBuffer::IndexBuffer( GraphicsDevice* device )
        : IndexBufferBase(device),
          indexBuffer_(0)
    {
    }

    //--------------------------------------------------------------------------

    void IndexBuffer::update_( u32 offset, u32 size )
    {
        if (!Size() || !Data<u8*>())
            return;

        void* data;
        indexBuffer_->Lock(offset, size, &data, (Dynamic() ? D3DLOCK_DISCARD : 0));
        memcpy(data, Data<u8>() + offset, size ? size : Type() * Size());
        indexBuffer_->Unlock();
    }

    bool IndexBuffer::load_()
    {
        if (!Size() || !Data<u8*>())
            return false;

        D3DFORMAT format = 
            Type() == IBT_U16 ? D3DFMT_INDEX16 :
            Type() == IBT_U32 ? D3DFMT_INDEX32 :
            D3DFMT_INDEX16;

        HRESULT result = Device().D3DDevice()->CreateIndexBuffer(Type() * Size(), 
            (Dynamic() ? D3DUSAGE_DYNAMIC : 0) | D3DUSAGE_WRITEONLY,
            format, D3DPOOL_DEFAULT, &indexBuffer_, NULL);
        
        if (FAILED(result))
            return false;

        return true;
    }

    void IndexBuffer::unload_()
    {
        if (indexBuffer_)
            indexBuffer_->Release();
        indexBuffer_ = 0;
    }

    void IndexBuffer::onLostDevice_()
    {
        // release index buffer
        unload_();
    }

    void IndexBuffer::onResetDevice_()
    {
        // load and update indicies
        load_();
        update_(0, 0);
    }

    //--------------------------------------------------------------------------

    void IndexBuffer::Set()
    {
        Device().D3DDevice()->SetIndices(indexBuffer_);
    }

    void IndexBuffer::Unset()
    {
        Device().D3DDevice()->SetIndices(NULL);
    }
}

#endif
