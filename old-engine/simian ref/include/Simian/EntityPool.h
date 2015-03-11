/*****************************************************************************/
/*!
\file		EntityPool.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_ENTITYPOOL_H_
#define SIMIAN_ENTITYPOOL_H_

#include "SimianPlatform.h"
#include "MemoryPool.h"
#include "Entity.h"

namespace Simian
{
    /** A memory pool that handles multiple instances of Entity. */
    class SIMIAN_EXPORT EntityPool: public MemoryPool<Entity>
    {
    private:
        Entity* master_;
    public:
        Entity* Master() const;
    private:
        /** Fetches an uninitalized Entity from the pool. */
        Entity* fetchUninit_();
    public:
        /** Creates a new EntityPool
            @remarks
                master is the only Entity that is loaded. Other entities are
                copied from master using the = operator of entity (deep 
                component copy).
            @see
                Load, Unload
        */
        EntityPool(s32 size, Entity* master);

        /** Loads the master Entity and copies it to the other entities. */
        void Load();
        /** Deinitializes borrowed entities and unloads the master.
            @remarks
                Unload invalidates instances of Entity from the pool. Using
                entities from an unloaded EntityPool will result in undefined
                behavior.
        */
        void Unload();

        /** Fetches an Entity from the pool. */
        Entity* Fetch();
        /** Returns an Entity to the pool. */
        void Return(Entity* entity);

        friend class EntityFactory;
    };
}

#endif
