/************************************************************************/
/*!
\file		Scene.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Scene.h"
#include "Simian/Entity.h"
#include "Simian/LogManager.h"
#include "Simian/EntityFactoryPlant.h"
#include "Simian/GameFactory.h"
#include "Simian/EntityComponent.h"
#include "Simian/ModelCache.h"
#include "Simian/TextureCache.h"
#include "Simian/ShaderCache.h"
#include "Simian/SMStrategy.h"
#include "Simian/SMDefaultStrategy.h"
#include "Simian/EnginePhases.h"

#include <algorithm>

namespace Simian
{
    TextureCache* Scene::textureCache_ = 0;
    ShaderCache* Scene::shaderCache_ = 0;
    ModelCache* Scene::modelCache_ = 0;

    bool Scene::ARQueueObject::operator==( const ARQueueObject& other ) const
    {
        return Action == other.Action && 
            Parent == other.Parent && 
            Target == other.Target;
    }

    //--------------------------------------------------------------------------

    Scene::Scene()
        : strategy_(0)
    {
        SetSMStrategy<SMDefaultStrategy>();
    }

    Scene::~Scene()
    {
        delete strategy_;
        strategy_ = 0;
    }

    //--------------------------------------------------------------------------

    const std::vector<Simian::Entity*>& Scene::RootEntities() const
    {
        return rootEntities_;
    }

    Simian::ModelCache* Scene::ModelCache()
    {
        return modelCache_;
    }

    void Scene::ModelCache( Simian::ModelCache* val )
    {
        modelCache_ = val;
    }

    Simian::TextureCache* Scene::TextureCache()
    {
        return textureCache_;
    }

    void Scene::TextureCache( Simian::TextureCache* val )
    {
        textureCache_ = val;
    }

    Simian::ShaderCache* Scene::ShaderCache()
    {
        return shaderCache_;
    }

    void Scene::ShaderCache( Simian::ShaderCache* val )
    {
        shaderCache_ = val;
    }

    const std::vector<std::string>& Scene::PoolFiles() const
    {
        return poolFiles_;
    }

    //--------------------------------------------------------------------------

    void Scene::initializePoolFilesFromScene_()
    {
        // get root
        FileRootSubNode* root = sceneFile_.Root();

        // get pools
        FileArraySubNode* pools = root->Array("Pools");

        if (pools)
        {
            for (u32 i = 0; i < pools->Size(); ++i)
                poolFiles_.push_back(pools->Data(i)->AsString());
        }
    }

    void Scene::createPoolsFromMemory_( const std::vector<std::string>& paths )
    {
        // resize pools
        pools_.resize(GameFactory::Instance().TotalEntities(), 0);

        // load the pools
        std::map<u32, std::string> poolTable;

        for (u32 i = 0; i < paths.size(); ++i)
            loadPool_(paths[i], poolTable);

        setupPools_(poolTable);
    }

    void Scene::loadPool_( const std::string& path, std::map<u32, std::string>& poolTable )
    {
        // open the file and load pools
        DataFileIODefault poolFile;
        poolFile.Read(path);

        FileArraySubNode& entities = *poolFile.Root()->Array("Entities");

        // build the pool table
        for (u32 i = 0; i < entities.Size(); ++i)
        {
            FileDataSubNode* type, *file, *size;
            type = entities.Object(i)->Data("Type");
            file = entities.Object(i)->Data("File");
            size = entities.Object(i)->Data("Size");

            if (!type || !file)
            {
                SWarn("Pool contains invalid entry.");
                continue;
            }

            // get id from type
            if (!GameFactory::Instance().EntityTable().HasValue(type->AsString()))
            {
                SWarn(std::string("No such type as: ") + type->AsString());
                continue;
            }
            u32 id = GameFactory::Instance().EntityTable().Value(type->AsString());

            // duplicate pool load, don't do it dude.
            if (pools_[id])
                continue;

            poolTable[id] = file->AsString();
            pools_[id] = new EntityFactoryPlant(this, id);
        }
    }

    void Scene::setupPools_( std::map<u32, std::string>& poolTable )
    {
        for (u32 i = 0; i < pools_.size(); ++i)
        {
            if (!pools_[i])
                continue;

            DataFileIODefault entityFile;
            if (!entityFile.Read(poolTable[i]))
            {
                SWarn(std::string("Failed to load entity: ") + poolTable[i]);
                delete pools_[i];
                pools_[i] = 0;
                continue;
            }

            pools_[i]->setupMaster_(*entityFile.Root(), poolTable);
        }
    }

    void Scene::destroyPools_()
    {
        for (u32 i = 0; i < pools_.size(); ++i)
        {
            if (!pools_[i])
                continue;
            pools_[i]->destroyMaster_();
            delete pools_[i];
        }
        pools_.clear();
    }

    void Scene::sharedLoadFile_()
    {
        // load all pools
        for (u32 i = 0; i < pools_.size(); ++i)
        {
            if (!pools_[i])
                continue;
            pools_[i]->SharedFileLoad();
        }
    }

    void Scene::sharedUnloadFile_()
    {
        // unload all pools
        for (u32 i = 0; i < pools_.size(); ++i)
        {
            if (!pools_[i])
                continue;
            pools_[i]->SharedFileUnload();
        }
    }

    void Scene::sharedLoad_()
    {
        // load all pools
        for (u32 i = 0; i < pools_.size(); ++i)
        {
            if (!pools_[i])
                continue;

            pools_[i]->SharedLoad();

            // clone everything in the pool
            pools_[i]->Init();
        }
    }

    void Scene::sharedUnload_()
    {
        // unload all pools
        for (u32 i = 0; i < pools_.size(); ++i)
        {
            if (!pools_[i])
                continue;

            pools_[i]->SharedUnload();
        }
    }

    void Scene::createScene_()
    {
        // load the scene file
        const FileRootSubNode* root = sceneFile_.Root();

        // parse and add stuff to the scene
        const FileArraySubNode* entities = root->Array("Entities");

        // no entities?
        if (!entities)
            return;

        // we only care about base objects
        for (u32 i = 0; i < entities->Size(); ++i)
        {
            // pull things out of the pool
            const FileObjectSubNode* entityNode = entities->Object(i);
            
            // invalid object node
            if (!entityNode)
                continue;

            Entity* entity = CreateEntity(*entityNode);
            if (!entity)
            {
                SWarn("Entity creation failed.");
                continue;
            }
            rootEntities_.push_back(entity);
            addUncullableRecursive_(entity);
            entity->parentScene_ = this;
        }
    }

    void Scene::awake_()
    {
        // call awake on all things
        for (u32 i = 0; i < rootEntities_.size(); ++i)
            awakeRecursive_(rootEntities_[i]);
    }

    void Scene::init_()
    {
        // call init on all things
        for (u32 i = 0; i < rootEntities_.size(); ++i)
            initRecursive_(rootEntities_[i]);

        // process add remove queue incase it increased
        processAddRemoveQueue_();
    }

    void Scene::processAddRemoveQueue_()
    {
        std::unique(addRemoveQueue_.begin(), addRemoveQueue_.end());

        // go through the add remove queue
        for (u32 i = 0; i < addRemoveQueue_.size(); ++i)
        {
            ARQueueObject& object = addRemoveQueue_[i];

            if (object.Action == ARQueueObject::ACTION_ADD)
            {
                // if the object is uncullable we add it to the unculled list
                addUncullableRecursive_(object.Target);

                // if adding, if anonymous, do loads, then run awake then init
                if (object.Parent)
                    object.Parent->children_.push_back(object.Target);
                else
                    rootEntities_.push_back(object.Target);
                if (object.Target->Anonymous())
                {
                    object.Target->SharedFileLoad();
                    object.Target->SharedLoad();
                }
                awakeRecursive_(object.Target);
                initRecursive_(object.Target);
            }
            else if (object.Action == ARQueueObject::ACTION_REMOVE)
            {
                // if removing run deinit
                object.Target->Deinit();

                // remove from parent then recurse and unload and destroy all non pool
                if (object.Parent)
                {
                    std::vector<Entity*>::iterator iter = std::find(object.Parent->children_.begin(), object.Parent->children_.end(), object.Target);
                    object.Parent->children_.erase(iter);
                }
                else
                {
                    std::vector<Entity*>::iterator iter = std::find(rootEntities_.begin(), rootEntities_.end(), object.Target);
                    rootEntities_.erase(iter);
                }

                // remove the object from the unculled list
                if (!object.Target->Cullable())
                {
                    std::vector<Entity*>::iterator i = 
                        std::find(unculledEntities_.begin(), unculledEntities_.end(), object.Target);
                    if (i != unculledEntities_.end())
                        unculledEntities_.erase(i);
                }

                // objects, for pool objects return them to their pool
                if (object.Target->Anonymous())
                {
                    object.Target->SharedUnload();
                    object.Target->SharedFileUnload();
                    delete object.Target;
                }
                else
                {
                    // return to pool, unload all children
                    pools_[object.Target->Type()]->Destroy(object.Target);
                }
            }
            else if (object.Action == ARQueueObject::ACTION_REPARENT)
            {
                Simian::Entity* parent = object.Target->ParentEntity();

                // if the parent is the same trivial rejection
                if (parent == object.Parent)
                    continue;

                // remove from parent..
                if (parent)
                {
                    std::vector<Entity*>::iterator iter = std::find(parent->children_.begin(), parent->children_.end(), object.Target);
                    parent->children_.erase(iter);
                }
                else
                {
                    std::vector<Entity*>::iterator iter = std::find(rootEntities_.begin(), rootEntities_.end(), object.Target);
                    rootEntities_.erase(iter);
                }

                // add to new parent!
                if (object.Parent)
                    object.Parent->children_.push_back(object.Target);
                else
                    rootEntities_.push_back(object.Target);

                // set up parenting
                object.Target->parentEntity_ = object.Parent;

                // call target's reparent..
                object.Target->Reparent();
            }
        }
        addRemoveQueue_.clear();

        // load caches if needed
        if (modelCache_->Invalidated())
        {
            modelCache_->LoadFiles();
            modelCache_->LoadMeshes();
        }

        // load caches if needed
        if (textureCache_->Invalidated())
        {
            textureCache_->LoadFiles();
            textureCache_->LoadTextures();
        }

        // load caches if needed
        if (shaderCache_->Invalidated())
            shaderCache_->LoadShaders();
    }

    void Scene::awakeRecursive_( Entity* entity )
    {
        entity->Awake();
        for (u32 i = 0; i < entity->children_.size(); ++i)
            awakeRecursive_(entity->children_[i]);
    }

    void Scene::initRecursive_( Entity* entity )
    {
        entity->Init();
        for (u32 i = 0; i < entity->children_.size(); ++i)
            initRecursive_(entity->children_[i]);
    }

    void Scene::deinitRecursive_( Entity* entity )
    {
        for (u32 i = 0; i < entity->children_.size(); ++i)
            deinitRecursive_(entity->children_[i]);
        entity->Deinit();
    }

    void Scene::sharedUnloadRecursive_( Entity* entity )
    {
        for (u32 i = 0; i < entity->children_.size(); ++i)
            sharedUnloadRecursive_(entity->children_[i]);

        if (entity->Anonymous())
        {
            entity->SharedUnload();
            entity->SharedFileUnload();
            delete entity;
        }
        else
        {
            // return to pool, unload all children
            pools_[entity->Type()]->Destroy(entity);
        }
    }

    void Scene::updateRecursive_( u32 phase, Entity* entity, f32 dt )
    {
        entity->Update(phase, dt);

        for (u32 i = 0; i < entity->children_.size(); ++i)
            updateRecursive_(phase, entity->children_[i], dt);
    }

    void Scene::addUncullableRecursive_( Entity* entity )
    {
        if (!entity->Cullable())
            unculledEntities_.push_back(entity);

        for (u32 i = 0; i < entity->children_.size(); ++i)
            addUncullableRecursive_(entity->children_[i]);
    }

    void Scene::clearAABBRecursive_( Entity* entity )
    {
        entity->ClearAABB();
        for (u32 i = 0; i < entity->children_.size(); ++i)
            clearAABBRecursive_(entity->children_[i]);
    }

    void Scene::recurseComponentSettings_( const FileObjectSubNode& entityNode, Entity* entity )
    {
        const FileDataSubNode* nameNode = entityNode.Data("Name");
        entity->name_ = nameNode ? nameNode->AsString() : entity->name_;

        entityNode.Data("Cullable", entity->cullable_, entity->cullable_);

        // get the components array
        const FileArraySubNode* componentsNode = entityNode.Array("Components");

        // no need to do it for this.
        if (componentsNode)
        {
            for (u32 i = 0; i < componentsNode->Size(); ++i)
            {
                const FileObjectSubNode* componentNode = componentsNode->Object(i);

                if (!componentNode)
                    continue;

                const FileDataSubNode* typeNode = componentNode->Data("Type");

                if (!typeNode)
                {
                    SWarn("Component needs a type!");
                    continue;
                }

                if (!GameFactory::Instance().ComponentTable().HasValue(typeNode->AsString()))
                {
                    SWarn("No such component: " + typeNode->AsString());
                    continue;
                }

                // deserialize component with this
                u32 componentId = GameFactory::Instance().ComponentTable().Value(typeNode->AsString());
                if (!entity->components_[componentId])
                {
                    SWarn("Entity does not have component: " + typeNode->AsString());
                    continue;
                }

                entity->components_[componentId]->Deserialize(*componentNode);
            }
        }

        const FileArraySubNode* childrenNode = entityNode.Array("Children");

        // bail case
        if (!childrenNode)
            return;

        // go through all my children and do this.
        for (u32 i = 0; i < childrenNode->Size(); ++i)
        {
            const FileObjectSubNode* child = childrenNode->Object(i);
            if (!child)
                continue;
            
            u32 index = i;
            // if we are an anonymous type, index is i, otherwise look for it
            if (child->Data("Index") || entityNode.Data("Type"))
            {
                const FileDataSubNode* indexNode = child->Data("Index");
                if (!indexNode)
                {
                    SWarn("Children of a pool object must have index.");
                    continue;
                }

                index = static_cast<u32>(indexNode->AsF32());
            }

            if (child && index < entity->children_.size())
                recurseComponentSettings_(*child, entity->children_[index]);
        }
    }

    Entity* Scene::recurseAnonymousEntity_( const FileObjectSubNode& entityNode )
    {
        Entity* entity = CreateEntity();

        const FileDataSubNode* nameNode = entityNode.Data("Name");
        entity->name_ = nameNode ? nameNode->AsString() : entity->name_;

        // check if the entity is cullable
        entityNode.Data("Cullable", entity->cullable_, entity->cullable_);
        
        // add components for each component in entity node
        const FileArraySubNode* components = entityNode.Array("Components");
        
        if (components)
        {
            for (u32 i = 0; i < components->Size(); ++i)
            {
                const FileObjectSubNode* componentNode = components->Object(i);
                if (!componentNode)
                    continue;
                const FileDataSubNode* type = componentNode->Data("Type");
                if (!type)
                {
                    SWarn("Components must have types.");
                    continue;
                }
                if (!GameFactory::Instance().ComponentTable().HasValue(type->AsString()))
                {
                    SWarn("No such component as: " + type->AsString());
                    continue;
                }
                u32 componentId = GameFactory::Instance().ComponentTable().Value(type->AsString());

                EntityComponent* component;
                entity->AddComponent(componentId, component);
                component->Deserialize(*componentNode);
            }
        }

        // load the entity (his components that is)
        entity->SharedFileLoad();
        entity->SharedLoad();

        const FileArraySubNode* childrenNode = entityNode.Array("Children");

        if (!childrenNode)
            return entity;

        for (u32 i = 0; i < childrenNode->Size(); ++i)
        {
            const FileObjectSubNode* childNode = childrenNode->Object(i);
            if (!childNode)
                continue;

            // check its type
            const FileDataSubNode* type = childNode->Data("Type");
            Entity* child = 0;
            if (type)
            {
                // no such type
                if (!GameFactory::Instance().EntityTable().HasValue(type->AsString()))
                {
                    SWarn("No such entity type: " + type->AsString());
                    continue;
                }
                u32 typeId = GameFactory::Instance().EntityTable().Value(type->AsString());
                child = CreateEntity(typeId);
            }
            else
                child = recurseAnonymousEntity_(*childNode);

            if (!child)
                continue;

            entity->children_.push_back(child);
            child->parentEntity_ = entity;
            child->parentScene_ = this;
        }
        
        return entity;
    }

    Entity* Scene::entityByNameRecursive_( Entity* entity, const std::string& name )
    {
        if (entity->Name() == name)
            return entity;

        for (std::vector<Entity*>::const_iterator i = entity->Children().begin(); 
             i != entity->Children().end(); ++i)
        {
            Entity* ent = entityByNameRecursive_(*i, name);
            if (ent)
                return ent;
        }

        return NULL;
    }

    Entity* Scene::entityByTypeRecursive_( Entity* entity, u32 type )
    {
        if (entity->Type() == type && !entity->Anonymous())
            return entity;

        for (std::vector<Entity*>::const_iterator i = entity->Children().begin(); 
            i != entity->Children().end(); ++i)
        {
            Entity* ent = entityByTypeRecursive_(*i, type);
            if (ent)
                return ent;
        }

        return NULL;
    }

    void Scene::serializeEntity_( Entity* entity, FileObjectSubNode* node, s32 index /*= -1*/ )
    {
        // put the name in
        node->AddData("Name", entity->Name());

        if (index >= 0)
            node->AddData("Index", index);
        else if (!entity->Anonymous())
        {
            std::string type = GameFactory::Instance().EntityTable().Key(entity->Type());
            node->AddData("Type", type);
            index = 0;
        }

        node->AddData("Cullable", entity->Cullable());

        // add the component
        FileArraySubNode* components = node->AddArray("Components");

        // serialize the component
        s32 componentId = 0;
        for (std::vector<EntityComponent*>::const_iterator i = entity->Components().begin();
             i != entity->Components().end(); ++i)
        {
            if ((*i))
            {
                FileObjectSubNode* component = components->AddObject("Component");
                std::string type = GameFactory::Instance().ComponentTable().Key(componentId);
                component->AddData("Type", type);
                (*i)->Serialize(*component);
            }
            ++componentId;
        }

        if (entity->Children().size())
        {
            FileArraySubNode* children = node->AddArray("Children");

            s32 childIndex = 0;
            // serialize children
            for (std::vector<Entity*>::const_iterator i = entity->Children().begin();
                i != entity->Children().end(); ++i)
            {
                // don't serialize non serializable stuff
                if (!(*i)->Serializable())
                    continue;
                serializeEntity_(*i, children->AddObject("Child"), index >= 0 ? childIndex : -1);
                ++childIndex;
            }
        }
    }

    bool Scene::serializeEntityOptimized_( Entity* entity, FileObjectSubNode* node, s32 index, Entity* master )
    {
        bool differencesFound = false;

        // put the name in
        node->AddData("Name", entity->Name());

        if (index >= 0)
            node->AddData("Index", index);
        else if (!entity->Anonymous())
        {
            std::string type = GameFactory::Instance().EntityTable().Key(entity->Type());
            node->AddData("Type", type);
            index = 0;
            master = pools_[entity->Type()]->Master();
        }

        node->AddData("Cullable", entity->Cullable());

        // add the component
        FileArraySubNode* components = new FileArraySubNode();
        components->Name("Components");

        // serialize the component
        s32 componentId = 0;
        for (std::vector<EntityComponent*>::const_iterator i = entity->Components().begin();
            i != entity->Components().end(); ++i)
        {
            if ((*i))
            {
                FileObjectSubNode* component = new FileObjectSubNode();
                component->Name("Component");
                (*i)->Serialize(*component);

                // compare component with master if it is not anonymous
                if (master)
                {
                    FileObjectSubNode* masterComponentNode = new FileObjectSubNode();
                    Simian::EntityComponent* masterComponent = 0;
                    master->ComponentByType(componentId, masterComponent);
                    masterComponent->Serialize(*masterComponentNode);

                    // compare component children and strip repeats
                    component->Diff(*masterComponentNode);

                    delete masterComponentNode;
                }

                if (component->Count())
                {
                    differencesFound = true;
                    std::string type = GameFactory::Instance().ComponentTable().Key(componentId);
                    component->AddData("Type", type);
                    components->AddChild(component);
                }
                else
                    delete component;
            }
            ++componentId;
        }

        if (components->Size())
            node->AddChild(components);
        else
            delete components;

        if (entity->Children().size())
        {
            FileArraySubNode* children = new FileArraySubNode();
            children->Name("Children");

            s32 childIndex = 0;
            // serialize children
            for (std::vector<Entity*>::const_iterator i = entity->Children().begin();
                i != entity->Children().end(); ++i)
            {
                // don't serialize non serializable stuff
                if (!(*i)->Serializable())
                    continue;
                FileObjectSubNode* child = new FileObjectSubNode();
                child->Name("Child");
                if (serializeEntityOptimized_(*i, child, index >= 0 ? childIndex : -1, index >= 0 ? master->ChildByIndex(childIndex) : 0) || index < 0)
                {
                    differencesFound = true;
                    children->AddChild(child);
                }
                else
                    delete child;
                ++childIndex;
            }

            if (children->Size())
                node->AddChild(children);
            else
                delete children;
        }
        
        return differencesFound;
    }

    //--------------------------------------------------------------------------

    void Scene::LoadImmediate( const DataLocation& data, bool threaded /*= false*/ )
    {
        if (!LoadLayout(data))
            return;

        if (threaded)
        {
            // start thread for loading shared
            // call sharedLoadFile_ from that thread
        }
        else
        {
            LoadFiles();
            LoadResources();
            LoadObjects();
        }
    }

    bool Scene::LoadLayout( const DataLocation& data )
    {
        return LoadLayout(data, poolFiles_);
    }

    bool Scene::LoadLayout( const DataLocation& data, const std::vector<std::string>& pools )
    {
        if (!sceneFile_.Read(data))
        {
            SWarn("Scene loaded with invalid data location.");
            return false;
        }

        // parse scene file and create pools
        initializePoolFilesFromScene_();
        // use the input vector instead of initialized pools
        createPoolsFromMemory_(pools); 

        return true;
    }

    void Scene::LoadFiles()
    {
        sharedLoadFile_();
    }

    void Scene::LoadResources()
    {
        sharedLoad_();

        // add entities to the scene (load anonymous on the fly)
        createScene_();
    }

    void Scene::LoadObjects()
    {
        awake_();
        init_();
    }

    void Scene::Serialize( const std::string& path, bool optimize )
    {
        FileRootSubNode* root = new FileRootSubNode();
        root->Name("Scene");
        
        // add pools involved.
        Simian::FileArraySubNode* pools = root->AddArray("Pools");
        for (Simian::u32 i = 0; i < poolFiles_.size(); ++i)
            pools->AddData("Pool", poolFiles_[i]);

        FileArraySubNode* entities = root->AddArray("Entities");
        for (u32 i = 0; i < rootEntities_.size(); ++i)
        {
            if (rootEntities_[i]->Serializable())
            {
                if (optimize)
                    serializeEntityOptimized_(rootEntities_[i], entities->AddObject("Entity"));
                else
                    serializeEntity_(rootEntities_[i], entities->AddObject("Entity"));
            }
        }

        // write to the file
        DataFileIODefault dataFile;
        dataFile.Root(root);
        dataFile.Write(path);
        delete root;
    }

    void Scene::Unload()
    {
        // unload everything (single-threaded for now)

        // deinit everything still attached (recurse the scene)
        for (u32 i = 0; i < rootEntities_.size(); ++i)
        {
            deinitRecursive_(rootEntities_[i]);
            sharedUnloadRecursive_(rootEntities_[i]);
        }

        // clear all unculled entities
        unculledEntities_.clear();

        // unload everything thats not in a pool
        sharedUnload_();
        sharedUnloadFile_();
        destroyPools_();
    }

    void Scene::Update( f32 dt )
    {
        processAddRemoveQueue_();

        // clear AABBs
        for (u32 i = 0; i < rootEntities_.size(); ++i)
            clearAABBRecursive_(rootEntities_[i]);

        // iterate through children and update every phase
        for (u32 i = 0; i < GameFactory::Instance().TotalPhases() - 1; ++i)
        {
            for (u32 j = 0; j < rootEntities_.size(); ++j)
                updateRecursive_(i, rootEntities_[j], dt);
        }

        strategy_->Cull();

        // render phase (last one) only applies to visible entities
        for (u32 i = 0; i < visibleEntities_.size(); ++i)
            visibleEntities_[i]->Update(P_RENDER, dt);
        
        // render all unculled entities
        for (u32 i = 0; i < unculledEntities_.size(); ++i)
            unculledEntities_[i]->Update(P_RENDER, dt);

        // make all entities invisible
        visibleEntities_.clear();
    }

    Entity* Scene::CreateEntity()
    {
        // anonymous entity
        Entity* entity = new Entity();
        entity->anonymous_ = true;
        return entity;
    }

    Entity* Scene::CreateEntity( u32 type )
    {
        // use the instancer list to create entities
        Entity* entity;
        bool success = pools_[type]->Create(entity);
        SAssert(success, "Entity creation failed.");
        success;
        return entity;
    }

    Entity* Scene::CreateEntity( const FileObjectSubNode& entityNode )
    {
        const FileDataSubNode* type = entityNode.Data("Type");
        
        Entity* ret = 0;

        // if it exist.. just use the other function to create from pool
        if (type)
        {
            // convert type to string
            u32 typeId = GameFactory::Instance().EntityTable().Value(type->AsString());
            ret = CreateEntity(typeId);
        }
        else
            // otherwise i am anonymous.
            ret = recurseAnonymousEntity_(entityNode);

        if (!ret)
            return ret;

        if (!ret->parentScene_)
            ret->parentScene_ = this;

        // unroll deserialize here..
        recurseComponentSettings_(entityNode, ret);

        return ret;
    }

    void Scene::AddEntity( Entity*& entity )
    {
        ARQueueObject object;
        object.Action = ARQueueObject::ACTION_ADD;
        object.Parent = NULL;
        object.Target = entity;
        addRemoveQueue_.push_back(object);
    }

    void Scene::RemoveEntity( Entity* entity )
    {
        for (u32 i = 0; i < entity->children_.size(); ++i)
            entity->RemoveChild(entity->children_[i]);

        ARQueueObject object;
        object.Action = ARQueueObject::ACTION_REMOVE;
        object.Parent = NULL;
        object.Target = entity;
        addRemoveQueue_.push_back(object);
    }

    Entity* Scene::EntityByName( const std::string& name )
    {
        for (u32 i = 0; i < rootEntities_.size(); ++i)
            if (rootEntities_[i]->Name() == name)
                return rootEntities_[i];
        return NULL;
    }

    Entity* Scene::EntityByType( u32 type )
    {
        for (u32 i = 0; i < rootEntities_.size(); ++i)
            if (rootEntities_[i]->Type() == type && !rootEntities_[i]->Anonymous())
                return rootEntities_[i];
        return NULL;
    }

    Entity* Scene::EntityByNameRecursive( const std::string& name )
    {
        for (u32 i = 0; i < rootEntities_.size(); ++i)
        {
            Entity* ent = entityByNameRecursive_(rootEntities_[i], name);
            if (ent)
                return ent;
        }
        return NULL;
    }

    Entity* Scene::EntityByTypeRecursive( u32 type )
    {
        for (u32 i = 0; i < rootEntities_.size(); ++i)
        {
            Entity* ent = entityByTypeRecursive_(rootEntities_[i], type);
            if (ent)
                return ent;
        }
        return NULL;
    }

    Entity* Scene::EntityByIndex( u32 index )
    {
        return rootEntities_[index];
    }

    void Scene::EntitiesByName( const std::string& name, EntityList& list )
    {
        list.clear();
        for (u32 i = 0; i < rootEntities_.size(); ++i)
            if (rootEntities_[i]->Name() == name)
                list.push_back(rootEntities_[i]);
    }

    void Scene::EntitiesByType( u32 type, EntityList& list )
    {
        list.clear();
        for (u32 i = 0; i < rootEntities_.size(); ++i)
            if (rootEntities_[i]->Type() == type)
                list.push_back(rootEntities_[i]);
    }

    void Scene::AddVisibleEntity( Entity* entity )
    {
        visibleEntities_.push_back(entity);
    }

    void Scene::LinkPool( const std::string& path )
    {
        std::vector<std::string>::iterator i = std::find(poolFiles_.begin(), poolFiles_.end(), path);
        if (i == poolFiles_.end())
            poolFiles_.push_back(path);
    }

    void Scene::UnlinkPool( const std::string& path )
    {
        std::vector<std::string>::iterator i = std::find(poolFiles_.begin(), poolFiles_.end(), path);
        if (i != poolFiles_.end())
            poolFiles_.erase(i);
    }
}
