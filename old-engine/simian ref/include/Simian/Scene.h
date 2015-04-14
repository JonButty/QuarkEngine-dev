/************************************************************************/
/*!
\file		Scene.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_SCENE_H_
#define SIMIAN_SCENE_H_

#include "SimianPlatform.h"
#include "DataLocation.h"
#include "DataFileIO.h"

#include <vector>
#include <map>
#include <string>

namespace Simian
{
    class Entity;
    class EntityFactoryPlant;

    typedef std::vector<Entity*> EntityList;

    class ModelCache;
    class TextureCache;
    class ShaderCache;
    class SMStrategy;

    class SIMIAN_EXPORT Scene
    {
    private:
        struct ARQueueObject
        {
            enum ARAction
            {
                ACTION_ADD,
                ACTION_REMOVE,
                ACTION_REPARENT
            } Action;
            Entity* Parent;
            Entity* Target;

            bool operator==(const ARQueueObject& other) const;
        };
    private:
        std::vector<Simian::Entity*> rootEntities_;
        std::vector<Simian::Entity*> visibleEntities_;
        std::vector<Simian::Entity*> unculledEntities_;
        std::vector<ARQueueObject> addRemoveQueue_;
        std::vector<EntityFactoryPlant*> pools_;
        std::vector<std::string> poolFiles_;
        DataFileIODefault sceneFile_;

        SMStrategy* strategy_;

        static ModelCache* modelCache_;
        static TextureCache* textureCache_;
        static ShaderCache* shaderCache_;
    public:
        const std::vector<Simian::Entity*>& RootEntities() const;

        static Simian::ModelCache* ModelCache();
        static void ModelCache(Simian::ModelCache* val);

        static Simian::TextureCache* TextureCache();
        static void TextureCache(Simian::TextureCache* val);

        static Simian::ShaderCache* ShaderCache();
        static void ShaderCache(Simian::ShaderCache* val);

        const std::vector<std::string>& PoolFiles() const;
    private:
        void initializePoolFilesFromScene_();
        void createPoolsFromMemory_(const std::vector<std::string>& paths);

        void loadPool_(const std::string& path, std::map<u32, std::string>& poolTable);
        void setupPools_(std::map<u32, std::string>& poolTable);

        void destroyPools_();
        void sharedLoadFile_();
        void sharedUnloadFile_();
        void sharedLoad_();
        void sharedUnload_();
        void createScene_();
        void awake_();
        void init_();

        void processAddRemoveQueue_();
        void awakeRecursive_(Entity* entity);
        void initRecursive_(Entity* entity);
        void deinitRecursive_(Entity* entity);
        void sharedUnloadRecursive_(Entity* entity);
        void updateRecursive_(u32 phase, Entity* entity, f32 dt);
        void addUncullableRecursive_( Entity* entity );
        void clearAABBRecursive_( Entity* entity );

        void recurseComponentSettings_(const FileObjectSubNode& entityNode, Entity* entity);
        Entity* recurseAnonymousEntity_(const FileObjectSubNode& entityNode);

        Entity* entityByNameRecursive_(Entity* entity, const std::string& name);
        Entity* entityByTypeRecursive_(Entity* entity, u32 type);

        void serializeEntity_(Entity* entity, FileObjectSubNode* node, s32 index = -1);
        bool serializeEntityOptimized_(Entity* entity, FileObjectSubNode* node, s32 index = -1, Entity* master = 0);
    public:
        Scene();
        ~Scene();

        template <typename T>
        void SetSMStrategy() 
        { 
            delete strategy_;
            strategy_ = new T();
            strategy_->RootEntities(rootEntities_);
            strategy_->Scene(this);
        }

        void LoadImmediate(const DataLocation& data, bool threaded = false);

        // loads the scene layout
        bool LoadLayout(const DataLocation& data);
        // loads the scene layout with custom pools (the scene still thinks it 
        // has pools according to the scene file)
        bool LoadLayout(const DataLocation& data, const std::vector<std::string>& pools);
        // this calls in another thread if enabled
        void LoadFiles();
        // this calls in the main thread only when LoadFiles is done
        void LoadResources();
        // this creates the objects on the scene and finishes the loading
        void LoadObjects();

        void Unload();

        void Serialize(const std::string& path, bool optimize = false);

        void Update(f32 dt);

        // creates anonymous entity
        Entity* CreateEntity();
        // creates entity of a named type (decides if its from a pool or not)
        Entity* CreateEntity(u32 type);
        // create an entity from a fileobjectsubnode
        Entity* CreateEntity(const FileObjectSubNode& entityNode);

        // queues an entity for adding (to root)
        void AddEntity(Entity*& entity);
        // queues an entity for removal (to root)
        void RemoveEntity(Entity* entity);

        // gets an entity by name (root only)
        Entity* EntityByName(const std::string& name);
        // gets an entity by type (root only)
        Entity* EntityByType(u32 type);

        // gets an entity by name (root only)
        Entity* EntityByNameRecursive(const std::string& name);
        // gets an entity by type (root only)
        Entity* EntityByTypeRecursive(u32 type);

        // gets the entity by its index as a child.
        Entity* EntityByIndex(u32 index);

        void EntitiesByName(const std::string& name, EntityList& list);
        void EntitiesByType(u32 type, EntityList& list);

        void AddVisibleEntity(Entity* entity);

        // Linked pools will only be loaded on the next load
        void LinkPool(const std::string& path);
        // Unlink a pool if it exists.
        void UnlinkPool(const std::string& path);

        friend class Entity;
        friend class GameFactory;
    };
}

#endif
