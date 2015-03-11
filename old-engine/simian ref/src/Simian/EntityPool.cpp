/*****************************************************************************/
/*!
\file		EntityPool.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/EntityPool.h"

namespace Simian
{
    EntityPool::EntityPool(s32 size, Entity* master)
    : MemoryPool(size), master_(master)
    {
    }

    //--------------------------------------------------------------------------

    Entity* EntityPool::Master() const
    {
        return master_;
    }

    //--------------------------------------------------------------------------

    Entity* EntityPool::fetchUninit_()
    {
        return MemoryPool<Entity>::Fetch();
    }

    //--------------------------------------------------------------------------

    void EntityPool::Load()
    {
        master_->Load();

        // clone all entities from the loaded master
        for (size_t i = 0; i < Pool().size(); ++i)
            *Pool()[i] = *master_;
    }

    void EntityPool::Unload()
    {
        for (std::list<Entity*>::const_iterator i = Borrowed().begin(); i != Borrowed().end(); ++i)
            (*i)->Deinit();
        master_->Unload();
    }

    Entity* EntityPool::Fetch()
    {
        Entity* ret = MemoryPool<Entity>::Fetch();
        ret->Init();
        return ret;
    }

    void EntityPool::Return(Entity* entity)
    {
        entity->Deinit();
        entity->Reset(master_);
        MemoryPool<Entity>::Return(entity);
    }
}
