/************************************************************************/
/*!
\file		Entity.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_ENTITY_H_
#define SIMIAN_ENTITY_H_

#include "SimianPlatform.h"
#include "AABB.h"

#include <vector>
#include <string>

namespace Simian
{
    class Scene;
    class EntityComponent;
    class Entity;

    typedef std::vector<Entity*> EntityList;

    class SIMIAN_EXPORT Entity
    {
    private:
        Scene* parentScene_;
        Entity* parentEntity_;
        u32 type_;
        std::string name_;
        bool anonymous_;
        std::vector<Entity*> children_;
        std::vector<EntityComponent*> components_;
        std::vector<EntityComponent*> activeComponents_;
        bool serializable_;
        AABB aabb_;
        bool cullable_;
    public:
        Scene* ParentScene() const;

        Entity* ParentEntity() const;

        Simian::u32 Type() const;

        const std::string& Name() const;
        void Name(const std::string& val);

        bool Anonymous() const;

        const std::vector<EntityComponent*>& Components() const;

        const std::vector<Entity*>& Children() const;

        bool Serializable() const;
        void Serializable(bool val);

        const Simian::AABB& AABB() const;

        bool Cullable() const;
    public:
        Entity();
        ~Entity();

        void AddComponent(u32 componentType);
        void AddComponent(u32 componentType, EntityComponent*& createdComponent);

        template <class T>
        void AddComponent(u32 componentType, T*& createdComponent)
        {
            AddComponent(componentType, reinterpret_cast<EntityComponent*&>(createdComponent));
        }

        void ComponentByType(u32 type, EntityComponent*& component);
        template <class T>
        void ComponentByType(u32 type, T*& component)
        {
            ComponentByType(type, reinterpret_cast<EntityComponent*&>(component));
        }

        // don't use this unless you know what you are doing.
        void ClearComponents();

        void SharedFileLoad();
        void SharedFileUnload();
        void SharedLoad();
        void SharedUnload();
        void Awake();
        void Init();
        void Deinit();
        void Reparent();
        void Update(u32 phase, f32 dt);

        void AddChild(Entity* entity);
        void RemoveChild(Entity* entity);
        void Reparent(Entity* parent);

        Entity* ChildByName(const std::string& name);
        Entity* ChildByType(u32 type);
        Entity* ChildByIndex(u32 index);

        void ChildrenByName(const std::string& name, EntityList& list);
        void ChildrenByType(u32 type, EntityList& list);

        void AddAABB(const Simian::AABB& aabb);
        void ClearAABB();
        
        Entity* Clone() const;

        friend class Scene;
        friend class EntityFactoryPlant;
    };
}

#endif
