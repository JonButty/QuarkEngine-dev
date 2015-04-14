/************************************************************************/
/*!
\file		TextureDX.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/TextureDX.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/Utility.h"
#include "Simian/DataLocation.h"

#include "IL/ilu.h"

#include <string>
#include <sstream>
#include <algorithm>

namespace Simian
{
    static const u32 MIN_LEVEL = 3;

    Texture::Texture(GraphicsDevice* graphicsDevice)
        : TextureBase(graphicsDevice),
          systexture_(0),
          texture_(0),
          image_(0),
          dxFlags_(0)
    {
    }

    Texture::~Texture()
    {
    }

    //--------------------------------------------------------------------------

    bool Texture::createTexture_()
    {
        // compute levels
        if (!Levels())
        {
            // calculate the rounded total levels
            u32 totalLevels = static_cast<u32>(log(Size().X())/log(2.0f) + 0.5f);
            Levels(totalLevels > MIN_LEVEL ? totalLevels - MIN_LEVEL : 1);
        }

        // create the texture on vram
        if (!createVRAMTexture_())
            return false;
        
        SFlagSet(dxFlags_, TF_TEXTURELOADED);

        return true;
    }

    bool Texture::createSystemTexture_()
    {
        // create texture on ram
        HRESULT result = Device().D3DDevice()->CreateTexture(
            static_cast<UINT>(Size().X()),
            static_cast<UINT>(Size().Y()),
            Levels(), 0, static_cast<D3DFORMAT>(Format()), D3DPOOL_SYSTEMMEM, &systexture_, NULL);

        if (FAILED(result))
            return false;

        return true;
    }

    bool Texture::createVRAMTexture_()
    {
        HRESULT result = Device().D3DDevice()->CreateTexture(
            static_cast<u32>(Size().X()),
            static_cast<u32>(Size().Y()),
            Levels(), Dynamic() ? D3DUSAGE_RENDERTARGET : 0, 
            static_cast<D3DFORMAT>(Format()), D3DPOOL_DEFAULT, &texture_, NULL);

        if (FAILED(result))
            return false;

        return true;
    }

    bool Texture::loadTexture_()
    {
        if (!Levels())
        {
            // calculate the rounded total levels
            u32 totalLevels = static_cast<u32>(log(Size().X())/log(2.0f) + 0.5f);
            Levels(totalLevels > MIN_LEVEL ? totalLevels - MIN_LEVEL : 1);
        }

        ilBindImage(image_);

        if (ilGetError() != IL_NO_ERROR)
            return false;

        if (!CreateTexture(static_cast<u32>(Size().X()), static_cast<u32>(Size().Y()), 
                           Texture::SF_RGBA, Levels()))
            return false;
        SFlagUnset(dxFlags_, TF_TEXTURELOADED); // unset from create texture.

        createSystemTexture_();

        for (u32 i = 0; i < systexture_->GetLevelCount(); ++i)
        {
            u32 width = static_cast<u32>(Size().X())/(1 << i);
            u32 height = static_cast<u32>(Size().Y())/(1 << i);

            // set the active mipmap if it exists, otherwise scale it down
            //if (ilActiveMipmap(i) == IL_FALSE) // mip map support for dds in openil seems broken. :/
            {
                iluImageParameter(ILU_FILTER, ILU_SCALE_MITCHELL);
                iluScale(width, height, 1);
                /*std::stringstream ss;
                ss << "Test" << i << ".png";
                std::string fileName = ss.str();
                ilSaveImage(fileName.c_str());*/
            }

            D3DLOCKED_RECT rect;
            systexture_->LockRect(i, &rect, NULL, 0);

            u8* pixels = new u8[width * height * sizeof(u8) * 4];
            u8* pixelsPtr = pixels;
            ilCopyPixels(0, 0, 0, width, height, 1, IL_RGBA, IL_UNSIGNED_BYTE, pixels);
            u32 numPixels = width * height;
            for (u32 j = 0; j < numPixels; ++j)
            {
                u8 r = *(pixelsPtr)++;
                u8 g = *(pixelsPtr)++;
                u8 b = *(pixelsPtr)++;
                u8 a = *(pixelsPtr)++;

                r = (r * a)/255;
                g = (g * a)/255;
                b = (b * a)/255;

                reinterpret_cast<DWORD*>(rect.pBits)[j] = D3DCOLOR_ARGB(a, r, g, b);
            }
            systexture_->UnlockRect(i);
            delete [] pixels;
        }

        // transfer to vram
        if (!transferToVram_())
            return false;

        SFlagSet(dxFlags_, TF_TEXTURELOADED);
        return true;
    }

    void Texture::set_(u32 sampler)
    {
        Device().D3DDevice()->SetTexture(sampler, texture_);
    }

    void Texture::unset_()
    {
        Device().D3DDevice()->SetTexture(Sampler(), NULL);
    }

    void Texture::samplerState_( u32 sampler, u32 state, u32 value )
    {
        if (state == SS_MINFILTER && value == FF_BILINEAR)
        {
            Device().D3DDevice()->SetSamplerState(sampler, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            Device().D3DDevice()->SetSamplerState(sampler, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
        }
        else if (state == SS_MINFILTER && value == FF_TRILINEAR)
        {
            Device().D3DDevice()->SetSamplerState(sampler, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            Device().D3DDevice()->SetSamplerState(sampler, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
        }
        else
            Device().D3DDevice()->SetSamplerState(sampler, static_cast<D3DSAMPLERSTATETYPE>(state), value);
    }

    u32 Texture::samplerState_( u32 sampler, u32 state )
    {
        DWORD ret;
        Device().D3DDevice()->GetSamplerState(sampler, static_cast<D3DSAMPLERSTATETYPE>(state), &ret);
        return ret;
    }

    void Texture::textureState_( u32 sampler, u32 state, u32 value )
    {
        Device().D3DDevice()->SetTextureStageState(sampler, static_cast<D3DTEXTURESTAGESTATETYPE>(state), value);
    }

    u32 Texture::textureState_( u32 sampler, u32 state )
    {
        DWORD ret;
        Device().D3DDevice()->GetTextureStageState(sampler, static_cast<D3DTEXTURESTAGESTATETYPE>(state), &ret);
        return ret;
    }

    void Texture::data_( void* data, u32 size, u32 level )
    {
        LPDIRECT3DTEXTURE9 texture = systexture_ ? systexture_ : texture_;
        SAssert(texture != 0, "No available texture is present.");

        // lock the rect
        D3DLOCKED_RECT rect;
        texture->LockRect(level, &rect, NULL, D3DLOCK_DISCARD);
        std::memcpy(rect.pBits, data, size);
        texture->UnlockRect(level);

        // do this incase its a systexture.
        transferToVram_();
    }

    bool Texture::transferToVram_()
    {
        // we don't transfer if system texture doesn't exist
        if (!systexture_)
            return false;

#define HANDLE_ERROR() if (FAILED(result))\
        {\
            return false;\
        }

        // transfer all levels from system texture
        for (u32 i = 0; i < systexture_->GetLevelCount(); ++i)
        {
            LPDIRECT3DSURFACE9 src, dst;
            HRESULT result = systexture_->GetSurfaceLevel(i, &src);
            HANDLE_ERROR();
            result = texture_->GetSurfaceLevel(i, &dst);
            HANDLE_ERROR();
            result = Device().D3DDevice()->UpdateSurface(src, NULL, dst, NULL);
            HANDLE_ERROR();
            src->Release();
            dst->Release();
        }

#undef HANDLE_ERROR

        return true;
    }

    void Texture::unloadVram_()
    {
        if (texture_)
        {
            texture_->Release();
            texture_ = 0;
        }
    }

    void Texture::onLostDevice_()
    {
        unloadVram_();
    }

    void Texture::onResetDevice_()
    {
        createVRAMTexture_();
        transferToVram_();
    }

    //--------------------------------------------------------------------------

    bool Texture::LoadImageFile( const DataLocation& data )
    {
        if (!data)
            return false;

        image_ = ilGenImage();

        if (ilGetError() != IL_NO_ERROR)
            return false;

        ilBindImage(image_);

        if (ilGetError() != IL_NO_ERROR)
            return false;

        std::string ext = data.Identifier().substr(data.Identifier().length() - 3);
        std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

        ILenum fileType =   ext == "png" ? IL_PNG :
                            ext == "dds" ? IL_DDS :
                            ext == "jpg" ? IL_JPG :
                            ext == "tga" ? IL_TGA :
                            ext == "bmp" ? IL_BMP :
                            IL_TYPE_UNKNOWN;

        ilLoadL(fileType, data.Memory(), data.Size());

        if (ilGetError() != IL_NO_ERROR)
            return false;

        Size(Simian::Vector2(
            static_cast<f32>(ilGetInteger(IL_IMAGE_WIDTH)),
            static_cast<f32>(ilGetInteger(IL_IMAGE_HEIGHT))));

        SFlagSet(dxFlags_, TF_IMAGELOADED);
        return true;
    }

    void Texture::Unload()
    {
        if (SIsFlagSet(dxFlags_, TF_TEXTURELOADED))
        {
            // unload texture
            unloadVram_();

            // unload ram
            if (systexture_)
            {
                systexture_->Release();
                systexture_ = 0;
            }

            SFlagUnset(dxFlags_, TF_TEXTURELOADED);
        }

        if (SIsFlagSet(dxFlags_, TF_IMAGELOADED))
        {
            // unload image
            ilDeleteImage(image_);
            image_ = 0;

            SFlagUnset(dxFlags_, TF_IMAGELOADED);
        }
    }
}
