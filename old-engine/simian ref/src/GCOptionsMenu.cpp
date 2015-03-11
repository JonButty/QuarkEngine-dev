/*************************************************************************/
/*!
\file		GCOptionsMenu.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCOptionsMenu.h"
#include "GCUIButton.h"
#include "ComponentTypes.h"
#include "GCUICheckbox.h"
#include "GCUISlider.h"

#include "Simian\EnginePhases.h"
#include "Simian\EngineComponents.h"
#include "Simian\Scene.h"
#include "Simian\Vector3.h"
#include "Simian\Game.h"
#include "Simian\CTransform.h"
#include "Simian\GraphicsDevice.h"
#include "Simian\CBitmapText.h"
#include "Simian\LogManager.h"
#include "Simian\AudioManager.h"

#include <algorithm>
#include <functional>
#include <string>
#include "Simian\Viewport.h"

// user files should be stored relative to this path
extern std::string USER_FILES_PATH;

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCOptionsMenu> GCOptionsMenu::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_OPTIONSMENU);

    GCOptionsMenu::GCOptionsMenu()
        : transform_(0),
          bitmapText_(0),
          fullscreenCheckbox_(0),
          musicSlider_(0),
          soundSlider_(0),
          currentResolution_(0)
    {
    }

    //--------------------------------------------------------------------------

    void GCOptionsMenu::CloseCallback( const Simian::Delegate& val )
    {
        closeCallback_ = val;
    }

    //--------------------------------------------------------------------------

    void GCOptionsMenu::update_( Simian::DelegateParameter& )
    {
        // find 0.8 of the scene
        Simian::f32 height = Simian::Game::Instance().Size().Y() * 0.9f;

        // scale to be 0.8 of screen height
        transform_->Scale(Simian::Vector3(height/1024.0f, height/1024.0f, 1.0f));

        // update resolution text
        std::stringstream ss;
        ss << resolutions_[currentResolution_].X() << " x " << 
              resolutions_[currentResolution_].Y();
        bitmapText_->Text(ss.str());

        // set the music volume
        Simian::AudioManager::Instance().MusicVolume(musicSlider_->Value());

        // set the sound volume
        Simian::AudioManager::Instance().SoundVolume(soundSlider_->Value());
    }

    void GCOptionsMenu::registerButton_( const std::string& name )
    {
        // find buttons and register them
        Simian::Entity* entity = ParentEntity()->ChildByName(name);
        GCUIButton* button;
        entity->ComponentByType(GC_UIBUTTON, button);
        button->AddListener(this);
    }

    void GCOptionsMenu::loadOptions_()
    {
        // load resolution
        struct : public std::binary_function<Simian::Vector2, Simian::Vector2, bool>
        {
        public:
            bool operator()(const Simian::Vector2& a, const Simian::Vector2& b) const
            {
                return (a.X() == b.X() && a.Y() == b.Y());
            }
        } CompareResolution;
        // find the resolution that matches
        std::vector<Simian::Vector2>::iterator i = std::find_if(
            resolutions_.begin(), resolutions_.end(), std::bind2nd(CompareResolution, Simian::Game::Instance().Size()));
        currentResolution_ = (i - resolutions_.begin()) % resolutions_.size();

        // load fullscreen
        fullscreenCheckbox_->Checked(Simian::Game::Instance().Fullscreen());

        // load music volume
        musicSlider_->Value(Simian::AudioManager::Instance().MusicVolume());

        // load sound volume
        soundSlider_->Value(Simian::AudioManager::Instance().SoundVolume());
    }

    void GCOptionsMenu::saveOptions_()
    {
        bool changeGraphics = false;

        Simian::Vector2& newResolution = resolutions_[currentResolution_];
        if (Simian::Game::Instance().Size().X() != newResolution.X() ||
            Simian::Game::Instance().Size().Y() != newResolution.Y())
            changeGraphics = true;
        
        if (fullscreenCheckbox_->Checked() != Simian::Game::Instance().Fullscreen())
            changeGraphics = true;

        Simian::Game::Instance().Size(newResolution);
        Simian::Game::Instance().Fullscreen(fullscreenCheckbox_->Checked());

        if (changeGraphics)
        {
            Simian::Game::Instance().GraphicsDevice().ApplyChanges();
            Simian::Game::Instance().Viewport().Set();
        }

        Simian::Game::Instance().SaveSettings(USER_FILES_PATH + "Settings.xml");
    }

    //--------------------------------------------------------------------------

    void GCOptionsMenu::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, 
            Simian::Delegate::CreateDelegate<GCOptionsMenu, &GCOptionsMenu::update_>(this));
    }

    void GCOptionsMenu::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);

        Simian::Entity* entity = ParentEntity()->ChildByName("ResolutionText");
        entity->ComponentByType(Simian::C_BITMAPTEXT, bitmapText_);

        entity = ParentEntity()->ChildByName("FullscreenCheckBox");
        entity->ComponentByType(GC_UICHECKBOX, fullscreenCheckbox_);

        entity = ParentEntity()->ChildByName("MusicSlider");
        entity->ComponentByType(GC_UISLIDER, musicSlider_);

        entity = ParentEntity()->ChildByName("SoundSlider");
        entity->ComponentByType(GC_UISLIDER, soundSlider_);

        // get all resolutions
        Simian::Game::Instance().GraphicsDevice().EnumerateResolutions(resolutions_);

        registerButton_("LeftButton");
        registerButton_("RightButton");
        registerButton_("ApplyButton");
        registerButton_("CancelButton");

		// force a resize
        // find 0.8 of the scene
        Simian::f32 height = Simian::Game::Instance().Size().Y() * 0.9f;

        // scale to be 0.8 of screen height
        transform_->Scale(Simian::Vector3(height/1024.0f, height/1024.0f, 1.0f));
		transform_->RecomputeWorld();
    }

    void GCOptionsMenu::OnInit()
    {
        // wait for all ui components to be loaded before loading the options
        loadOptions_();
    }

    void GCOptionsMenu::OnTriggered( Simian::DelegateParameter& param )
    {
        GCInputListener::Parameter* p;
        param.As(p);

        if (p->Callee->Name() == "ApplyButton")
        {
            saveOptions_();
        }
        else if (p->Callee->Name() == "CancelButton")
        {
            // remove!
            if (ParentEntity()->ParentEntity())
                ParentEntity()->ParentEntity()->RemoveChild(ParentEntity());
            else
                ParentScene()->RemoveEntity(ParentEntity());

            // do the callback
            closeCallback_();
        }
        else if (p->Callee->Name() == "LeftButton")
        {
            if (currentResolution_ == 0)
                currentResolution_ = resolutions_.size() - 1;
            else
                --currentResolution_;
        }
        else if (p->Callee->Name() == "RightButton")
        {
            currentResolution_ = (currentResolution_ + 1) % resolutions_.size();
        }
    }

    void GCOptionsMenu::Serialize( Simian::FileObjectSubNode& )
    {
    }

    void GCOptionsMenu::Deserialize( const Simian::FileObjectSubNode& )
    {
    }
}
