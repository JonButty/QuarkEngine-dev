/*************************************************************************/
/*!
\file		GCCinematicEscape.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/09/30
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*************************************************************************/
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/Entity.h"
#include "Simian/Keyboard.h"
#include "Simian/Scene.h"

#include "EntityTypes.h"
#include "ComponentTypes.h"

#include "GCUIButton.h"
#include "GCCinematicEscape.h"
#include "GCEditor.h"
#include "GCDescensionCamera.h"
#include "GCPlayerLogic.h"
#include "GCCinematicUI.h"
#include "GCEventScript.h"

#include "Simian/CCamera.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCCinematicEscape> GCCinematicEscape::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_CINEMATICESCAPE);

    bool GCCinematicEscape::canEscape_ = false;

    GCCinematicEscape::GCCinematicEscape() : backBtn_(NULL)
    {  }

    //--------------------------------------------------------------------------
    void GCCinematicEscape::CanEscape(bool val)
    {
        canEscape_ = val;
    }

    //--------------------------------------------------------------------------
    void GCCinematicEscape::update_( Simian::DelegateParameter& )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        if (canEscape_ && !backBtn_)
        {
            backBtn_ = ParentScene()->CreateEntity(E_BACKBUTTON);
            ParentScene()->AddEntity(backBtn_);

            GCUIButton* button = NULL;
            backBtn_->ComponentByType(GC_UIBUTTON, button);
            button->AddListener(this);
        } else if (!canEscape_ && backBtn_) {
            ParentScene()->RemoveEntity(backBtn_);
            backBtn_ = NULL;
        }

        if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_ESCAPE) &&
           canEscape_)
        {
            OnTriggered(Simian::Delegate::NoParameter);
        }
    }

    //--------------------------------------------------------------------------
    void GCCinematicEscape::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate
            <GCCinematicEscape, &GCCinematicEscape::update_>(this));
    }

    void GCCinematicEscape::OnAwake()
    {
        canEscape_ = false;
    }

    void GCCinematicEscape::OnInit()
    {
        //--
    }

    void GCCinematicEscape::OnDeinit()
    {
        //--
    }

    void GCCinematicEscape::OnTriggered(Simian::DelegateParameter&) //--script event
    {
        Simian::Entity* exiter = ParentScene()->EntityByNameRecursive(triggerTarget_);
        if (exiter)
        {
            canEscape_ = false;
            GCEventScript* script;
            exiter->ComponentByType(GC_EVENTSCRIPT, script);
            script->OnTriggered(Simian::Delegate::NoParameter);
        }
        if (backBtn_)
        {
            ParentScene()->RemoveEntity(backBtn_);
            backBtn_ = NULL;
        }
    }

    void GCCinematicEscape::Serialize( Simian::FileObjectSubNode& data)
    {
        data.AddData("TriggerTarget", triggerTarget_);
    }

    void GCCinematicEscape::Deserialize( const Simian::FileObjectSubNode& data)
    {
        data.Data("TriggerTarget", triggerTarget_, triggerTarget_);
    }
}
