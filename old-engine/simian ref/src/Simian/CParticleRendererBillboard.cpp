/************************************************************************/
/*!
\file		CParticleRendererBillboard.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/CParticleRendererBillboard.h"
#include "Simian/EngineComponents.h"
#include "Simian/ParticleRendererBillboard.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"
#include "Simian/MaterialCache.h"
#include "Simian/CCamera.h"
#include "Simian/CCameraBase.h"
#include "Simian/EnginePhases.h"
#include "Simian/RenderQueue.h"

namespace Simian
{
    FactoryPlant<EntityComponent, CParticleRendererBillboard> CParticleRendererBillboard::factoryPlant_(
        &GameFactory::Instance().ComponentFactory(), C_PARTICLERENDERERBILLBOARD);
    Simian::GraphicsDevice* CParticleRendererBillboard::graphicsDevice_ = 0;
    Simian::MaterialCache* CParticleRendererBillboard::materialCache_;

    CParticleRendererBillboard::CParticleRendererBillboard()
        : renderer_(0),
          particleSystem_(0),
          material_(0),
          local_(false),
          velocityOrient_(false)
    {
    }

    //--------------------------------------------------------------------------

    ParticleRendererBillboard* CParticleRendererBillboard::Renderer() const
    {
        return renderer_;
    }

    Simian::GraphicsDevice* CParticleRendererBillboard::GraphicsDevice()
    {
        return graphicsDevice_;
    }

    void CParticleRendererBillboard::GraphicsDevice( Simian::GraphicsDevice* val )
    {
        graphicsDevice_ = val;
    }

    Simian::MaterialCache* CParticleRendererBillboard::MaterialCache()
    {
        return materialCache_;
    }

    void CParticleRendererBillboard::MaterialCache( Simian::MaterialCache* val )
    {
        materialCache_ = val;
    }

    //--------------------------------------------------------------------------

    void CParticleRendererBillboard::initRenderer_()
    {
        renderer_->Local(local_);
        renderer_->VelocityOrient(velocityOrient_);
    }

    void CParticleRendererBillboard::update_( DelegateParameter& )
    {
        if (particleSystem_->Layer() == RenderQueue::RL_SCENE)
        {
            CCamera* cam;
            CCameraBase::SceneCamera(cam);
            renderer_->Camera(&cam->Camera());
        }
        else
            renderer_->Camera(NULL);
    }

    //--------------------------------------------------------------------------

    void CParticleRendererBillboard::OnSharedLoad()
    {
        material_ = materialCache_->Load(materialLocation_);
        RegisterCallback(P_UPDATE, Delegate::CreateDelegate<CParticleRendererBillboard, &CParticleRendererBillboard::update_>(this));
    }

    void CParticleRendererBillboard::OnAwake()
    {
        ComponentByType(C_PARTICLESYSTEM, particleSystem_);
        if (!particleSystem_)
            return;

        renderer_ = particleSystem_->ParticleSystem()->AddRenderer<ParticleRendererBillboard>();
        renderer_->Device(graphicsDevice_);
        renderer_->Material(material_);

        initRenderer_();
    }

    void CParticleRendererBillboard::OnInit()
    {
        CCamera* cam = 0;
        CCameraBase::SceneCamera(cam);
        renderer_->Camera(&cam->Camera());
    }

    void CParticleRendererBillboard::Serialize( FileObjectSubNode& data )
    {
        data.AddData("MaterialFile", materialLocation_);
        data.AddData("Local", local_);
        data.AddData("VelocityOrient", velocityOrient_);
    }

    void CParticleRendererBillboard::Deserialize( const FileObjectSubNode& data )
    {
        data.Data("MaterialFile", materialLocation_, materialLocation_);
        data.Data("Local", local_, local_);
        data.Data("VelocityOrient", velocityOrient_, velocityOrient_);

        if (renderer_)
            initRenderer_();
    }
}
