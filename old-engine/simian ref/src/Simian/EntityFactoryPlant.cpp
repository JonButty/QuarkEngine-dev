/************************************************************************/
/*!
\file		EntityFactoryPlant.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/EntityFactoryPlant.h"
#include "Simian/Entity.h"
#include "Simian/DataFileIO.h"
#include "Simian/GameFactory.h"
#include "Simian/EntityComponent.h"
#include "Simian/LogManager.h"
#include "Simian/Scene.h"

namespace Simian
{
    EntityFactoryPlant::EntityFactoryPlant(Scene* parentScene, u32 type)
        : parentScene_(parentScene),
          type_(type)
    {
    }

    EntityFactoryPlant::~EntityFactoryPlant()
    {
    }

    //--------------------------------------------------------------------------

    Entity* EntityFactoryPlant::Master() const
    {
        return master_;
    }

    //--------------------------------------------------------------------------

    Entity* EntityFactoryPlant::setupEntityRecursive_( const FileObjectSubNode& entityLayout, std::map<u32, std::string>& poolTable )
    {
        Entity* ret = new Entity();
        ret->parentScene_ = parentScene_;

        // get name
        const FileDataSubNode* nameNode = entityLayout.Data("Name");
        ret->name_ = nameNode ? nameNode->AsString() : "Unnamed Entity";

        entityLayout.Data("Cullable", ret->cullable_, ret->cullable_);

        // loop through components
        const FileArraySubNode* componentsNode = entityLayout.Array("Components");

        // do i have components?
        if (!componentsNode)
            return ret; // bail

        // loop components
        for (u32 i = 0; i < componentsNode->Size(); ++i)
        {
            const FileObjectSubNode* object = componentsNode->Object(i);
            const FileDataSubNode* type = object->Data("Type");

            if (!type)
            {
                SWarn("Component has no type.");
                continue;
            }

            if (!GameFactory::Instance().ComponentTable().HasValue(type->AsString()))
            {
                SWarn("No component of type: " + type->AsString());
                continue;
            }
            u32 componentId = GameFactory::Instance().ComponentTable().Value(type->AsString());

            EntityComponent* component;
            ret->AddComponent(componentId, component);
            component->Deserialize(*object);
        }

        const FileArraySubNode* childrenNode = entityLayout.Array("Children");
        if (!childrenNode)
            return ret;
        
        for (u32 i = 0; i < childrenNode->Size(); ++i)
        {
            const FileObjectSubNode* childNode = childrenNode->Object(i);
            const FileDataSubNode* type, *name;
            type = childNode->Data("Type");
            name = childNode->Data("Name");
            Entity* child = 0;
            if (!type)
            {
                // create with this sub node
                child = setupEntityRecursive_(*childNode, poolTable);
                child->anonymous_ = true;
            }
            else
            {
                // load the associated entity file and run this again
                DataFileIODefault childFile;
                if (!GameFactory::Instance().EntityTable().HasValue(type->AsString()))
                {
                    SWarn("No such entity: " << type->AsString());
                    continue;
                }
                u32 childType = GameFactory::Instance().EntityTable().Value(type->AsString());
                if (!childFile.Read(poolTable[childType]))
                {
                    SWarn("Child Type: " << childType << ", " << type_)
                    SWarn("Entity file failed to load: " << poolTable[childType]);
                    continue;
                }
                child = setupEntityRecursive_(*childFile.Root(), poolTable);
                child->anonymous_ = false;
                child->type_ = childType;
            }
            child->name_ = name ? name->AsString() : child->name_;
            childNode->Data("Cullable", child->cullable_, child->cullable_);
            ret->children_.push_back(child);
            child->parentEntity_ = ret;
        }

        return ret;
    }

    void EntityFactoryPlant::setupMaster_(const FileObjectSubNode& entityLayout,
        std::map<u32, std::string>& poolTable)
    {
        // create completely new entities for everything (including children, 
        // and deserialize all data correctly)
        masterData_ = entityLayout;
        master_ = setupEntityRecursive_(entityLayout, poolTable);
        master_->type_ = type_;
    }

    void EntityFactoryPlant::destroyMaster_()
    {
        // delete master recursively
        destroyChildRecursive_(master_);
        master_ = 0;
    }

    void EntityFactoryPlant::destroyChildRecursive_(Entity* entity)
    {
        for (u32 i = 0; i < entity->children_.size(); ++i)
            destroyChildRecursive_(entity->children_[i]);
        delete entity;
    }

    Entity* EntityFactoryPlant::cloneChildEntity_( Entity* ent )
    {
        Entity* ret;

        if (!ent->Anonymous())
        {
            // this will handle its own parenting so i'm dandy
            ret = parentScene_->CreateEntity(ent->Type());
            return ret;
        }

        // otherwise i have to do my own initialization (and add my own children)
        ret = ent->Clone();

        for (u32 i = 0; i < ent->children_.size(); ++i)
        {
            Entity* child = cloneChildEntity_(ent->children_[i]);
            ret->children_.push_back(child);
            child->parentEntity_ = ret;
        }

        // load my shiat
        ret->SharedFileLoad();
        ret->SharedLoad();

        return ret;
    }

    void EntityFactoryPlant::cloneComponentSettings_( FileObjectSubNode* master, Entity* child )
    {
        // get children
        FileArraySubNode* children = master->Array("Children");

        if (!children)
            return;

        for (u32 i = 0; i < children->Size(); ++i)
        {
            FileObjectSubNode* childNode = children->Object(i);

            FileDataSubNode* childName = childNode->Data("Name");
            if (childName)
                child->ChildByIndex(i)->Name(childName->AsString());

            // copy all components
            FileArraySubNode* components = childNode->Array("Components");
            if (!components)
                continue;
            for (u32 j = 0; j < components->Size(); ++j)
            {
                const FileObjectSubNode* componentNode = components->Object(j);
                const FileDataSubNode* type = componentNode->Data("Type");

                if (!type)
                {
                    SWarn("Component has no type.");
                    continue;
                }

                if (!GameFactory::Instance().ComponentTable().HasValue(type->AsString()))
                {
                    SWarn("No component of type: " + type->AsString());
                    continue;
                }
                u32 componentId = GameFactory::Instance().ComponentTable().Value(type->AsString());

                EntityComponent* component;
                child->ChildByIndex(i)->ComponentByType(componentId, component);
                component->Deserialize(*componentNode);
            }
        }
    }

    //--------------------------------------------------------------------------

    bool EntityFactoryPlant::createBaseEntity_( Entity*& entity )
    {
        entity = master_->Clone();
        return entity ? true : false;
    }

    void EntityFactoryPlant::destroyBaseEntity_( Entity*& entity )
    {
        delete entity;
        entity = 0;
    }

    //--------------------------------------------------------------------------

    void EntityFactoryPlant::SharedFileLoad()
    {
        master_->SharedFileLoad();
    }

    void EntityFactoryPlant::SharedFileUnload()
    {
        master_->SharedFileUnload();
    }

    void EntityFactoryPlant::SharedLoad()
    {
        master_->SharedLoad();
    }

    void EntityFactoryPlant::SharedUnload()
    {
        master_->SharedUnload();
    }

    void EntityFactoryPlant::Init()
    {
        // create the pools here
    }

    bool EntityFactoryPlant::Create( Entity*& entity )
    {
        // piece an entity together from this pool
        if (!createBaseEntity_(entity))
            return false;

        // add remove child entities (recursive)
        for (u32 i = 0; i < master_->children_.size(); ++i)
        {
            Entity* child = cloneChildEntity_(master_->children_[i]);
            entity->children_.push_back(child);
            child->parentEntity_ = entity;
        }

        // roll out children's settings
        cloneComponentSettings_(&masterData_, entity);

        return true;
    }

    void EntityFactoryPlant::Destroy( Entity*& entity )
    {
        // return the entity to the pool
        destroyBaseEntity_(entity);
    }
}
