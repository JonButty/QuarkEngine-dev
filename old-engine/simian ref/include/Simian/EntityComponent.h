/************************************************************************/
/*!
\file		EntityComponent.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_ENTITYCOMPONENT_H_
#define SIMIAN_ENTITYCOMPONENT_H_

#include "SimianPlatform.h"
#include "Delegate.h"
#include "TemplateFactory.h"
#include "GameFactory.h"
#include "ExplicitType.h"
#include "Entity.h"
#include "DataFileIO.h"

#include <vector>

namespace Simian
{
    class Entity;
    class FileObjectSubNode;

    struct SIMIAN_EXPORT EntityUpdateParameter: public DelegateParameter
    {
        f32 Dt;
    };

    class SIMIAN_EXPORT EntityComponent
    {
    private:
        ExplicitType<Entity*> parentEntity_;
        ExplicitType<std::vector<std::vector<Delegate> > > callbacks_;
    public:
        Entity* ParentEntity() const;
        Scene* ParentScene() const;
    public:
        EntityComponent();
        virtual ~EntityComponent();

        // These 2 functions are called back on the loading thread
        // for pools these are only called for the master
        virtual void OnSharedFileLoad() {}
        virtual void OnSharedFileUnload() {}

        // These 2 are called on the main thread only when the loading thread is 
        // completely executed, for pools these are only called on the master
        virtual void OnSharedLoad() {}
        virtual void OnSharedUnload() {}

        virtual void OnAwake() {}
        virtual void OnInit() {}
        virtual void OnDeinit() {}
        virtual void OnReparent() {}

        virtual void Serialize(FileObjectSubNode&) {}
        virtual void Deserialize(const FileObjectSubNode&) {}

        void RegisterCallback(u32 phase, const Delegate& callback);
        void Update(u32 phase, f32 dt);

        // helper functions for making it easier to get components from other 
        // entities in the same parent.
        void ComponentByType(u32 type, EntityComponent*& component);
        template <class T>
        void ComponentByType(u32 type, T*& component)
        {
            ComponentByType(type, reinterpret_cast<EntityComponent*&>(component));
        }

        friend class Entity;
    };
}

#endif
