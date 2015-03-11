/*****************************************************************************/
/*!
\file		EntityFactory.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_ENTITYFACTORY_H_
#define SIMIAN_ENTITYFACTORY_H_

#include "SimianPlatform.h"
#include <list>
#include <vector>
#include "EntityPool.h"

namespace Simian
{
    /** Manages a group of EntityPools. */
    class SIMIAN_EXPORT EntityFactory
    {
    private:
        std::list<Entity*> masters_;
        std::vector<EntityPool*> pools_;
        std::vector<std::pair<bool, std::pair<size_t, Entity*> > > pendingEntities_;
    private:
        /** Creates a new uninitialized Entity. */
        template <typename T>
        Entity* createEntityUninit_(T type)
        {
            return pools_[(size_t)type]->fetchUninit_();
        }
    public:
        EntityFactory& operator=(const EntityFactory& factory);
    public:
        /** Creates new EntityFactory. */
        template <typename T>
        EntityFactory(T totalTypes)
        {
            pools_.resize((size_t)totalTypes, 0);
        }
        ~EntityFactory();

        /** Creates a pool for a certain type.
            @remarks
                type should allow casting to unsigned integral.
        */
        template <typename T>
        void CreatePool(T type, Entity*& master, size_t size)
        {
            pools_[(size_t)type] = new EntityPool(size, master);
            masters_.push_back(master);
            master = 0; // consumes master
        }

        /** Creates an Entity of a certain type.
            @remarks
                type should allow casting to unsigned integral.
        */
        template <typename T>
        Entity* CreateEntity(T type)
        {
            Entity* entity = pools_[(size_t)type]->fetchUninit_();
            pendingEntities_.push_back(std::pair<bool, std::pair<size_t, Entity*> >(true, 
                std::pair<size_t, Entity*>(type, entity)));
            return entity;
        }

        /** Creates an Entity of a certain type immediately.
            @remarks
                type should allow casting to unsigned integral.
        */
        template <typename T>
        Entity* CreateEntityImmediate(T type)
        {
            return pools_[(size_t)type]->Fetch();
        }

        /** Destroys an Entity of a certain type.
            @remarks
                type should allow casting to unsigned integral.
        */
        template <typename T>
        void DestroyEntity(T type, Entity* entity)
        {                                           
            pendingEntities_.push_back(std::pair<bool, std::pair<size_t, Entity*> >(false, 
                std::pair<size_t, Entity*>(type, entity)));
        }

        /** Destroys an Entity of a certain type immediately.
            @remarks
                type should allow casting to unsigned integral.
        */
        template <typename T>
        void DestroyEntityImmediate(T type, Entity* entity)
        {
            pools_[(size_t)type]->Return(entity);
        }

        /** Loads all the EntityPools associated with the EntityFactory. */
        void Load();
        /** Unloads all the EntityPools associated with the EntityFactory. */
        void Unload();
        /** Fetches and returns the pending entities to their respective pools. */
        void UpdatePendingEntities();

        friend class GameFactory;
        friend class Scene;
    };
}

#endif
