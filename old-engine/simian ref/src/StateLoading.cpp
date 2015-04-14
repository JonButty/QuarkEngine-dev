/*************************************************************************/
/*!
\file		StateLoading.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "StateLoading.h"
#include "StateGame.h"

#include "Simian/Game.h"
#include "Simian/MaterialCache.h"
#include "Simian/Material.h"
#include "Simian/Camera.h"
#include "Simian/RenderQueue.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/RenderTexture.h"
#include "Simian/LogManager.h"
#include "Simian/GameFactory.h"
#include "Simian/Scene.h"
#include "Simian/TextureCache.h"
#include "Simian/ModelCache.h"
#include "Simian/Model.h"
#include "Simian/BitmapFont.h"
#include "Simian/BitmapText.h"
#include "Simian/Math.h"

#define STATELOADING_RELOAD_RESOURCES 0

namespace Descension
{
    static const Simian::f32 SPINNER_SIZE = 35.0f;
    static const Simian::f32 SPIN_SPEED = 120.0f;

    StateLoading::StateLoading()
        : camera_(0),
          nextState_(StateGame::InstancePtr()),
          scene_(0),
          loadingLayoutDone_(false),
          loadingFilesDone_(false),
          loadingResourcesLock_(false),
          loadedResources_(false),
		  crystalSpin_(0.0f),
          backgroundColor_(0.1f, 0.05f, 0.025f, 1.0f),
          timer_(0.0f),
          lastTime_(0.0f),
		  hideLoadScreen_(true)
    {
    }

    //--------------------------------------------------------------------------

    void StateLoading::NextState( Simian::GameState* val )
    {
        nextState_ = val;
    }

    void StateLoading::BackgroundColor( const Simian::Color& val )
    {
        backgroundColor_ = val;
    }

    void StateLoading::Timer( Simian::f32 val )
    {
        timer_ = val;
    }

    void StateLoading::HideLoadScreen( bool val )
    {
        hideLoadScreen_ = val;
    }

    //--------------------------------------------------------------------------

    void StateLoading::loadingThreadEntry_( Simian::DelegateParameter& )
    {
        // load the layout
        scene_->LoadLayout(StateGame::Instance().LevelFile());
        scene_->LoadFiles();
        loadingLayoutDone_.Set(true);

        // synchronize while main thread loads resources
        loadingResourcesLock_.Acquire(); // stop and wait
        loadingResourcesLock_.Release(); // just release immediately

        // load the texture/model cache
        Simian::Game::Instance().TextureCache().LoadFiles();
        Simian::Game::Instance().ModelCache().LoadFiles();
        loadingFilesDone_.Set(true);
    }

    void StateLoading::progress_( Simian::DelegateParameter& )
    {
        Simian::f32 ct = Simian::OS::Instance().GetOSTicks();
        Simian::f32 dt = lastTime_ ? ct - lastTime_ : 0.0f;
        lastTime_ = ct;
        updateSpinner_(dt);
        Simian::Game::Instance().DrawFrame();
    }

    void StateLoading::draw_( Simian::DelegateParameter& )
    {
        Simian::Game::Instance().GraphicsDevice().Draw(
            Simian::Game::Instance().FullscreenQuad(),
            Simian::GraphicsDevice::PT_TRIANGLESTRIP, 2);
    }
	
    void StateLoading::drawText_( Simian::DelegateParameter& )
    {
		text_->Draw(0);
    }

    void StateLoading::updateSpinner_( Simian::f32 dt )
    {
		if (hideLoadScreen_)
			return;

        crystalSpin_ += SPIN_SPEED * dt;

        // draw the background
        Simian::Vector2 texSize = 0.0036f * (*background_)[0].TextureStages()[0].Texture()->Size();
        Simian::Game::Instance().RenderQueue().AddRenderObject(Simian::RenderObject(
            *background_, Simian::RenderQueue::RL_SCENE, 
            Simian::Matrix::Scale(texSize.X() * 1.7778f, texSize.Y(), 1.0f),
            Simian::Delegate::CreateDelegate<StateLoading, &StateLoading::draw_>(this), 0));

		// draw the crystal
		crystalModel_->Draw(Simian::Game::Instance().RenderQueue(), *crystalMaterial_, 
			Simian::Matrix::Translation(0.0f, -0.25f, 0.0f) * 
			Simian::Matrix::RotationAxisAngle(Simian::Vector3::UnitY, Simian::Degree(crystalSpin_)), 
			Simian::RenderQueue::RL_SCENE);

        // draw the title
        texSize = 0.0018f * (*titleMaterial_)[0].TextureStages()[0].Texture()->Size();
        Simian::Game::Instance().RenderQueue().AddRenderObject(Simian::RenderObject(
            *titleMaterial_, Simian::RenderQueue::RL_SCENE, 
            Simian::Matrix::Translation(0, 2.0f, 0)*Simian::Matrix::Scale(texSize.X(), texSize.Y(), 1.0f),
            Simian::Delegate::CreateDelegate<StateLoading, &StateLoading::draw_>(this), 0));

		// draw the hintbox
		texSize = 0.007f * (*hintBoxMaterial_)[0].TextureStages()[0].Texture()->Size();
		Simian::Game::Instance().RenderQueue().AddRenderObject(Simian::RenderObject(
            *hintBoxMaterial_, Simian::RenderQueue::RL_SCENE, 
			Simian::Matrix::Translation(0, -2.5f, 0)*Simian::Matrix::Scale(texSize.X(), texSize.Y(), 1.0f),
            Simian::Delegate::CreateDelegate<StateLoading, &StateLoading::draw_>(this), 0));
		
		// draw the text material
		const Simian::f32 TEXT_SCALE = 0.004f;
		Simian::Game::Instance().RenderQueue().AddRenderObject(Simian::RenderObject(
			*fontMaterial_, Simian::RenderQueue::RL_SCENE, 
			Simian::Matrix::Translation(0.0f, -2.5f, -0.01f)*Simian::Matrix::Scale(TEXT_SCALE, TEXT_SCALE, TEXT_SCALE),
            Simian::Delegate::CreateDelegate<StateLoading, &StateLoading::drawText_>(this), 0));

        // submit loading screen to render queue then change the state
        /*Simian::Game::Instance().RenderQueue().AddRenderObject(Simian::RenderObject(
            *material_, Simian::RenderQueue::RL_SCENE, Simian::Matrix::Scale(hwidth, hheight, 1.0f),
            Simian::Delegate::CreateDelegate<StateLoading, &StateLoading::draw_>(this), 0));

        Simian::Game::Instance().RenderQueue().AddRenderObject(Simian::RenderObject(
            *spinner_, Simian::RenderQueue::RL_SCENE, Simian::Matrix::Translation(0.0f, 0.0f, -1.0f) * 
            Simian::Matrix::RotationAxisAngle(Simian::Vector3::UnitZ, Simian::Degree(spinnerRotation_)) * 
            Simian::Matrix::Scale(SPINNER_SIZE, SPINNER_SIZE, 1.0f),
            Simian::Delegate::CreateDelegate<StateLoading, &StateLoading::draw_>(this), 0));*/
    }

    //--------------------------------------------------------------------------

    void StateLoading::OnEnter()
    {
    }

    void StateLoading::OnLoad()
    {
        // create the scene
        Simian::GameFactory::Instance().CreateScene(scene_);

        loadingLayoutDone_.Set(false);
        loadingFilesDone_.Set(false);
        loadingResourcesLock_.Acquire();
        loadedResources_ = false;

        loadingThread_.EntryPoint(Simian::Delegate::CreateDelegate<StateLoading, &StateLoading::loadingThreadEntry_>(this));
        loadingThread_.Run();

		// load required resources
		crystalMaterial_ = Simian::Game::Instance().MaterialCache().Load("Materials/Loading_ParagonCrystal.mat");
		crystalModel_ = Simian::Game::Instance().ModelCache().Load("Models/Prop_ParagonCrystal.dae");
		titleMaterial_ = Simian::Game::Instance().MaterialCache().Load("Materials/Loading_Title.mat");
		fontMaterial_ = Simian::Game::Instance().MaterialCache().Load("Materials/hightower.mat");
        hintBoxMaterial_ = Simian::Game::Instance().MaterialCache().Load("Materials/Loading_HintBox.mat");
        background_ = Simian::Game::Instance().MaterialCache().Load("Materials/Loading_Background.mat");
        Simian::Game::Instance().LoadCaches();

		// text resources
		font_ = new Simian::BitmapFont(&Simian::Game::Instance().GraphicsDevice());
		font_->LoadFile("Fonts/hightower.fnt");
		font_->CreateBitmapText(text_);

		// load the hints file
		Simian::DataFileIODefault hintsFile;
		hintsFile.Read("hints.xml");
		Simian::FileArraySubNode* hints = hintsFile.Root()->Array("Hints");
		Simian::s32 index = Simian::Math::Random(0, static_cast<Simian::s32>(hints->Size()));
		text_->Text(hints->Data(index)->AsString());
		text_->Alignment(Simian::Vector2(-0.5f, -0.5f));
		hintsFile.Close();

        Simian::Game::Instance().GraphicsDevice().CreateRenderTexture(renderTexture_);
        renderTexture_->LoadImmediate(Simian::Game::Instance().Size());

        camera_ = new Simian::Camera(&Simian::Game::Instance().GraphicsDevice());
        camera_->AddLayer(Simian::RenderQueue::RL_SCENE);
		camera_->Projection(Simian::Matrix::PerspectiveFOV(Simian::Degree(60.0f), 
			Simian::Game::Instance().Size().X()/Simian::Game::Instance().Size().Y(),
            0.1f, 10000.0f));
        camera_->View(Simian::Matrix::LookAt(
            Simian::Vector3(0.0f, 0.0f, -5.0f),
            Simian::Vector3(0.0f, 0.0f, 0.0f),
            Simian::Vector3::UnitY));
        camera_->RenderTexture(renderTexture_);
		camera_->ClearColor(backgroundColor_);
        Simian::Game::Instance().RenderQueue().AddCamera(camera_);

        // reset animation vars
        lastTime_ = 0.0f;

        // set progress hooks
        Simian::Game::Instance().TextureCache().ProgressCallback(
            Simian::Delegate::CreateDelegate<StateLoading, &StateLoading::progress_>(this));
        Simian::Game::Instance().ModelCache().ProgressCallback(
            Simian::Delegate::CreateDelegate<StateLoading, &StateLoading::progress_>(this));
    }

    void StateLoading::OnInit()
    {
    }

    void StateLoading::OnUpdate( Simian::f32 dt )
    {
        updateSpinner_(dt);

        timer_ += dt;

        if (loadingLayoutDone_.Get() && !loadedResources_)
        {
            // load all main thread resources
            scene_->LoadResources();
            loadedResources_ = true;
            loadingResourcesLock_.Release();
        }

        if (loadingFilesDone_.Get() && timer_ > 1.0f)
        {
            // load unthreadable stuff
            Simian::Game::Instance().LoadCaches();

            // pass scene to state game
            StateGame::Instance().Scene(scene_);

			// set it so that the proper loading screen is used the next time.
			hideLoadScreen_ = false;
            backgroundColor_ = Simian::Color(0.1f, 0.05f, 0.025f, 1.0f);
            timer_ = 0.0f;

            // change to the next state
#if 1
            Simian::Game::Instance().GameStateManager().ChangeState(nextState_);
#endif
        }
    }

    void StateLoading::OnDeinit()
    {
    }

    void StateLoading::OnUnload()
    {
        // unset progress hooks
        Simian::Game::Instance().TextureCache().ProgressCallback(Simian::Delegate());
        Simian::Game::Instance().ModelCache().ProgressCallback(Simian::Delegate());

		font_->DestroyBitmapText(text_);
		font_->Unload();
		delete font_;
		font_ = 0;

        renderTexture_->Unload();
        Simian::Game::Instance().GraphicsDevice().DestroyRenderTexture(renderTexture_);

        Simian::Game::Instance().RenderQueue().RemoveCamera(camera_);
        delete camera_;
        camera_ = 0;

#if STATELOADING_RELOAD_RESOURCES == 1
        Simian::Game::Instance().UnloadCaches();
#endif
    }

    void StateLoading::OnExit()
    {
    }
}
