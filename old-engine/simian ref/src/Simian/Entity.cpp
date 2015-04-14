/************************************************************************/
/*!
\file		Entity.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Entity.h"
#include "Simian/EntityComponent.h"
#include "Simian/Scene.h"
#include "Simian/GameFactory.h"
#include "Simian/DebugRenderer.h"
#include "Simian/EnginePhases.h"
#include "Simian/LogManager.h"

#define DEBUG_DRAW_ENTITY_AABBS 0

namespace Simian
{
    Entity::Entity()
        : parentScene_(0),
          parentEntity_(0),
          type_(0),
          name_(""),
          anonymous_(false),
          serializable_(true),
          cullable_(true)
    {
        components_.resize(GameFactory::Instance().TotalComponents(), NULL);
    }

    Entity::~Entity()
    {
        ClearComponents();
    }

    //--------------------------------------------------------------------------

    Scene* Entity::ParentScene() const
    {
        return parentScene_;
    }

    Entity* Entity::ParentEntity() const
    {
        return parentEntity_;
    }

    Simian::u32 Entity::Type() const
    {
        return type_;
    }

    const std::string& Entity::Name() const
    {
        return name_;
    }

    void Entity::Name( const std::string& val )
    {
        name_ = val;
    }

    bool Entity::Anonymous() const
    {
        return anonymous_;
    }

    const std::vector<EntityComponent*>& Entity::Components() const
    {
        return components_;
    }

    const std::vector<Entity*>& Entity::Children() const
    {
        return children_;
    }

    bool Entity::Serializable() const
    {
        return serializable_;
    }

    void Entity::Serializable( bool val )
    {
        serializable_ = val;
    }

    const Simian::AABB& Entity::AABB() const
    {
        return aabb_;
    }

    bool Entity::Cullable() const
    {
        return cullable_;
    }

    //--------------------------------------------------------------------------

    void Entity::AddComponent( u32 componentType )
    {
        EntityComponent* newComponent; // thrown away
        AddComponent(componentType, newComponent);
    }

    void Entity::AddComponent( u32 componentType, EntityComponent*& createdComponent )
    {
        // create component from game factory's component factory
        EntityComponent* component;
        GameFactory::Instance().ComponentFactory().CreateInstance(componentType, component);

        /*if (components_[componentType])
            SVerbose("I have a dubplicate: " << Name());*/

        // add to entity
        components_[componentType] = component;
        component->parentEntity_.Set(this);

        // add the component to the active components
        activeComponents_.push_back(component);

        // set createdcomponent to the new component
        createdComponent = component;
    }

    void Entity::ClearComponents()
    {
        for (u32 i = 0; i < components_.size(); ++i)
            delete components_[i];
        components_.clear();
        activeComponents_.clear();
    }

    void Entity::ComponentByType( u32 type, EntityComponent*& component )
    {
        component = components_[type];
    }

    void Entity::SharedFileLoad()
    {
        for (u32 i = 0; i < activeComponents_.size(); ++i)
            activeComponents_[i]->OnSharedFileLoad();
    }

    void Entity::SharedFileUnload()
    {
        for (u32 i = 0; i < activeComponents_.size(); ++i)
            activeComponents_[i]->OnSharedFileUnload();
    }

    void Entity::SharedLoad()
    {
        for (u32 i = 0; i < activeComponents_.size(); ++i)
            activeComponents_[i]->OnSharedLoad();
    }

    void Entity::SharedUnload()
    {
        for (u32 i = 0; i < activeComponents_.size(); ++i)
            activeComponents_[i]->OnSharedUnload();
    }

    void Entity::Awake()
    {
        for (u32 i = 0; i < activeComponents_.size(); ++i)
            activeComponents_[i]->OnAwake();
    }

    void Entity::Init()
    {
        for (u32 i = 0; i < activeComponents_.size(); ++i)
            activeComponents_[i]->OnInit();
    }

    void Entity::Deinit()
    {
        for (u32 i = 0; i < activeComponents_.size(); ++i)
            activeComponents_[i]->OnDeinit();
    }

    void Entity::Reparent()
    {
        for (u32 i = 0; i < activeComponents_.size(); ++i)
            activeComponents_[i]->OnReparent();
    }

    void Entity::Update( u32 phase, f32 dt )
    {
#if DEBUG_DRAW_ENTITY_AABBS == 1
        if (phase == P_RENDER - 1)
            DebugRendererDrawCube(aabb_.Midpoint(), aabb_.Size(), Color(1.0f, 1.0f, 0.4f));
#endif

        for (u32 i = 0; i < activeComponents_.size(); ++i)
            activeComponents_[i]->Update(phase, dt);
    }

    void Entity::AddChild( Entity* entity )
    {
        Scene::ARQueueObject object;
        object.Action = Scene::ARQueueObject::ACTION_ADD;
        object.Parent = this;
        object.Target = entity;

        // set parent scene and parent entity immediately
        entity->parentScene_ = parentScene_;
        entity->parentEntity_ = this;

        parentScene_->addRemoveQueue_.push_back(object);
    }

    void Entity::RemoveChild( Entity* entity )
    {
        // remove children recursively
        for (u32 i = 0; i < entity->children_.size(); ++i)
            entity->RemoveChild(entity->children_[i]);

        Scene::ARQueueObject object;
        object.Action = Scene::ARQueueObject::ACTION_REMOVE;
        object.Parent = this;
        object.Target = entity;
        parentScene_->addRemoveQueue_.push_back(object);
    }

    void Entity::Reparent( Entity* parent )
    {
        Scene::ARQueueObject object;
        object.Action = Scene::ARQueueObject::ACTION_REPARENT;
        object.Parent = parent;
        object.Target = this;
        parentScene_->addRemoveQueue_.push_back(object);
    }

    Entity* Entity::ChildByName( const std::string& name )
    {
        for (u32 i = 0; i < children_.size(); ++i)
            if (children_[i]->Name() == name)
                return children_[i];
        return NULL;
    }

    Entity* Entity::ChildByType( u32 type )
    {
        for (u32 i = 0; i < children_.size(); ++i)
            if (children_[i]->Type() == type && !children_[i]->Anonymous())
                return children_[i];
        return NULL;
    }

    Entity* Entity::ChildByIndex( u32 index )
    {
        return children_[index];
    }

    void Entity::ChildrenByName( const std::string& name, EntityList& list )
    {
        list.clear();
        for (u32 i = 0; i < children_.size(); ++i)
            if (children_[i]->Name() == name)
                list.push_back(children_[i]);
    }

    void Entity::ChildrenByType( u32 type, EntityList& list )
    {
        list.clear();
        for (u32 i = 0; i < children_.size(); ++i)
            if (children_[i]->Type() == type)
                list.push_back(children_[i]);
    }

    void Entity::AddAABB( const Simian::AABB& aabb )
    {
        if (aabb_.Exists())
            aabb_.Join(aabb);
        else
            aabb_ = aabb;

        // join with parent recursively
        if (parentEntity_)
            parentEntity_->AddAABB(aabb_);
    }

    void Entity::ClearAABB()
    {
        aabb_ = Simian::AABB();
    }

    Entity* Entity::Clone() const
    {
        // create an entity that is the same as this (apart from children)
        Entity* ent = new Entity();
        ent->name_ = name_;
        ent->parentScene_ = parentScene_;
        ent->type_ = type_;
        ent->anonymous_ = anonymous_;
        ent->cullable_ = cullable_;

        for (u32 i = 0; i < components_.size(); ++i)
        {
            // skip empty component slots
            if (!components_[i])
                continue;

            EntityComponent* component;
            ent->AddComponent(i, component);
            GameFactory::Instance().ComponentFactory().CopyInstance(i, component, components_[i]);

            if (!anonymous_)
                component->callbacks_.Set(*components_[i]->callbacks_);
        }

        return ent;
    }
}
