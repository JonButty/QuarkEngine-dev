/*************************************************************************/
/*!
\file		GCLevelSelect.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/10/12
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCLevelSelect.h"
#include "GCUIButton.h"
#include "ComponentTypes.h"
#include "SaveManager.h"
#include "StateGame.h"
#include "StateLoading.h"

#include "Simian\EnginePhases.h"
#include "Simian\EngineComponents.h"
#include "Simian\Scene.h"
#include "Simian\Vector3.h"
#include "Simian\Game.h"
#include "Simian\CTransform.h"
#include "Simian\CSprite.h"
#include "Simian\Math.h"
#include "Simian\LogManager.h"

#include <sstream>

namespace Descension
{
    //--Number of buttons on the top
    Simian::u32 GCLevelSelect::BUTTON_COUNT = 11;

    Simian::FactoryPlant<Simian::EntityComponent, GCLevelSelect> GCLevelSelect::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_LEVELSELECT);

    GCLevelSelect::GCLevelSelect()
        : transform_(0)
    {
    }

    //--------------------------------------------------------------------------

    void GCLevelSelect::CloseCallback( const Simian::Delegate& val )
    {
        closeCallback_ = val;
    }

    //--------------------------------------------------------------------------

    void GCLevelSelect::registerButton_( const std::string& name )
    {
        // find buttons and register them
        Simian::Entity* entity = ParentEntity()->ChildByName(name);
        GCUIButton* button;
        entity->ComponentByType(GC_UIBUTTON, button);
        button->AddListener(this);
    }

    void GCLevelSelect::update_(Simian::DelegateParameter& param)
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt * 2;

        for (Simian::u32 i=1; i<BUTTON_COUNT; ++i)
        {
            GCUIButton *btn = NULL;
            Simian::CSprite *preview = NULL;
            bVec_[i]->ComponentByType(GC_UIBUTTON, btn);

            if (btn->OnHover())
            {
                cVec_[i]->ComponentByType(Simian::C_SPRITE, preview);
                Simian::Color col = preview->Color();
                col.A(Simian::Math::Clamp<Simian::f32>
                    (preview->Color().A()+dt, 0.0f, 1.0f));
                preview->Color(col);
            }
            else
            {
                cVec_[i]->ComponentByType(Simian::C_SPRITE, preview);
                Simian::Color col = preview->Color();
                col.A(Simian::Math::Clamp<Simian::f32>
                    (preview->Color().A()-dt, 0.0f, 1.0f));
                preview->Color(col);
            }
        }
    }

    void GCLevelSelect::SetAllButtons(void)
    {
        for (Simian::u32 i=0; i<BUTTON_COUNT; ++i)
        {
            Simian::CSprite* btn = NULL;
            GCUIButton* button = NULL;
            bVec_[i]->ComponentByType(Simian::C_SPRITE, btn);
            bVec_[i]->ComponentByType(GC_UIBUTTON, button);
            button->AddListener(this);

            if (SaveManager::Instance().InitialData().LevelsUnlocked[i])
                btn->Visible(true);
            else
                btn->Visible(false);
        }
    }

    //--------------------------------------------------------------------------

    void GCLevelSelect::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);

        registerButton_("BackButton");

        for (Simian::u32 i=0; i<BUTTON_COUNT; ++i)
        {
            Simian::Entity* entity;
            std::stringstream sstr;

            sstr << "A" << i;
            entity = ParentEntity()->ChildByName(sstr.str());
            aVec_.push_back(entity);
            
            sstr.str("");
            sstr << "B" << i;
            entity = ParentEntity()->ChildByName(sstr.str());
            bVec_.push_back(entity);

            sstr.str("");
            sstr << "C" << i;
            entity = ParentEntity()->ChildByName(sstr.str());
            cVec_.push_back(entity);
        }
        
        //--Everything is invisible
        for (Simian::u32 i=1; i<BUTTON_COUNT; ++i)
        {
            Simian::CSprite *preview = NULL;
            cVec_[i]->ComponentByType(Simian::C_SPRITE, preview);
            Simian::Color col = preview->Color();
            col.A(0.0f);
            preview->Color(col);
        }

        //--Load from SaveManager
        SetAllButtons();
    }

    void GCLevelSelect::OnTriggered( Simian::DelegateParameter& param )
    {
        GCInputListener::Parameter* p;
        param.As(p);

        if (p->Callee->Name() == "BackButton")
        {
            // remove!
            //if (ParentEntity()->ParentEntity())
            //    ParentEntity()->ParentEntity()->RemoveChild(ParentEntity());
            //else
            //    ParentScene()->RemoveEntity(ParentEntity());

            Simian::CTransform* transform = NULL;
            ComponentByType(Simian::C_TRANSFORM, transform);
            transform->Position(Simian::Vector3(5000.0f, 0, 0));

            // do the callback
            closeCallback_();
        }
        else
            LoadLevel(p->Callee->Name());
    }

    void GCLevelSelect::LoadLevel(const std::string& callee)
    {
        std::stringstream sstr;
        sstr << callee.substr(1, callee.length()-1);
        Simian::u32 id;
        sstr >> id;

        std::string filePath = StateGame::GetFileFromIndex(id);
        SaveManager::Instance().CopyInitialToCurrent();
        SaveManager::Instance().CurrentData().SaveID = SaveManager::SAVE_NORMAL;

        Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());
        StateLoading::Instance().NextState(StateGame::InstancePtr());
        StateGame::Instance().LevelFile(filePath);
    }

    void GCLevelSelect::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate
            <GCLevelSelect, &GCLevelSelect::update_>(this));
    }

    void GCLevelSelect::Serialize( Simian::FileObjectSubNode& )
    {
    }

    void GCLevelSelect::Deserialize( const Simian::FileObjectSubNode& )
    {
    }
}
