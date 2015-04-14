/************************************************************************/
/*!
\file		CBoneTrail.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/CBoneTrail.h"
#include "Simian/MaterialCache.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/Scene.h"
#include "Simian/LogManager.h"
#include "Simian/EngineComponents.h"
#include "Simian/CModel.h"
#include "Simian/Model.h"
#include "Simian/AnimationController.h"
#include "Simian/EnginePhases.h"
#include "Simian/RenderQueue.h"

namespace Simian
{
    GraphicsDevice* CBoneTrail::device_ = 0;
    MaterialCache* CBoneTrail::materialCache_ = 0;
    RenderQueue* CBoneTrail::renderQueue_ = 0;

    FactoryPlant<EntityComponent, CBoneTrail> CBoneTrail::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), C_BONETRAIL);

    CBoneTrail::CBoneTrail()
        : segmentCount_(2),
          trailDuration_(1.0f),
          opacity_(1.0f),
          model_(0),
          vertexBuffer_(0),
          material_(0),
          trail_(2)
    {
    }

    CBoneTrail::~CBoneTrail()
    {

    }

    //--------------------------------------------------------------------------

    GraphicsDevice* CBoneTrail::Device()
    {
        return device_;
    }

    void CBoneTrail::Device( GraphicsDevice* val )
    {
        device_ = val;
    }

    Simian::MaterialCache* CBoneTrail::MaterialCache()
    {
        return materialCache_;
    }

    void CBoneTrail::MaterialCache( Simian::MaterialCache* val )
    {
        materialCache_ = val;
    }

    Simian::RenderQueue* CBoneTrail::RenderQueue()
    {
        return renderQueue_;
    }

    void CBoneTrail::RenderQueue( Simian::RenderQueue* val )
    {
        renderQueue_ = val;
    }

    Simian::f32 CBoneTrail::Opacity() const
    {
        return opacity_;
    }

    void CBoneTrail::Opacity( Simian::f32 val )
    {
        opacity_ = val;
    }

    AnimationJoint* CBoneTrail::TopJoint() const
    {
        return joints_[0];
    }

    AnimationJoint* CBoneTrail::BottomJoint() const
    {
        return joints_[1];
    }

    Simian::Vector3 CBoneTrail::TopJointPosition() const
    {
        return jointPosition_(joints_[0]);
    }

    Simian::Vector3 CBoneTrail::BottomJointPosition() const
    {
        return jointPosition_(joints_[1]);
    }

    //--------------------------------------------------------------------------

    Simian::Vector3 CBoneTrail::jointPosition_( AnimationJoint* joint ) const
    {
        Simian::Matrix boneMatrix = model_->Model().Skeleton()->RootTransformInversed()
            * joint->World();
        boneMatrix = model_->Transform().World() * boneMatrix;
        return boneMatrix.Position();
    }

    void CBoneTrail::updateVertexBuffer_()
    {
        // put all the vertices in the vertex vector
        u32 j = 0;

        for (u32 i = 0; i < segmentCount_; ++i)
        {
            const PointTrail::Segment& segment = trail_.Segments()[i];
            float u = i/(segmentCount_ - 1.0f);
            vertices_[j++] = PositionColorUVVertex(segment.Top, 
                Simian::Color(1.0f, 1.0f, 1.0f, segment.Opacity),
                Simian::Vector2(u, 0.0f));
            vertices_[j++] = PositionColorUVVertex(segment.Bottom, 
                Simian::Color(1.0f, 1.0f, 1.0f, segment.Opacity),
                Simian::Vector2(u, 1.0f));
        }
        vertexBuffer_->Data(&vertices_[0]);
    }

    void CBoneTrail::drawTrail_( Simian::DelegateParameter& )
    {
        device_->World(Simian::Matrix::Identity);
        device_->Draw(vertexBuffer_, GraphicsDevice::PT_TRIANGLESTRIP, 
                      segmentCount_ * 2 - 2);
    }

    void CBoneTrail::update_( Simian::DelegateParameter& param )
    {
        EntityUpdateParameter* p;
        param.As(p);

        // update trail
        trail_.Update(p->Dt, jointPosition_(joints_[0]), 
                      jointPosition_(joints_[1]), opacity_);

        updateVertexBuffer_();
    }

    void CBoneTrail::render_( Simian::DelegateParameter& )
    {
        renderQueue_->AddRenderObject(RenderObject(*material_, 
            Simian::RenderQueue::RL_SCENE, model_->Transform().World(),
            Simian::Delegate::CreateDelegate<CBoneTrail, &CBoneTrail::drawTrail_>(this), 0));
    }

    //--------------------------------------------------------------------------

    void CBoneTrail::OnSharedLoad()
    {
        RegisterCallback(P_UPDATE, Simian::Delegate::CreateDelegate<CBoneTrail, &CBoneTrail::update_>(this));
        RegisterCallback(P_RENDER, Simian::Delegate::CreateDelegate<CBoneTrail, &CBoneTrail::render_>(this));
    }

    void CBoneTrail::OnInit()
    {
        // try to load the material
        material_ = materialCache_->Load(materialFile_);

        // create the vertex buffer
        device_->CreateVertexBuffer(vertexBuffer_);
        vertexBuffer_->Load(PositionColorUVVertex::Format, segmentCount_ * 2, true);

        // find the target
        Entity* target = ParentEntity();
        if (target_ == "Parent")
            target = ParentEntity()->ParentEntity();
        else if (target_ != "")
            target = ParentScene()->EntityByNameRecursive(target_);
        if (target == NULL)
        {
            SWarn("CBoneTrail requires a target.");
        }

        // get the model component from the target
        target->ComponentByType(C_MODEL, model_);

        if (!&model_->Controller())
        {
            SWarn("CBoneTrail's model requires a controller.");
        }

        // get the joint positions of first joint and second joint
        joints_[0] = model_->Controller().JointByName(firstJoint_);
        joints_[1] = model_->Controller().JointByName(secondJoint_);

        if (!joints_[0] || !joints_[1])
        {
            SWarn("CBoneTrail's joint doesn't exist. " << joints_[0] << " or " 
                  << joints_[1]);
        }

        // top position
        Simian::Vector3 top = jointPosition_(joints_[0]);
        // bottom position
        Simian::Vector3 bottom = jointPosition_(joints_[1]);

        // resize the trail
        trail_.Duration(trailDuration_);
        trail_.Resize(segmentCount_);
        trail_.Reset(top, bottom, opacity_);

        // make room for the vertices
        vertices_.resize(segmentCount_ * 2);

        // update vertex buffer
        updateVertexBuffer_();
    }

    void CBoneTrail::OnDeinit()
    {
        // destroy the vertex buffer
        vertexBuffer_->Unload();
        device_->DestroyVertexBuffer(vertexBuffer_);
    }

    void CBoneTrail::Serialize( FileObjectSubNode& data )
    {
        data.AddData("Target", target_);
        data.AddData("FirstJoint", firstJoint_);
        data.AddData("SecondJoint", secondJoint_);
        data.AddData("MaterialFile", materialFile_);
        data.AddData("SegmentCount", segmentCount_);
        data.AddData("TrailDuration", trailDuration_);
    }

    void CBoneTrail::Deserialize( const FileObjectSubNode& data )
    {
        data.Data("Target", target_, target_);
        data.Data("FirstJoint", firstJoint_, firstJoint_);
        data.Data("SecondJoint", secondJoint_, secondJoint_);
        data.Data("MaterialFile", materialFile_, materialFile_);
        data.Data("SegmentCount", segmentCount_, segmentCount_);
        data.Data("TrailDuration", trailDuration_, trailDuration_);

        // update the trail
        trail_.Resize(segmentCount_);
        trail_.Duration(trailDuration_);
    }
}
