/************************************************************************/
/*!
\file		EntityFactoryPlant.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_ENTITYFACTORYPLANT_H_
#define SIMIAN_ENTITYFACTORYPLANT_H_

#include "SimianPlatform.h"
#include "FileObjectSubNode.h"
#include <map>

namespace Simian
{
    class FileObjectSubNode;
    class FileArraySubNode;
    class Scene;
    class Entity;

    class SIMIAN_EXPORT EntityFactoryPlant
    {
    protected:
        Scene* parentScene_;
        Entity* master_;
        FileObjectSubNode masterData_;
        u32 type_;
    public:
        Entity* Master() const;
    private:
        Entity* setupEntityRecursive_(const FileObjectSubNode& entityLayout, 
            std::map<u32, std::string>& poolTable);
        void setupMaster_(const FileObjectSubNode& entityLayout, 
            std::map<u32, std::string>& poolTable);
        void destroyMaster_();
        void destroyChildRecursive_(Entity* entity);

        Entity* cloneChildEntity_(Entity* ent);
        void cloneComponentSettings_(FileObjectSubNode* master, Entity* child);
    protected:
        virtual bool createBaseEntity_(Entity*& entity);
        virtual void destroyBaseEntity_(Entity*& entity);
    public:
        EntityFactoryPlant(Scene* parentScene, u32 type);
        virtual ~EntityFactoryPlant();

        virtual void SharedFileLoad();
        virtual void SharedFileUnload();
        virtual void SharedLoad();
        virtual void SharedUnload();

        virtual void Init();

        bool Create(Entity*& entity);
        void Destroy(Entity*& entity);

        friend class Scene;
    };
}

#endif
