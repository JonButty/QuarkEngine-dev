/************************************************************************/
/*!
\file		CModel.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/CModel.h"
#include "Simian/Model.h"
#include "Simian/EngineComponents.h"
#include "Simian/CTransform.h"
#include "Simian/LogManager.h"
#include "Simian/ModelCache.h"
#include "Simian/MaterialCache.h"
#include "Simian/EnginePhases.h"
#include "Simian/AnimationController.h"
#include "Simian/RenderQueue.h"
#include "Simian/Joint.h"
#include "Simian/Utility.h"
#include "Simian/CCamera.h"
#include "Simian/Camera.h"

namespace Simian
{
    Simian::ModelCache* CModel::modelCache_ = 0;
    Simian::MaterialCache* CModel::materialCache_ = 0;
    Simian::RenderQueue* CModel::renderQueue_ = 0;
    FactoryPlant<EntityComponent, CModel> CModel::factoryPlant_(&GameFactory::Instance().ComponentFactory(), C_MODEL);

    CModel::CModel()
        : transform_(0),
          model_(0),
          material_(0),
          controller_(0),
          layers_(RenderQueue::RL_SCENE),
          visible_(true)
    {
    }

    //--------------------------------------------------------------------------

    CTransform& CModel::Transform() const
    {
        return *transform_;
    }

    Simian::Model& CModel::Model() const
    {
        return *model_;
    }

    Simian::Material& CModel::Material() const
    {
        return *material_;
    }

    AnimationController& CModel::Controller() const
    {
        return *controller_;
    }

    const std::string& CModel::ModelLocation() const
    {
        return modelLocation_;
    }

    void CModel::ModelLocation( const std::string& val )
    {
        modelLocation_ = val;
    }

    const std::string& CModel::AnimationLocation() const
    {
        return animationLocation_;
    }

    void CModel::AnimationLocation( const std::string& val )
    {
        animationLocation_ = val;
    }

    const std::string& CModel::MaterialLocation() const
    {
        return materialLocation_;
    }

    void CModel::MaterialLocation( const std::string& val )
    {
        materialLocation_ = val;
    }

    Simian::ModelCache* CModel::ModelCache()
    {
        return modelCache_;
    }

    void CModel::ModelCache( Simian::ModelCache* val )
    {
        modelCache_ = val;
    }

    Simian::MaterialCache* CModel::MaterialCache()
    {
        return materialCache_;
    }

    void CModel::MaterialCache( Simian::MaterialCache* val )
    {
        materialCache_ = val;
    }

    Simian::RenderQueue* CModel::RenderQueue()
    {
        return renderQueue_;
    }

    void CModel::RenderQueue( Simian::RenderQueue* val )
    {
        renderQueue_ = val;
    }

    void CModel::DrawCallback( const Delegate& val )
    {
        drawCallback_ += val;
    }

    void CModel::EndDrawCallback( const Delegate& val )
    {
        endDrawCallback_ += val;
    }

    bool CModel::Visible() const
    {
        return visible_;
    }

    void CModel::Visible( bool val )
    {
        visible_ = val;
    }

    //--------------------------------------------------------------------------

    void CModel::onUpdate_( DelegateParameter& param )
    {
        if (controller_)
        {
            float dt = param.As<EntityUpdateParameter>().Dt;
            controller_->Update(dt);
            
            // update the transforms of all attached objects
            for (u32 i = 0; i < attachedEntities_.size(); ++i)
            {
                // assuming an object with identity.. transform the object by the bone matrix
                Simian::Matrix transform(Simian::Matrix::Identity);

                // transform object by bone matrix
                const Simian::Matrix& boneMatrix = 
                    model_->Skeleton()->RootTransformInversed() * 
                    attachedEntities_[i].first->World();
                // transform of object in world space
                transform = transform_->World() * boneMatrix * transform;

                // transform the object back to its local space
                if (attachedEntities_[i].second->ParentTransform())
                    transform = 
                        attachedEntities_[i].second->ParentTransform()->World().Inversed() * 
                        transform;

                // decompose my transformation..
                Vector3 pos, scale;
                Vector4 rotation;
                transform.Decompose(pos, rotation, scale);

                // set the position
                attachedEntities_[i].second->Position(pos);

                // form the rotation from quaternion
                f32 denom = 1.0f/std::sqrt(1.0f - rotation.W() * rotation.W());
                attachedEntities_[i].second->Rotation(
                    Simian::Vector3(rotation.X() * denom, 
                                    rotation.Y() * denom, 
                                    rotation.Z() * denom),
                    Simian::Radian(2.0f * std::acos(rotation.W())));
            }
        }

        // update the entity's AABB
        Vector3 min = model_->Min();
        Vector3 max = model_->Max();
        AABB aabb(min, max);
        aabb.TransformRigid(transform_->World());
        ParentEntity()->AddAABB(aabb);
    }

    void CModel::onRender_( DelegateParameter& )
    {   
        if (!visible_)
            return;

        renderQueue_->AddRenderObject(Simian::RenderObject(
            *material_, layers_, transform_->World(),
            Simian::Delegate::CreateDelegate<CModel, &CModel::drawModel_>(this), 
            NULL));
    }

    void CModel::drawModel_( DelegateParameter& )
    {
        drawCallback_();
        model_->Draw(controller_);
        endDrawCallback_();
    }

    //--------------------------------------------------------------------------

    void CModel::OnSharedLoad()
    {
        // check if we really can't go on..
        if (!modelCache_ || !renderQueue_ || !materialCache_)
        {
            SError("Model component requires valid model cache, material cache and render queue.");
            return;
        }
        
        model_ = modelCache_->Load(modelLocation_, animationLocation_);
        material_ = materialCache_->Load(materialLocation_);

        RegisterCallback(P_UPDATE, Delegate::CreateDelegate<CModel, &CModel::onUpdate_>(this));
        RegisterCallback(P_RENDER, Delegate::CreateDelegate<CModel, &CModel::onRender_>(this));
    }

    //--------------------------------------------------------------------------

    void CModel::OnAwake()
    {
        ComponentByType(C_TRANSFORM, transform_);
        if (animationLocation_ != "")
            controller_ = model_->CreateAnimationController();
    }

    void CModel::OnDeinit()
    {
        if (controller_)
            model_->DestroyAnimationController(controller_);
    }

    void CModel::AttachEntityToJoint( const std::string& jointName, Simian::Entity* entity )
    {
        // no skeletons
        if (!controller_)
            return;

        // find the joint to attach to
        AnimationJoint* joint = controller_->JointByName(jointName);

        // no such joint
        if (!joint)
            return;

        // create an entity joint pair
        CTransform* transform;
        entity->ComponentByType(C_TRANSFORM, transform);

        if (!transform)
            return;

        attachedEntities_.push_back(JointEntityPair(joint, transform));
    }

    void CModel::DetachEntityFromJoint( Simian::Entity* entity )
    {
        // look for an iterator that has this entity
        std::vector<JointEntityPair>::iterator i = 
            std::find_if(attachedEntities_.begin(), attachedEntities_.end(), 
                         FindAttachedEntity(entity));

        // if we found something, erase it!
        if (i != attachedEntities_.end())
            attachedEntities_.erase(i);
    }

    void CModel::Serialize( FileObjectSubNode& data )
    {
        data.AddData("ModelFile", modelLocation_);
        data.AddData("AnimationFile", animationLocation_);
        data.AddData("MaterialFile", materialLocation_);
        data.AddData("Visible", visible_);

        data.AddData("Layer", 
            layers_ == RenderQueue::RL_PRESCENE  ? "PRESCENE" :
            layers_ == RenderQueue::RL_SCENE     ? "SCENE" :
            layers_ == RenderQueue::RL_POSTSCENE ? "POSTSCENE" :
            layers_ == RenderQueue::RL_PREUI     ? "PREUI" :
            layers_ == RenderQueue::RL_UI        ? "UI" :
            layers_ == RenderQueue::RL_POSTUI    ? "POSTUI" : "SCENE");
    }

    void CModel::Deserialize( const FileObjectSubNode& data )
    {
        // get the 3 parameters
        const FileDataSubNode* modelLocation = data.Data("ModelFile");
        if (modelLocation)
            modelLocation_ = modelLocation->AsString();

        const FileDataSubNode* animLocation = data.Data("AnimationFile");
        if (animLocation)
            animationLocation_ = animLocation->AsString();

        const FileDataSubNode* materialLocation = data.Data("MaterialFile");
        if (materialLocation)
            materialLocation_ = materialLocation->AsString();

        data.Data("Visible", visible_, visible_);

        const FileDataSubNode* layer = data.Data("Layer");
        layers_ = layer ? static_cast<u8>(
            layer->AsString() == "PRESCENE"  ? RenderQueue::RL_PRESCENE : 
            layer->AsString() == "SCENE"     ? RenderQueue::RL_SCENE : 
            layer->AsString() == "POSTSCENE" ? RenderQueue::RL_POSTSCENE : 
            layer->AsString() == "PREUI"     ? RenderQueue::RL_PREUI : 
            layer->AsString() == "UI"        ? RenderQueue::RL_UI : 
            layer->AsString() == "POSTUI"    ? RenderQueue::RL_POSTUI : 
            RenderQueue::RL_SCENE) : layers_;
    }
}
