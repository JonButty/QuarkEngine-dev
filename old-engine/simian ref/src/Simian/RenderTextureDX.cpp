/************************************************************************/
/*!
\file		RenderTextureDX.cpp
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

#include "Simian/RenderTexture.h"
#include "Simian/GraphicsDevice.h"

namespace Simian
{
    RenderTexture::RenderTexture(GraphicsDevice* device)
        : RenderTextureBase(device),
          texture_(0)
    {
    }

    //--------------------------------------------------------------------------

    void RenderTexture::set_( u32 sampler )
    {
        Device().D3DDevice()->SetTexture(sampler, texture_);
    }

    void RenderTexture::unset_()
    {
        Device().D3DDevice()->SetTexture(Sampler(), NULL);
    }

    void RenderTexture::samplerState_( u32 sampler, u32 state, u32 value )
    {
        Device().D3DDevice()->SetSamplerState(
            sampler, static_cast<D3DSAMPLERSTATETYPE>(state), value);
    }

    Simian::u32 RenderTexture::samplerState_( u32 sampler, u32 state )
    {
        DWORD val;
        Device().D3DDevice()->GetSamplerState(
            sampler, static_cast<D3DSAMPLERSTATETYPE>(state), &val);
        return val;
    }
    
    void RenderTexture::textureState_( u32 sampler, u32 state, u32 value )
    {
        Device().D3DDevice()->SetTextureStageState(
            sampler, static_cast<D3DTEXTURESTAGESTATETYPE>(state), value);
    }

    Simian::u32 RenderTexture::textureState_( u32 sampler, u32 state )
    {
        DWORD val;
        Device().D3DDevice()->GetTextureStageState(
            sampler, static_cast<D3DTEXTURESTAGESTATETYPE>(state), &val);
        return val;
    }

    void RenderTexture::onLostDevice_()
    {
        if (texture_)
            Unload();
    }

    void RenderTexture::onResetDevice_()
    {
        if (Size().X() > 0.0f && Size().Y() > 0.0f)
            LoadTexture(Size(), format_);
    }

    //--------------------------------------------------------------------------

    bool RenderTexture::LoadImmediate( const Simian::Vector2& size, u32 format )
    {
        return RenderTextureBase::LoadImmediate(size, format);
    }

    bool RenderTexture::LoadTexture( const Simian::Vector2& s, u32 format )
    {
        LPDIRECT3DSURFACE9 backBuffer;
        D3DSURFACE_DESC desc;
        HRESULT result = Device().D3DDevice()->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);
        if (FAILED(result))
            return false;
        backBuffer->GetDesc(&desc);
        Vector2 size;
        if (autoResize_)
            size = Simian::Vector2(static_cast<f32>(desc.Width),
                                   static_cast<f32>(desc.Height));
        else
            size = Vector2(s.X() <= desc.Width ? s.X() : static_cast<f32>(desc.Width), 
            s.Y() <= desc.Height ? s.Y() : static_cast<f32>(desc.Height));
        backBuffer->Release();

        Size(size);
        format_ = static_cast<RenderTexture::SurfaceFormat>(format);

        result = Device().D3DDevice()->CreateTexture(
            static_cast<u32>(size.X()), static_cast<u32>(size.Y()),
            1, D3DUSAGE_RENDERTARGET, static_cast<D3DFORMAT>(format), 
            D3DPOOL_DEFAULT, &texture_, NULL);

        if (FAILED(result))
            return false;

        result = texture_->GetSurfaceLevel(0, &surface_);

        if (FAILED(result))
        {
            texture_->Release();
            texture_ = 0;
            return false;
        }

        return true;
    }

    void RenderTexture::Unload()
    {
        surface_->Release();
        surface_ = 0;
        texture_->Release();
        texture_ = 0;
    }

    void RenderTexture::Use()
    {
        Device().D3DDevice()->SetRenderTarget(0, surface_);
    }

    void RenderTexture::Unuse()
    {
        LPDIRECT3DSURFACE9 backBuffer;
        Device().D3DDevice()->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);
        Device().D3DDevice()->SetRenderTarget(0, backBuffer);
        backBuffer->Release();
    }
}

#endif
