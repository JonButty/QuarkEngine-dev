/************************************************************************/
/*!
\file		Texture.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_TEXTURE_H_
#define SIMIAN_TEXTURE_H_

#include "SimianPlatform.h"
#include "Color.h"
#include "Vector2.h"
#include "DataLocation.h"

namespace Simian
{
    class GraphicsDevice;
    class DataLocation;

    class SIMIAN_EXPORT TextureBase
    {
    public:
        enum SamplerStateFlag
        {
            SS_MINFILTER,
            SS_MAGFILTER,
            SS_WRAPU,
            SS_WRAPV
        };

        enum TextureStateFlag
        {
            TSF_COLOROP,
            TSF_COLORARG1,
            TSF_COLORARG2,
            TSF_ALPHAOP,
            TSF_ALPHAARG1,
            TSF_ALPHAARG2,
            TSF_TEXCOORDINDEX,
            TSF_TRANSFORMFLAG,
            TSF_CONSTANT
        };

        enum TextureStateOperation
        {
            TSO_DISABLED,
            TSO_ARG1,
            TSO_ARG2,
            TSO_MODULATE,
            TSO_MODULATE2X,
            TSO_MODULATE4X,
            TSO_ADD
        };

        enum TextureStateArgument
        {
            TSA_CONSTANT,
            TSA_CURRENT,
            TSA_DIFFUSE,
            TSA_TEXTURE
        };

        enum TextureTransformFlag
        {
            TTF_DISABLED,
            TTF_COUNT1,
            TTF_COUNT2,
            TTF_COUNT3,
            TTF_COUNT1PROJ,
            TTF_COUNT2PROJ,
            TTF_COUNT3PROJ
        };

        enum TextureCoordinateIndex
        {
            TCI_CAMERAPOSITION
        };

        enum FilterFlag
        {
            FF_POINT,
            FF_LINEAR,
            FF_BILINEAR,
            FF_TRILINEAR
        };

        enum WrapFlag
        {
            WF_CLAMP,
            WF_REPEAT
        };

        enum SurfaceFormat
        {
            SF_RGB,
            SF_RGBA,
            SF_RGBAF
        };
    private:
        GraphicsDevice* device_;
        u32 sampler_;
        bool active_;
        Simian::Vector2 size_;
        u32 levels_;
        u32 format_;
        bool dynamic_;
    public:
        Simian::u32 Sampler() const { return sampler_; }

        GraphicsDevice& Device() const { return *device_; }

        const Simian::Vector2& Size() const { return size_; }
        void Size(const Simian::Vector2& val) { size_ = val; }

        Simian::u32 Levels() const { return levels_; }

        void SamplerState(u32 state, u32 value) 
        { 
            if (active_)
                samplerState_(sampler_, state, value);
        }
        u32 SamplerState(u32 state)
        {
            if (active_)
                return samplerState_(sampler_, state);
            return 0;
        }

        void TextureState(u32 state, u32 value)
        {
            if (active_)
                textureState_(sampler_, state, value);
        }

        u32 TextureState(u32 state)
        {
            if (active_)
                return textureState_(sampler_, state);
            return 0;
        }

        Simian::u32 Format() const { return format_; }

        bool Dynamic() const { return dynamic_; }

        template <typename T> 
        void Data(T* data, u32 size, u32 level = 0)
        {
            data_(reinterpret_cast<void*>(data), size, level);
        }
    protected:
        void Levels(Simian::u32 val) { levels_ = val; }
    protected:
        S_NIMPL virtual void set_(u32 sampler) = 0;
        S_NIMPL virtual void unset_() = 0;
        S_NIMPL virtual bool loadTexture_() = 0;
        S_NIMPL virtual bool createTexture_() = 0;

        S_NIMPL virtual void samplerState_(u32 sampler, u32 state, u32 value) = 0;
        S_NIMPL virtual u32 samplerState_(u32 sampler, u32 state) = 0;

        S_NIMPL virtual void textureState_(u32 sampler, u32 state, u32 value) = 0;
        S_NIMPL virtual u32 textureState_(u32 sampler, u32 state) = 0;

        S_NIMPL virtual void data_(void* data, u32 size, u32 level) = 0;
    public:
        TextureBase(GraphicsDevice* device): 
          device_(device),
          sampler_(0),
          active_(false),
          levels_(0),
          format_(0),
          dynamic_(false) {}
        virtual ~TextureBase() {}

        void Set(u32 sampler = 0)
        {
            sampler_ = sampler;
            active_ = true;
            set_(sampler);
        }

        void Unset() 
        {
            unset_();
            active_ = false;
            sampler_ = 0;
        }

        bool LoadImmediate(const DataLocation& data, u32 levels = 0) 
        {
            if (!LoadImageFile(data))
                return false;
            return LoadTexture(levels);
        }
        S_NIMPL virtual bool LoadImageFile(const DataLocation&) = 0;
        bool CreateTexture(u32 width, u32 height, u32 format, u32 levels = 0, bool dynamic = false) 
        {
            size_ = Simian::Vector2(static_cast<f32>(width), static_cast<f32>(height));
            format_ = format;
            levels_ = levels;
            dynamic_ = dynamic;
            return createTexture_();
        }
        bool LoadTexture(u32 levels = 0) { levels_ = levels; return loadTexture_(); }
        S_NIMPL void Unload() {}
    };
}

#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX
#include "TextureDX.h"
#elif SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_GL
#include "TextureGL.h"
#endif

#endif
