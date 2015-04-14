/************************************************************************/
/*!
\file		RenderTextureDX.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_RENDERTEXTUREDX_H_
#define SIMIAN_RENDERTEXTUREDX_H_

#include "SimianPlatform.h"
#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX

#include "RenderTexture.h"

#include <d3d9.h>

namespace Simian
{
    class SIMIAN_EXPORT RenderTexture: public RenderTextureBase
    {
    private:
        LPDIRECT3DTEXTURE9 texture_;
        LPDIRECT3DSURFACE9 surface_;
        SurfaceFormat format_;
    private:
        void set_(u32 sampler);
        void unset_();

        void samplerState_(u32 sampler, u32 state, u32 value);
        u32 samplerState_(u32 sampler, u32 state);

        void textureState_(u32 sampler, u32 state, u32 value);
        u32 textureState_(u32 sampler, u32 state);

        void onLostDevice_();
        void onResetDevice_();
    public:
        RenderTexture(GraphicsDevice* device);

        bool LoadImmediate(const Simian::Vector2& size, u32 format = Texture::SF_RGB);
        bool LoadTexture(const Simian::Vector2& size, u32 format = Texture::SF_RGB);
        void Unload();

        void Use();
        void Unuse();

        friend class GraphicsDevice;
    };
}

#endif

#endif
