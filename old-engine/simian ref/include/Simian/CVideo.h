/************************************************************************/
/*!
\file		CVideo.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CVIDEO_H_
#define SIMIAN_CVIDEO_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"

#include <string>

#include <d3d9.h>

namespace Simian
{
    class CSprite;
    class VideoClip;
    class GraphicsDevice;

    class SIMIAN_EXPORT CVideo: public EntityComponent
    {
    private:
        CSprite* sprite_;
        VideoClip* videoClip_;
        std::string videoFile_;
        u32 bufferSize_;

        // low level stuff.. this will only work in directx..
        LPDIRECT3DSURFACE9 yuvSurface_;

        static Simian::GraphicsDevice* graphicsDevice_;

        static FactoryPlant<EntityComponent, CVideo> plant_;
    public:
        Simian::VideoClip& Video();

        static Simian::GraphicsDevice& GraphicsDevice();
        static void GraphicsDevice(Simian::GraphicsDevice* val);
    private:
        void onRelease_(DelegateParameter&);
        void onReset_(DelegateParameter&);

        void update_(Simian::DelegateParameter& param);
    public:
        CVideo();

        void OnSharedLoad();
        void OnAwake();
        void OnDeinit();

        void Serialize( FileObjectSubNode& );
        void Deserialize( const FileObjectSubNode& );
    };
}

#endif
