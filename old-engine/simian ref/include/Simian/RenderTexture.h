/************************************************************************/
/*!
\file		RenderTexture.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_RENDERTEXTURE_H_
#define SIMIAN_RENDERTEXTURE_H_

#include "SimianPlatform.h"
#include "Texture.h"

namespace Simian
{
    class SIMIAN_EXPORT RenderTextureBase: public TextureBase
    {
    public:
        bool autoResize_;
    public:
        bool AutoResize() const { return autoResize_; }
        void AutoResize(bool val) { autoResize_ = val; }
    private:
        // hide unused methods
        Simian::u32 Levels() const { return 0; }
        bool LoadImmediate(const DataLocation&, u32) { return false; }
        bool LoadImageFile(const DataLocation&) { return false; }
        bool LoadTexture(u32) { return false; }
        bool loadTexture_() { return false; }
        bool createTexture_() { return false; }
        void data_(void*, u32, u32) {}
    public:
        RenderTextureBase(GraphicsDevice* device)
        : TextureBase(device), autoResize_(false) {}
        virtual ~RenderTextureBase() {}

        bool LoadImmediate(const Simian::Vector2& size, u32 format) { return LoadTexture(size, format); }
        S_NIMPL virtual bool LoadTexture(const Simian::Vector2& size, u32 format) = 0;
        S_NIMPL void Unload() {}

        S_NIMPL void Use() {}
        S_NIMPL void Unuse() {}
    };
}

#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX
#include "RenderTextureDX.h"
#elif SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_GL
#include "RenderTextureGL.h"
#endif

#endif
