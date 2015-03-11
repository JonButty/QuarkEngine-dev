/*************************************************************************/
/*!
\file		GCTrigger.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCTrigger.h"

#include "Simian/Scene.h"
#include "GCInputListener.h"

namespace Descension
{
    GCTrigger::GCTrigger()
    {
    }

    //--------------------------------------------------------------------------

    void GCTrigger::OnAwake()
    {
        // resolve tals
        for (Simian::u32 i = 0; i < tals_.size(); ++i)
        {
            // skip resolved tals
            if (tals_[i].listener_)
                continue;

            Simian::Entity* ent = ParentEntity();
            if (tals_[i].targetName_ != "")
                ent = ParentScene()->EntityByNameRecursive(tals_[i].targetName_);
            if (!ent)
                continue;

            // if no component just continue
            if (!Simian::GameFactory::Instance().ComponentTable().HasValue(tals_[i].listenerName_))
                continue;

            // find component id
            Simian::u32 componentType = Simian::GameFactory::Instance().ComponentTable().Value(tals_[i].listenerName_);

            // get the entities component
            ent->ComponentByType(componentType, tals_[i].listener_);
        }
    }

    void GCTrigger::Trigger(void* data)
    {
        // run all available tals
        for (Simian::u32 i = 0; i < tals_.size(); ++i)
        {
            if (tals_[i].listener_)
            {
                GCInputListener::Parameter param;
                param.Callee = ParentEntity();
                param.Data = data;
                tals_[i].listener_->OnTriggered(param);
            }
        }
    }

    void GCTrigger::AddListener( GCInputListener* listener )
    {
        TalTarget target;
        target.listener_ = listener;
        tals_.push_back(target);
    }

    void GCTrigger::Serialize( Simian::FileObjectSubNode& data )
    {
        Simian::FileArraySubNode* tals = data.AddArray("Tals");
        
        for (Simian::u32 i = 0; i < tals_.size(); ++i)
        {
            Simian::FileObjectSubNode* talNode = tals->AddObject("Tal");
            talNode->AddData("Target", tals_[i].targetName_);
            talNode->AddData("Listener", tals_[i].listenerName_);
        }
    }

    void GCTrigger::Deserialize( const Simian::FileObjectSubNode& data )
    {
        // get the tals
        const Simian::FileArraySubNode* tals = data.Array("Tals");

        if (tals)
        {
            tals_.clear();
            for (Simian::u32 i = 0; i < tals->Size(); ++i)
            {
                TalTarget newTal;
                const Simian::FileObjectSubNode* tal = tals->Object(i);
                tal->Data("Target", newTal.targetName_, "");
                tal->Data("Listener", newTal.listenerName_, "");
                newTal.listener_ = 0;
                tals_.push_back(newTal);
            }
        }
    }
}
