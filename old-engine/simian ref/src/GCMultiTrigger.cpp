/*************************************************************************/
/*!
\file		GCMultiTrigger.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/10/12
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*************************************************************************/
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/CTransform.h"
#include "Simian/Entity.h"
#include "Simian/Scene.h"

#include "EntityTypes.h"
#include "ComponentTypes.h"

#include "GCMultiTrigger.h"
#include "GCEventScript.h"
#include "GCEditor.h"

#include <iostream>

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCMultiTrigger>
        GCMultiTrigger::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_MULTITRIGGER);
    
    GCMultiTrigger::GCMultiTrigger()
    {  }

    //--------------------------------------------------------------------------
    void GCMultiTrigger::update_( Simian::DelegateParameter& )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
    }

    //--------------------------------------------------------------------------
    void GCMultiTrigger::OnSharedLoad()
    {
        //--
    }

    void GCMultiTrigger::OnAwake()
    {
        //--
    }

    void GCMultiTrigger::OnInit()
    {
        if (triggerPtr_.size() > 0)
            triggerPtr_.clear();

        for (Simian::u32 i=0; i<triggerList_.size(); ++i)
        {
            Simian::Entity* trigger = ParentScene()->EntityByNameRecursive
                (triggerList_[i]);
            if (trigger)
            {
                GCEventScript* script;
                trigger->ComponentByType(GC_EVENTSCRIPT, script);
                if (script)
                    triggerPtr_.push_back(script);
            }
        }
    }

    void GCMultiTrigger::OnDeinit()
    {
        //--nothing
    }

    void GCMultiTrigger::OnTriggered(Simian::DelegateParameter&)
    {
        for (Simian::u32 i=0; i<triggerPtr_.size(); ++i)
            triggerPtr_[i]->OnTriggered(Simian::Delegate::NoParameter);
    }

    void GCMultiTrigger::Serialize( Simian::FileObjectSubNode& data)
    {
        Simian::FileArraySubNode* targetArray = data.AddArray("TriggerTargets");

        for (Simian::u32 i=0; i<triggerList_.size(); ++i)
        {
            Simian::FileObjectSubNode* node = targetArray->AddObject("Entity");
            node->AddData("Name", triggerList_[i]);
        }
    }

    void GCMultiTrigger::Deserialize( const Simian::FileObjectSubNode& data)
    {
        const Simian::FileArraySubNode* targetArray = data.Array("TriggerTargets");
        std::string name;

        if (triggerList_.size() > 0)
            triggerList_.clear();

        for (Simian::u32 i=0; i<targetArray->Size(); ++i)
        {
            const Simian::FileObjectSubNode* node = targetArray->Object(i);
            node->Data("Name", name);
            triggerList_.push_back(name);
        }
    }
}
