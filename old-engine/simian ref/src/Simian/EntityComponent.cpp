/************************************************************************/
/*!
\file		EntityComponent.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/EntityComponent.h"
#include "Simian/GameFactory.h"

namespace Simian
{
    EntityComponent::EntityComponent()
        : parentEntity_(0)
    {
        (*callbacks_).resize(GameFactory::Instance().TotalPhases());
    }

    EntityComponent::~EntityComponent()
    {
    }

    //--------------------------------------------------------------------------

    Entity* EntityComponent::ParentEntity() const
    {
        return parentEntity_;
    }

    Scene* EntityComponent::ParentScene() const
    {
        return parentEntity_ ? parentEntity_->ParentScene() : NULL;
    }

    //--------------------------------------------------------------------------

    void EntityComponent::RegisterCallback( u32 phase, const Delegate& callback )
    {
        (*callbacks_)[phase].push_back(callback);
    }

    void EntityComponent::Update( u32 phase, f32 dt )
    {
        for (u32 i = 0; i < (*callbacks_)[phase].size(); ++i)
        {
            EntityUpdateParameter param;
            param.Dt = dt;
            (*callbacks_)[phase][i](this, param);
        }
    }

    void EntityComponent::ComponentByType( u32 type, EntityComponent*& component )
    {
        if (ParentEntity())
            ParentEntity()->ComponentByType(type, component);
    }
}
