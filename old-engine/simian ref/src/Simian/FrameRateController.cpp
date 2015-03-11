/************************************************************************/
/*!
\file		FrameRateController.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/FrameRateController.h"
#include "Simian/MaterialCache.h"
#include "Simian/Material.h"
#include "Simian/BitmapFont.h"
#include "Simian/BitmapText.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/Keyboard.h"

#include <sstream>

namespace Simian
{
    Simian::MaterialCache* FrameRateController::materialCache_ = 0;
    Simian::GraphicsDevice* FrameRateController::graphicsDevice_ = 0;

    FrameRateController::FrameRateController()
        : frameCounter_(0),
          timePassed_(0.0f),
          totalFrames_(0),
          totalTime_(0.0f),
          debugFont_(0),
          fpsText_(0),
          averageFpsText_(0),
          hidden_(true)
    {
    }

    //--------------------------------------------------------------------------

    Simian::u32 FrameRateController::Fps() const
    {
        return fps_;
    }

    Simian::f32 FrameRateController::AverageFps() const
    {
        return totalFrames_/totalTime_;
    }

    Simian::MaterialCache* FrameRateController::MaterialCache()
    {
        return materialCache_;
    }

    void FrameRateController::MaterialCache( Simian::MaterialCache* val )
    {
        materialCache_ = val;
    }

    Simian::GraphicsDevice* FrameRateController::GraphicsDevice()
    {
        return graphicsDevice_;
    }

    void FrameRateController::GraphicsDevice( Simian::GraphicsDevice* val )
    {
        graphicsDevice_ = val;
    }

    //--------------------------------------------------------------------------

    void FrameRateController::Load()
    {
        textMaterial_ = materialCache_->Load("Materials/inconsolata.mat");

        debugFont_ = new BitmapFont(graphicsDevice_);
        debugFont_->LoadImmediate("Fonts/inconsolata.fnt");
        
        debugFont_->CreateBitmapText(fpsText_, 128);
        debugFont_->CreateBitmapText(averageFpsText_, 128);

        fpsText_->Alignment(Simian::Vector2(0.0f, -1.0f));
        averageFpsText_->Alignment(Simian::Vector2(0.0f, -1.0f));
    }

    void FrameRateController::Unload()
    {
        debugFont_->DestroyBitmapText(averageFpsText_);
        averageFpsText_ = 0;
        debugFont_->DestroyBitmapText(fpsText_);
        fpsText_ = 0;

        debugFont_->Unload();
        delete debugFont_;
        debugFont_ = 0;
    }

    void FrameRateController::Update( float dt )
    {
        timePassed_ += dt;
        totalTime_ += dt;
        ++totalFrames_;
        ++frameCounter_;

        if (timePassed_ > 1.0f)
        {
            timePassed_ = 0;
            fps_ = frameCounter_;
            frameCounter_ = 0;
        }

        if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_F11))
            hidden_ = !hidden_;

        if (hidden_)
            return;

        std::stringstream ss;
        ss << "Fps: " << Fps();
        fpsText_->Text(ss.str());
        ss.str("");
        ss << "Average Fps: " << AverageFps();
        averageFpsText_->Text(ss.str());
        ss.str("");
    }

    void FrameRateController::Render()
    {
        if (hidden_)
            return;

        // get half width/height
        f32 hwidth = graphicsDevice_->Size().X() * 0.5f;
        f32 hheight = graphicsDevice_->Size().Y() * 0.5f;

        textMaterial_->Set();
        for (u32 i = 0; i < textMaterial_->Size(); ++i)
        {
            // compute world
            Simian::Matrix world = 
                Simian::Matrix::Translation(-hwidth, hheight, 0);

            // set pass
            (*textMaterial_)[i].Set();

            // draw fps
            graphicsDevice_->World(world);
            debugFont_->Draw(fpsText_);

            // draw average fps
            world = Simian::Matrix::Translation(0, -20, 0) * world;
            graphicsDevice_->World(world);
            debugFont_->Draw(averageFpsText_);

            // unset pass
            (*textMaterial_)[i].Unset();
        }
        textMaterial_->Unset();
    }
}
