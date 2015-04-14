/************************************************************************/
/*!
\file		FrameRateController.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_FRAMERATECONTROLLER_H_
#define SIMIAN_FRAMERATECONTROLLER_H_

#include "SimianPlatform.h"

namespace Simian
{
    class Material;
    class MaterialCache;
    class BitmapFont;
    class BitmapText;
    class GraphicsDevice;

    class SIMIAN_EXPORT FrameRateController
    {
    private:
        u32 frameCounter_;
        f32 timePassed_;
        u32 totalFrames_;
        f32 totalTime_;
        u32 fps_;
        
        Material* textMaterial_;
        BitmapFont* debugFont_;
        BitmapText* fpsText_;
        BitmapText* averageFpsText_;

        static Simian::GraphicsDevice* graphicsDevice_;
        static Simian::MaterialCache* materialCache_;

        bool hidden_;
    public:
        u32 Fps() const;
        f32 AverageFps() const;

        static Simian::MaterialCache* MaterialCache();
        static void MaterialCache(Simian::MaterialCache* val);

        static Simian::GraphicsDevice* GraphicsDevice();
        static void GraphicsDevice(Simian::GraphicsDevice* val);
    public:
        FrameRateController();

        void Load();
        void Unload();

        void Update(float dt);
        void Render();
    };
}

#endif
