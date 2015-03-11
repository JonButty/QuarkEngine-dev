/*************************************************************************/
/*!
\file		TriggerEvent.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "TriggerEvent.h"
#include "GCInputListener.h"
#include "SpawnEventTypes.h"

#include "Simian\Entity.h"
#include "Simian\Scene.h"
#include "Simian\GameFactory.h"

namespace Descension
{
    SpawnFactory::Plant<TriggerEvent>::Type TriggerEvent::plant_(
        SpawnFactory::InstancePtr(), SE_TRIGGER);

    TriggerEvent::TriggerEvent()
        : callee_(0),
          listener_(0)
    {
    }

    //--------------------------------------------------------------------------

    void TriggerEvent::ResolveTal( Simian::Entity* entity, const std::string& listener, const std::string& target )
    {
        callee_ = entity;
        
        entity = target != "" ? entity->ParentScene()->EntityByNameRecursive(target) : entity;
        if (!entity)
            return;

        if (!Simian::GameFactory::Instance().ComponentTable().HasValue(listener))
            return;

        Simian::s32 type = Simian::GameFactory::Instance().ComponentTable().Value(listener);
        entity->ComponentByType(type, listener_);
    }

    bool TriggerEvent::Execute( Simian::f32 )
    {
        if (listener_)
        {
            GCInputListener::Parameter p;
            p.Callee = callee_;
            p.Data = 0;
            listener_->OnTriggered(p);
        }
        return true;
    }
}
