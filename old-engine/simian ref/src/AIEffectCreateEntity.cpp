/******************************************************************************/
/*!
\file		AIEffectCreateEntity.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/11/01
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "AIEffectCreateEntity.h"

#include "GCAttributes.h"

#include "Simian/EngineComponents.h"
#include "Simian/Entity.h"
#include "Simian/Scene.h"
#include "Simian/CTransform.h"

namespace Descension
{
    Simian::FactoryPlant<AIEffectBase, AIEffectCreateEntity> AIEffectCreateEntity::factoryPlant_(
        AIEffectBase::createFactoryPlant_<AIEffectCreateEntity>(EFF_E_CREATEENT));

    AIEffectCreateEntity::AIEffectCreateEntity()
        :   AIEffectBase(EFF_E_CREATEENT),
            created_(false),
            ownerTrans_(0),
            entityType_(0),
            stateTrigger_(0)
    {
    }

    AIEffectCreateEntity::~AIEffectCreateEntity()
    {
    }

    bool AIEffectCreateEntity::Update( Simian::f32  )
    {
        if(!Enable())
            return false;

        Simian::u32 currState = Target()->CurrentState();
        if(!created_ && currState == stateTrigger_)
        {
            Simian::Entity* ent = Target()->ParentScene()->CreateEntity(entityType_);
            if(ent)
            {
                Simian::CTransform* entTrans = 0;
                ent->ComponentByType(Simian::C_TRANSFORM, entTrans);
                if(entTrans)
                {
                    entTrans->Position(ownerTrans_->Position());
                    Target()->ParentScene()->AddEntity(ent);
                    created_ = true;
                }
            }
        }
        else if(created_ && currState != stateTrigger_)
            created_ = false;
        return false;
    }

    //-Private------------------------------------------------------------------

    void AIEffectCreateEntity::onAwake_()
    {
    }

    void AIEffectCreateEntity::onInit_( GCAttributes* )
    {
        Target()->ComponentByType(Simian::C_TRANSFORM,ownerTrans_);
        
        if(!ownerTrans_)
            Enable(false);
    }

    void AIEffectCreateEntity::onUnload_()
    {
    }

    void AIEffectCreateEntity::serialize_( Simian::FileObjectSubNode& node )
    {
        node.AddData("EntityType",entityType_);
        node.AddData("StateTrigger",stateTrigger_);
    }

    void AIEffectCreateEntity::deserialize_( const Simian::FileObjectSubNode& node )
    {
        std::string str;
        node.Data("EntityType",str,str);

        if(str.size() > 0)
            entityType_ = Simian::GameFactory::Instance().EntityTable().Value(str);

        node.Data("StateTrigger",stateTrigger_,stateTrigger_);
    }
}
