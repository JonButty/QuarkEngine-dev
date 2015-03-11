/************************************************************************/
/*!
\file		TextureDX.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_TEXTUREDX_H_
#define SIMIAN_TEXTUREDX_H_

#include "SimianPlatform.h"
#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX

#include "Texture.h"
#include "IL/il.h"

#include <d3d9.h>

namespace Simian
{
    class SIMIAN_EXPORT Texture: public TextureBase
    {
    public:
        enum Flags
        {
            TF_IMAGELOADED,
            TF_TEXTURELOADED
        };

        enum SamplerStateFlag
        {
            SS_MINFILTER    = D3DSAMP_MINFILTER,
            SS_MAGFILTER    = D3DSAMP_MAGFILTER,
            SS_WRAPU        = D3DSAMP_ADDRESSU,
            SS_WRAPV        = D3DSAMP_ADDRESSV
        };

        enum TextureStateFlag
        {
            TSF_COLOROP         = D3DTSS_COLOROP,
            TSF_COLORARG1       = D3DTSS_COLORARG1,
            TSF_COLORARG2       = D3DTSS_COLORARG2,
            TSF_ALPHAOP         = D3DTSS_ALPHAOP,
            TSF_ALPHAARG1       = D3DTSS_ALPHAARG1,
            TSF_ALPHAARG2       = D3DTSS_ALPHAARG2,
            TSF_TEXCOORDINDEX   = D3DTSS_TEXCOORDINDEX,
            TSF_TRANSFORMFLAG   = D3DTSS_TEXTURETRANSFORMFLAGS,
            TSF_CONSTANT        = D3DTSS_CONSTANT
        };

        enum TextureStateOperation
        {
            TSO_DISABLED    = D3DTOP_DISABLE,
            TSO_ARG1        = D3DTOP_SELECTARG1,
            TSO_ARG2        = D3DTOP_SELECTARG2,
            TSO_MODULATE    = D3DTOP_MODULATE,
            TSO_MODULATE2X  = D3DTOP_MODULATE2X,
            TSO_MODULATE4X  = D3DTOP_MODULATE4X,
            TSO_ADD         = D3DTOP_ADD
        };

        enum TextureStateArgument
        {
            TSA_CONSTANT    = D3DTA_CONSTANT,
            TSA_CURRENT     = D3DTA_CURRENT,
            TSA_DIFFUSE     = D3DTA_DIFFUSE,
            TSA_TEXTURE     = D3DTA_TEXTURE
        };

        enum TextureTransformFlag
        {
            TTF_DISABLED   = D3DTTFF_DISABLE,
            TTF_COUNT1     = D3DTTFF_COUNT1,
            TTF_COUNT2     = D3DTTFF_COUNT2,
            TTF_COUNT3     = D3DTTFF_COUNT3,
            TTF_COUNT1PROJ = D3DTTFF_COUNT1 | D3DTTFF_PROJECTED,
            TTF_COUNT2PROJ = D3DTTFF_COUNT2 | D3DTTFF_PROJECTED,
            TTF_COUNT3PROJ = D3DTTFF_COUNT3 | D3DTTFF_PROJECTED
        };

        enum TextureCoordinateIndex
        {
            TCI_CAMERAPOSITION = D3DTSS_TCI_CAMERASPACEPOSITION
        };

        enum FilterFlag
        {
            FF_POINT        = D3DTEXF_POINT,
            FF_LINEAR       = D3DTEXF_LINEAR,
            FF_BILINEAR,
            FF_TRILINEAR
        };

        enum WrapFlag
        {
            WF_CLAMP        = D3DTADDRESS_CLAMP,
            WF_REPEAT       = D3DTADDRESS_WRAP
        };

        enum SurfaceFormat
        {
            SF_RGB = D3DFMT_X8R8G8B8,
            SF_RGBA = D3DFMT_A8R8G8B8,
            SF_RGBAF = D3DFMT_A16B16G16R16F
        };
    private:
        LPDIRECT3DTEXTURE9 systexture_;
        LPDIRECT3DTEXTURE9 texture_;
        ILuint image_;
        u8 dxFlags_;
    private:
        bool createTexture_();
        bool createSystemTexture_();
        bool createVRAMTexture_();
        bool loadTexture_();
        void set_(u32 sampler);
        void unset_();

        void samplerState_( u32 sampler, u32 state, u32 value );
        u32 samplerState_( u32 sampler, u32 state );

        void textureState_( u32 sampler, u32 state, u32 value );
        u32 textureState_( u32 sampler, u32 state );

        void data_(void* data, u32 size, u32 level);

        bool transferToVram_();
        void unloadVram_();

        void onLostDevice_();
        void onResetDevice_();
    public:
        Texture(GraphicsDevice* graphicsDevice);
        ~Texture();

        bool LoadImageFile(const DataLocation& data);
        void Unload();

        friend class GraphicsDevice;
        // HACK: will remove when video is supported cross platform
        friend class CVideo; 
    };
}

#endif

#endif
