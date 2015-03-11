/************************************************************************/
/*!
\file		CParticleEmitterDisc.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/CParticleEmitterDisc.h"
#include "Simian/ParticleEmitterDisc.h"
#include "Simian/EngineComponents.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"

namespace Simian
{
    FactoryPlant<EntityComponent, CParticleEmitterDisc> CParticleEmitterDisc::factoryPlant_(
        &GameFactory::Instance().ComponentFactory(), C_PARTICLEEMITTERDISC);

    CParticleEmitterDisc::CParticleEmitterDisc()
        : emitter_(0),
          particleSystem_(0),
          radius_(0.0f),
          randomRadius_(0.0f),
          normal_(Vector3::UnitY),
          forward_(Vector3::UnitZ)
    {

    }

    //--------------------------------------------------------------------------

    ParticleEmitterDisc* CParticleEmitterDisc::Emitter() const
    {
        return emitter_;
    }

    Simian::f32 CParticleEmitterDisc::Radius() const
    {
        return radius_;
    }

    void CParticleEmitterDisc::Radius( Simian::f32 val )
    {
        radius_ = val;
        if (emitter_)
            emitter_->Radius(val);
    }

    Simian::f32 CParticleEmitterDisc::RandomRadius() const
    {
        return randomRadius_;
    }

    void CParticleEmitterDisc::RandomRadius( Simian::f32 val )
    {
        randomRadius_ = val;
        if (emitter_)
            emitter_->RandomRadius(val);
    }

    //--------------------------------------------------------------------------

    void CParticleEmitterDisc::OnAwake()
    {
        ComponentByType(C_PARTICLESYSTEM, particleSystem_);
        if (!particleSystem_)
            return;

        // add the emitter
        emitter_ = particleSystem_->ParticleSystem()->AddEmitter<ParticleEmitterDisc>();
        emitter_->Radius(radius_);
        emitter_->RandomRadius(randomRadius_);
        emitter_->Normal(normal_);
        emitter_->Forward(forward_);
    }

    void CParticleEmitterDisc::Serialize( FileObjectSubNode& data )
    {
        data.AddData("Radius", radius_);
        data.AddData("RandomRadius", randomRadius_);

        FileObjectSubNode* normal = data.AddObject("Normal");
        normal->AddData("X", normal_.X());
        normal->AddData("Y", normal_.Y());
        normal->AddData("Z", normal_.Z());

        FileObjectSubNode* forward = data.AddObject("Forward");
        forward->AddData("X", forward_.X());
        forward->AddData("Y", forward_.Y());
        forward->AddData("Z", forward_.Z());
    }

    void CParticleEmitterDisc::Deserialize( const FileObjectSubNode& data )
    {
        data.Data("Radius", radius_, radius_);
        data.Data("RandomRadius", randomRadius_, randomRadius_);

        const FileObjectSubNode* normal = data.Object("Normal");
        if (normal)
        {
            float x, y, z;
            normal->Data("X", x, normal_.X());
            normal->Data("Y", y, normal_.Y());
            normal->Data("Z", z, normal_.Z());
            normal_ = Vector3(x, y, z);
        }

        const FileObjectSubNode* forward = data.Object("Forward");
        if (forward)
        {
            float x, y, z;
            forward->Data("X", x, forward_.X());
            forward->Data("Y", y, forward_.Y());
            forward->Data("Z", z, forward_.Z());
            forward_ = Vector3(x, y, z);
        }

        if (!emitter_)
            return;

        emitter_->Radius(radius_);
        emitter_->RandomRadius(randomRadius_);
        emitter_->Normal(normal_);
        emitter_->Forward(forward_);
    }
}
