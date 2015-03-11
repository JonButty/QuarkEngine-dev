/************************************************************************/
/*!
\file		CVideo.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/CVideo.h"
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/CSprite.h"
#include "Simian/Material.h"
#include "Simian/Texture.h"
#include "Simian/LogManager.h"
#include "Simian/VideoClip.h"
#include "Simian/GraphicsDevice.h"

namespace Simian
{
    FactoryPlant<EntityComponent, CVideo> CVideo::plant_(
        &GameFactory::Instance().ComponentFactory(), C_VIDEO);

    Simian::GraphicsDevice* CVideo::graphicsDevice_;

    CVideo::CVideo()
        : sprite_(0),
          videoClip_(0),
          bufferSize_(0),
          yuvSurface_(0)
    {
    }

    //--------------------------------------------------------------------------

    Simian::VideoClip& CVideo::Video()
    {
        return *videoClip_;
    }

    Simian::GraphicsDevice& CVideo::GraphicsDevice()
    {
        return *graphicsDevice_;
    }

    void CVideo::GraphicsDevice( Simian::GraphicsDevice* val )
    {
        graphicsDevice_ = val;
    }

    //--------------------------------------------------------------------------

    void CVideo::onRelease_( DelegateParameter& )
    {
        yuvSurface_->Release();
        yuvSurface_ = 0;
    }

    void CVideo::onReset_( DelegateParameter& )
    {
        HRESULT ret = graphicsDevice_->D3DDevice()->CreateOffscreenPlainSurface(
            videoClip_->Width(), videoClip_->Height(), 
            (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),
            D3DPOOL_DEFAULT, &yuvSurface_, NULL);
        if (FAILED(ret))
        {
            SWarn("Failed to create YV12 surface.");
            yuvSurface_ = 0;
            return;
        }

        RECT rect = { 0, 0, videoClip_->Height(), videoClip_->Width() };
        graphicsDevice_->D3DDevice()->ColorFill(yuvSurface_, &rect, 
            D3DCOLOR_ARGB(0xFF, 0, 0, 0));
    }

    void CVideo::update_( Simian::DelegateParameter& param )
    {
        if (!bufferSize_)
            return;

        // return if video is not playing
        if (videoClip_->State() != VideoClip::PS_PLAYING)
            return;

        EntityUpdateParameter* p;
        param.As(p);
        videoClip_->Update(p->Dt);

        // if the video is no longer playing we cannot update.
        if (videoClip_->State() != VideoClip::PS_PLAYING)
            return;

        // update material in sprite.
        Texture* texture = (*sprite_->Material())[0].TextureStages()[0].Texture();
        if (texture->Dynamic())
        {
            u8* y, *u, *v;
            u32 ys, us, vs;

            // bail if there are no frames ready
            if (!yuvSurface_ || !videoClip_->ExtractFrame(&y, ys, &u, us, &v, vs))
                return;

            D3DLOCKED_RECT rect;
            yuvSurface_->LockRect(&rect, NULL, D3DLOCK_DISCARD);
            u8* y1Ptr = (u8*)rect.pBits;
            u8* y2Ptr = y1Ptr + rect.Pitch;
            u32 hPitch = rect.Pitch/2;
            u8* vPtr = y1Ptr + rect.Pitch * videoClip_->Height();
            u8* uPtr = vPtr + hPitch * videoClip_->Height()/2;

            u32 uvStride = videoClip_->Width()/2;

            for (u32 i = 0; i < videoClip_->Height(); i += 2)
            {
                memcpy(y1Ptr, y, videoClip_->Width());
                memcpy(y2Ptr, y + ys, videoClip_->Width());
                y += 2 * ys;
                y1Ptr += 2 * rect.Pitch;
                y2Ptr += 2 * rect.Pitch;

                memcpy(vPtr, v, uvStride);
                v += vs;
                vPtr += hPitch;

                memcpy(uPtr, u, uvStride);
                u += us;
                uPtr += hPitch;
            }

            yuvSurface_->UnlockRect();
            
            // stretch rect unto textures surface
            LPDIRECT3DSURFACE9 textureSurface;
            texture->texture_->GetSurfaceLevel(0, &textureSurface);
            graphicsDevice_->D3DDevice()->StretchRect(
                yuvSurface_, NULL, textureSurface, NULL, D3DTEXF_LINEAR);
            textureSurface->Release();
        }
    }

    //--------------------------------------------------------------------------

    void CVideo::OnSharedLoad()
    {
        RegisterCallback(P_UPDATE, Delegate::CreateDelegate<CVideo, &CVideo::update_>(this));
    }

    void CVideo::OnAwake()
    {
        ComponentByType(C_SPRITE, sprite_);

        videoClip_ = new VideoClip();
        if (!videoClip_->Load(videoFile_))
        {
            SWarn("Video clip failed to load: " << videoFile_);
            return;
        }

        videoClip_->Play();

        bufferSize_ = videoClip_->Width() * videoClip_->Height();

        // create a surface for the yuv data
        // TODO: release and reset surface accordingly!
        HRESULT ret = graphicsDevice_->D3DDevice()->CreateOffscreenPlainSurface(
            videoClip_->Width(), videoClip_->Height(), 
            (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),
            D3DPOOL_DEFAULT, &yuvSurface_, NULL);
        if (FAILED(ret))
        {
            SWarn("Failed to create YV12 surface.");
            yuvSurface_ = 0;
            return;
        }

        RECT rect = { 0, 0, videoClip_->Height(), videoClip_->Width() };
        graphicsDevice_->D3DDevice()->ColorFill(yuvSurface_, &rect, 
                                                D3DCOLOR_ARGB(0xFF, 0, 0, 0));

        graphicsDevice_->registerResource_(
            Delegate::CreateDelegate<CVideo, &CVideo::onRelease_>(this),
            Delegate::CreateDelegate<CVideo, &CVideo::onReset_>(this));
    }

    void CVideo::OnDeinit()
    {
        graphicsDevice_->unregisterResource_(
            Delegate::CreateDelegate<CVideo, &CVideo::onRelease_>(this),
            Delegate::CreateDelegate<CVideo, &CVideo::onReset_>(this));

        if (videoClip_)
        {
            videoClip_->Stop();
            videoClip_->Unload();
            delete videoClip_;
        }
        videoClip_ = 0;
        bufferSize_ = 0;

        if (yuvSurface_)
            yuvSurface_->Release();
        yuvSurface_ = 0;
    }

    void CVideo::Serialize( FileObjectSubNode& data )
    {
        data.AddData("VideoFile", videoFile_);
    }

    void CVideo::Deserialize( const FileObjectSubNode& data )
    {
        data.Data("VideoFile", videoFile_);
    }
}
