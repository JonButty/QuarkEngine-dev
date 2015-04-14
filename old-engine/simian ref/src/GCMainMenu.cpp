/*************************************************************************/
/*!
\file		GCMainMenu.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCMainMenu.h"
#include "ComponentTypes.h"
#include "GCUIButton.h"
#include "StateLoading.h"
#include "StateGame.h"
#include "EntityTypes.h"
#include "GCOptionsMenu.h"
#include "GCCinematicUI.h"
#include "GCDestructiveUI.h"
#include "GCCredits.h"
#include "GCHowToPlay.h"
#include "GCLevelSelect.h"

#include "SaveManager.h"

#include "Simian/EnginePhases.h"
#include "Simian/CSprite.h"
#include "Simian/Game.h"
#include "Simian/CTransform.h"
#include "Simian/GameStateManager.h"
#include "Simian/Scene.h"
#include "Simian/LogManager.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCMainMenu> GCMainMenu::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_MAINMENU);

    GCMainMenu::GCMainMenu()
        : ignoreInput_(false),
          fading_(false),
          timer_(0.0f)
    {
    }

    //--------------------------------------------------------------------------

    void GCMainMenu::update_( Simian::DelegateParameter& param)
    {
        Simian::EntityUpdateParameter* p;
        param.As(p);

        timer_ += p->Dt;
        if (fading_ && timer_ >= 1.5f)
        {
            if (lastAction_ == "StartButton")
            {
                // Start game
                Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());
                StateLoading::Instance().NextState(StateGame::InstancePtr());
                StateGame::Instance().LevelFile("Data/Scenes/IntroVideo.scn");
            }
            else if (lastAction_ == "ContinueButton")
            {
                // TODO: Add code to continue from save game here

                // Start game
                SaveManager::Instance().LoadCheckpoint(); //--Properly config data
                /*
                Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());
                StateLoading::Instance().NextState(StateGame::InstancePtr());
                StateGame::Instance().LevelFile(data.CurrentLevel);
                */
            }
            else if (lastAction_ == "QuitButton")
                Simian::Game::Instance().Exit();
        }
    }

    void GCMainMenu::disableButtons_()
    {
        for (unsigned i = 0; i < buttons_.size(); ++i)
            buttons_[i]->Enabled(false);
    }

    void GCMainMenu::getUIElement_( UIElement& element, const std::string& name )
    {
        Simian::Entity* entity = ParentEntity()->ChildByName(name);
        entity->ComponentByType(Simian::C_SPRITE, element.Sprite);
        entity->ComponentByType(Simian::C_TRANSFORM, element.Transform);

        element.OriginalSize = element.Sprite->Size();
        element.Height = 0.0f;
    }

    void GCMainMenu::registerTal_( const std::string& name )
    {
        GCUIButton* button;
        Simian::Entity* entity = ParentEntity()->ChildByName(name);
        entity->ComponentByType(GC_UIBUTTON, button);
        button->AddListener(this);
        buttons_.push_back(button);
    }

    void GCMainMenu::registerTalChild_( const std::string& name )
    {
        GCUIButton* button;
        Simian::Entity* entity = ParentEntity()->ChildByName(name);
        entity = entity->Children()[0];
        entity->ComponentByType(GC_UIBUTTON, button);
        button->AddListener(this);
        buttons_.push_back(button);
    }

    void GCMainMenu::closeOptions_( Simian::DelegateParameter& )
    {
        ignoreInput_ = false;
        for (unsigned i = 0; i < buttons_.size(); ++i)
            buttons_[i]->Enabled(true);
    }

    void GCMainMenu::newGameYes_( Simian::DelegateParameter& )
    {
        // TODO: Add code to delete save game.
        SaveManager::Instance().SaveData(SaveManager::SAVE_INVALID);

        ignoreInput_ = true;
        timer_ = 0.0f;
        fading_ = true;
        GCCinematicUI::Instance().FadeOut(Simian::Color(0.0f, 0.0f, 0.0f), 1.0f, -10.0f);
    }

    void GCMainMenu::quitYes_( Simian::DelegateParameter& )
    {
        ignoreInput_ = true;
        timer_ = 0.0f;
        fading_ = true;
        GCCinematicUI::Instance().FadeOut(Simian::Color(0.0f, 0.0f, 0.0f), 1.0f, -10.0f);
    }

    void GCMainMenu::quitNo_( Simian::DelegateParameter& )
    {
        ignoreInput_ = false;
        for (unsigned i = 0; i < buttons_.size(); ++i)
            buttons_[i]->Enabled(true);
    }

    //--------------------------------------------------------------------------

    void GCMainMenu::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, 
            Simian::Delegate::CreateDelegate<GCMainMenu, &GCMainMenu::update_>(this));
    }

    void GCMainMenu::OnAwake()
    {
        // register tal with start button
        registerTal_("StartButton");
        registerTal_("OptionsButton");
        registerTal_("LevelSelectButton");
        registerTal_("QuitButton");

        // register offset buttons
        registerTalChild_("ContinueOffset");
        registerTalChild_("CreditsOffset");
        registerTalChild_("HTPOffset");

        //--Load once!
        SaveManager::Instance().LoadData();

        // TODO: if no save game do this branch
        if (SaveManager::Instance().InitialData().SaveID == SaveManager::SAVE_INVALID)
        {
            Simian::Entity* entity = ParentEntity()->ChildByName("ContinueOffset");
            entity = entity->Children()[0];
            Simian::CSprite* sprite;
            entity->ComponentByType(Simian::C_SPRITE, sprite);
            sprite->Visible(false);
        }

        Simian::Entity* entity = ParentScene()->CreateEntity(E_LEVELSELECT);
        ParentScene()->AddEntity(entity);

        Simian::CTransform* transform = NULL;
        entity->ComponentByType(Simian::C_TRANSFORM, transform);
        transform->Position(Simian::Vector3(5000.0f, 0, 0));

        GCLevelSelect* levelSelect;
        entity->ComponentByType(GC_LEVELSELECT, levelSelect);
        levelSelect->CloseCallback(Simian::Delegate::CreateDelegate<GCMainMenu, &GCMainMenu::closeOptions_>(this));
    }

    void GCMainMenu::OnTriggered( Simian::DelegateParameter& param )
    {
        if (ignoreInput_)
            return;

        GCInputListener::Parameter* p;
        param.As(p);

        lastAction_ = p->Callee->Name();

        // check for which button was pressed
        if (p->Callee->Name() == "StartButton")
        {
            ignoreInput_ = true;
            disableButtons_();

            // TODO: add code to detect existance of save game
            if (SaveManager::Instance().InitialData().SaveID !=
                SaveManager::SAVE_INVALID)
            {
                Simian::Entity* entity = ParentScene()->CreateEntity(E_DESTRUCTIVEUINEWGAME);
                ParentScene()->AddEntity(entity);

                GCDestructiveUI* destructiveUI;
                entity->ComponentByType(GC_DESTRUCTIVEUI, destructiveUI);
                destructiveUI->YesCallbacks() +=
                    Simian::Delegate::CreateDelegate<GCMainMenu, &GCMainMenu::newGameYes_>(this);
                destructiveUI->NoCallbacks() +=
                    Simian::Delegate::CreateDelegate<GCMainMenu, &GCMainMenu::quitNo_>(this);
            }
            else
            {
                // TODO: Add code to delete save game.
                SaveManager::Instance().SaveData(SaveManager::SAVE_INVALID);
                timer_ = 0.0f;
                fading_ = true;
                GCCinematicUI::Instance().FadeOut(Simian::Color(0.0f, 0.0f, 0.0f), 1.0f, -10.0f);
            }
        }
        else if (p->Callee->Name() == "ContinueButton")
        {
            ignoreInput_ = true;
            disableButtons_();
            timer_ = 0.0f;
            fading_ = true;
            GCCinematicUI::Instance().FadeOut(Simian::Color(0.0f, 0.0f, 0.0f), 1.0f, -10.0f);
        }
        else if (p->Callee->Name() == "OptionsButton")
        {
            ignoreInput_ = true;
            disableButtons_();

            // Add options entity
            Simian::Entity* entity = ParentScene()->CreateEntity(E_OPTIONSUI);
            Simian::CTransform* transform;
            entity->ComponentByType(Simian::C_TRANSFORM, transform);
            transform->Position(transform->Position() + Simian::Vector3(0, 0, -5.0));
            ParentScene()->AddEntity(entity);

            GCOptionsMenu* optionsMenu;
            entity->ComponentByType(GC_OPTIONSMENU, optionsMenu);
            optionsMenu->CloseCallback(Simian::Delegate::CreateDelegate<GCMainMenu, &GCMainMenu::closeOptions_>(this));
        }
        else if (p->Callee->Name() == "CreditsButton")
        {
            ignoreInput_ = true;
            disableButtons_();

            // Add options entity
            Simian::Entity* entity = ParentScene()->CreateEntity(E_CREDITSUI);
            Simian::CTransform* transform;
            entity->ComponentByType(Simian::C_TRANSFORM, transform);
            transform->Position(transform->Position() + Simian::Vector3(0, 0, -5.0));
            ParentScene()->AddEntity(entity);

            GCCredits* credits;
            entity->ComponentByType(GC_CREDITSMENU, credits);
            credits->CloseCallback(Simian::Delegate::CreateDelegate<GCMainMenu, &GCMainMenu::closeOptions_>(this));
        }
        else if (p->Callee->Name() == "HTPButton")
        {
            ignoreInput_ = true;
            disableButtons_();

            // Add options entity
            Simian::Entity* entity = ParentScene()->CreateEntity(E_HOWTOPLAYUI);
            Simian::CTransform* transform;
            entity->ComponentByType(Simian::C_TRANSFORM, transform);
            transform->Position(transform->Position() + Simian::Vector3(0, 0, -5.0));
            ParentScene()->AddEntity(entity);

            GCHowToPlay* htp;
            entity->ComponentByType(GC_HOWTOPLAYMENU, htp);
            htp->CloseCallback(Simian::Delegate::CreateDelegate<GCMainMenu, &GCMainMenu::closeOptions_>(this));
        }
        else if (p->Callee->Name() == "QuitButton")
        {
            ignoreInput_ = true;
            disableButtons_();

            Simian::Entity* entity = ParentScene()->CreateEntity(E_DESTRUCTIVEUI);
            ParentScene()->AddEntity(entity);

            GCDestructiveUI* destructiveUI;
            entity->ComponentByType(GC_DESTRUCTIVEUI, destructiveUI);
            destructiveUI->YesCallbacks() += Simian::Delegate::CreateDelegate<GCMainMenu, &GCMainMenu::quitYes_>(this);
            destructiveUI->NoCallbacks() += Simian::Delegate::CreateDelegate<GCMainMenu, &GCMainMenu::quitNo_>(this);
        }
        else if (p->Callee->Name() == "LevelSelectButton")
        {
            ignoreInput_ = true;
            disableButtons_();

            Simian::Entity* entity = ParentScene()->EntityByName("LevelSelect");
            Simian::CTransform* transform = NULL;
            entity->ComponentByType(Simian::C_TRANSFORM, transform);
            transform->Position(Simian::Vector3(0, 0, 0));
        }
    }

    void GCMainMenu::Serialize( Simian::FileObjectSubNode& )
    {
    }

    void GCMainMenu::Deserialize( const Simian::FileObjectSubNode& )
    {
    }
}
