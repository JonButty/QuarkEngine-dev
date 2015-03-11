/************************************************************************/
/*!
\file		CModel.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CMODEL_H_
#define SIMIAN_CMODEL_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"
#include "Delegate.h"
#include "CTransform.h"

#include <algorithm>

namespace Simian
{
    class Model;
    class Material;
    class AnimationController;
    class ModelCache;
    class MaterialCache;
    class RenderQueue;
    class AnimationJoint;

    class SIMIAN_EXPORT CModel: public EntityComponent
    {
    private:
        typedef std::pair<AnimationJoint*, CTransform*> JointEntityPair;

        class FindAttachedEntity: std::unary_function<JointEntityPair, bool>
        {
        private:
            Entity* entity_;
        public:
            FindAttachedEntity(Entity* entity): entity_(entity) {}
            bool operator()(const JointEntityPair& a) { return a.second->ParentEntity() == entity_; }
        };
    private:
        CTransform* transform_;
        Model* model_;
        Material* material_;
        AnimationController* controller_;

        // deserialized data
        std::string modelLocation_;
        std::string animationLocation_;
        std::string materialLocation_;

        // attached entities
        std::vector<JointEntityPair> attachedEntities_;

        // layers
        u8 layers_;

        // static cache pointers
        static ModelCache* modelCache_;
        static MaterialCache* materialCache_;
        static RenderQueue* renderQueue_;

        bool visible_;

        DelegateList drawCallback_;
        DelegateList endDrawCallback_;

        // factory plant
        static FactoryPlant<EntityComponent, CModel> factoryPlant_;
    public:
        CTransform& Transform() const;

        Simian::Model& Model() const;

        Simian::Material& Material() const;

        AnimationController& Controller() const;

        const std::string& ModelLocation() const;
        void ModelLocation(const std::string& val);

        const std::string& AnimationLocation() const;
        void AnimationLocation(const std::string& val);

        const std::string& MaterialLocation() const;
        void MaterialLocation(const std::string&  val);

        static Simian::ModelCache* ModelCache();
        static void ModelCache(Simian::ModelCache* val);

        static Simian::MaterialCache* MaterialCache();
        static void MaterialCache(Simian::MaterialCache* val);

        static Simian::RenderQueue* RenderQueue();
        static void RenderQueue(Simian::RenderQueue* val);

        void DrawCallback(const Delegate& val);
        void EndDrawCallback(const Delegate& val);

        bool Visible() const;
        void Visible(bool val);
    private:
        void onUpdate_(DelegateParameter& param);
        void onRender_(DelegateParameter& param);
        void drawModel_(DelegateParameter& param);
    public:
        CModel();

        void OnSharedLoad();

        void OnAwake();
        void OnDeinit();

        void AttachEntityToJoint(const std::string& jointName, Simian::Entity* entity);
        void DetachEntityFromJoint(Simian::Entity* entity);

        void Serialize(FileObjectSubNode& data);
        void Deserialize(const FileObjectSubNode& data);
    };
}

#endif
