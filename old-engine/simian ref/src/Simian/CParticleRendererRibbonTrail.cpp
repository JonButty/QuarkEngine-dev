/************************************************************************/
/*!
\file		CParticleRendererRibbonTrail.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/CParticleRendererRibbonTrail.h"
#include "Simian/MaterialCache.h"
#include "Simian/EngineComponents.h"
#include "Simian/ParticleRendererRibbonTrail.h"
#include "Simian/ParticleSystem.h"
#include "Simian/CParticleSystem.h"

namespace Simian
{
    MaterialCache* CParticleRendererRibbonTrail::materialCache_ = 0;
    GraphicsDevice* CParticleRendererRibbonTrail::graphicsDevice_ = 0;

    FactoryPlant<EntityComponent, CParticleRendererRibbonTrail> CParticleRendererRibbonTrail::factoryPlant_(
        &GameFactory::Instance().ComponentFactory(), C_PARTICLERENDERERRIBBONTRAIL);

    CParticleRendererRibbonTrail::CParticleRendererRibbonTrail()
        : renderer_(0),
          particleSystem_(0),
          material_(0),
          segmentCount_(2),
          interpolationInterval_(0.1f),
          segmentTwist_(0),
          segmentTwistVelocity_(0),
          local_(false)
    {
    }

    //--------------------------------------------------------------------------

    ParticleRendererRibbonTrail* CParticleRendererRibbonTrail::Renderer() const
    {
        return renderer_;
    }

    Simian::MaterialCache* CParticleRendererRibbonTrail::MaterialCache()
    {
        return materialCache_;
    }

    void CParticleRendererRibbonTrail::MaterialCache( Simian::MaterialCache* val )
    {
        materialCache_ = val;
    }

    Simian::GraphicsDevice* CParticleRendererRibbonTrail::GraphicsDevice()
    {
        return graphicsDevice_;
    }

    void CParticleRendererRibbonTrail::GraphicsDevice( Simian::GraphicsDevice* val )
    {
        graphicsDevice_ = val;
    }

    //--------------------------------------------------------------------------

    void CParticleRendererRibbonTrail::initRenderer_()
    {
        renderer_->SegmentCount(segmentCount_);
        renderer_->InterpolationInterval(interpolationInterval_);
        renderer_->SegmentTwist(segmentTwist_);
        renderer_->SegmentTwistVelocity(segmentTwistVelocity_);
        renderer_->Local(local_);
    }

    //--------------------------------------------------------------------------

    void CParticleRendererRibbonTrail::OnSharedLoad()
    {
        material_ = materialCache_->Load(materialLocation_);
    }

    void CParticleRendererRibbonTrail::OnAwake()
    {
        ComponentByType(C_PARTICLESYSTEM, particleSystem_);
        if (!particleSystem_)
            return;
        
        renderer_ = particleSystem_->ParticleSystem()->AddRenderer<ParticleRendererRibbonTrail>();
        renderer_->Device(graphicsDevice_);
        renderer_->Material(material_);

        initRenderer_();
    }

    void CParticleRendererRibbonTrail::Serialize( FileObjectSubNode& data )
    {
        data.AddData("MaterialFile", materialLocation_);
        data.AddData("SegmentCount", segmentCount_);
        data.AddData("InterpolationInterval", interpolationInterval_);
        data.AddData("SegmentTwist", segmentTwist_);
        data.AddData("SegmentTwistVelocity", segmentTwistVelocity_);
        data.AddData("Local", local_);
    }

    void CParticleRendererRibbonTrail::Deserialize( const FileObjectSubNode& data )
    {
        data.Data("MaterialFile", materialLocation_, materialLocation_);
        data.Data("SegmentCount", segmentCount_, segmentCount_);
        data.Data("InterpolationInterval", interpolationInterval_, interpolationInterval_);
        data.Data("SegmentTwist", segmentTwist_, segmentTwist_);
        data.Data("SegmentTwistVelocity", segmentTwistVelocity_, segmentTwistVelocity_);
        data.Data("Local", local_, local_);

        if (renderer_)
        {
            initRenderer_();

            // reload renderer
            renderer_->Unload();
            renderer_->Load();
        }
    }
}
