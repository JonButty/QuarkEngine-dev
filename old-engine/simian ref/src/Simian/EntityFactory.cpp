/*****************************************************************************/
/*!
\file		EntityFactory.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/EntityFactory.h"

namespace Simian
{
    EntityFactory::~EntityFactory()
    {
        for (size_t i = 0; i < pools_.size(); ++i)
            delete pools_[i];
        pools_.clear();
        for (std::list<Entity*>::iterator i = masters_.begin(); i != masters_.end(); ++i)
            delete *i;
        masters_.clear();
    }

    //--------------------------------------------------------------------------

    EntityFactory& EntityFactory::operator=(const EntityFactory& factory)
    {
        Unload();
        pendingEntities_.clear();

        for (size_t i = 0; i < pools_.size(); ++i)
        {
            delete pools_[i];
            pools_[i] = 0;
        }
        pools_.clear();
        pools_.resize(19);
        for (std::list<Entity*>::iterator i = masters_.begin(); i != masters_.end(); ++i)
            delete *i;
        masters_.clear();

        for (size_t i = 0; i < factory.pools_.size(); ++i)
        {
            Entity* entity = new Entity();
            *entity = *factory.pools_[i]->Master();
            CreatePool(i, entity, factory.pools_[i]->Capacity());
        }

        Load();

        return *this;
    }

    //--------------------------------------------------------------------------

    void EntityFactory::Load()
    {
        for (size_t i = 0; i < pools_.size(); ++i)
            if (pools_[i]) pools_[i]->Load();
    }

    void EntityFactory::Unload()
    {
        for (size_t i = 0; i < pools_.size(); ++i)
            if (pools_[i]) pools_[i]->Unload();
    }
    
    void EntityFactory::UpdatePendingEntities()
    {
        for (size_t i = 0; i < pendingEntities_.size(); ++i)
        {
            if (pendingEntities_[i].first)
                pendingEntities_[i].second.second->Init();
            else
                pools_[pendingEntities_[i].second.first]->Return(pendingEntities_[i].second.second);
        }
        pendingEntities_.clear();
    }
}
