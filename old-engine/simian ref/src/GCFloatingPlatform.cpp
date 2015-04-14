/*************************************************************************/
/*!
\file		GCFloatingPlatform.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/10/11
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*************************************************************************/
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/CTransform.h"
#include "Simian/Entity.h"
#include "Simian/EntityComponent.h"
#include "Simian/Scene.h"
#include "Simian/Interpolation.h"
#include "Simian/Math.h"
#include "Simian/CSoundEmitter.h"

#include "EntityTypes.h"
#include "ComponentTypes.h"
#include "Collision.h"

#include "GCPhysicsController.h"
#include "GCFloatingPlatform.h"
#include "GCEditor.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCFloatingPlatform>
        GCFloatingPlatform::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_FLOATINGPLATFORM);
    
    GCFloatingPlatform::GCFloatingPlatform()
        : physics_(0), speed_(0), height_(0), intStart_(0), intEnd_(0), intp_(0),
          transform_(0), sink_(false), sounds_(0), wasSinking_(false)
    {  }

    //--------------------------------------------------------------------------
    void GCFloatingPlatform::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        if (sink_)
        {
            if (!wasSinking_)
                startInterpolate(-height_);
        } else {
            if (wasSinking_)
                startInterpolate(0.0f);
        }

        intp_ += dt*speed_;
        intp_ = Simian::Math::Clamp<Simian::f32>(intp_, 0.0f, 1.0f);
        Simian::f32 yPos = Simian::Interpolation::Interpolate<Simian::f32>
            (intStart_, intEnd_, intp_, Simian::Interpolation::EaseCubicInOut);
        physics_->Enabled(false);
        transform_->Position( Simian::Vector3(physics_->Position().X(),
            yPos, physics_->Position().Z()) );
        physics_->Enabled(true);
    }

    void GCFloatingPlatform::startInterpolate(Simian::f32 target)
    {
        intp_ = 0.0f;
        intStart_ = physics_->Position().Y();
        intEnd_ = target;
        wasSinking_ = sink_;
    }

    //--------------------------------------------------------------------------
    void GCFloatingPlatform::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate
            <GCFloatingPlatform, &GCFloatingPlatform::update_>(this));
    }

    void GCFloatingPlatform::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(GC_PHYSICSCONTROLLER, physics_);
        ComponentByType(Simian::C_SOUNDEMITTER, sounds_);
    }

    void GCFloatingPlatform::OnInit()
    {
        wasSinking_ = !sink_;
    }

    void GCFloatingPlatform::OnDeinit()
    {
        //--nothing
    }

    void GCFloatingPlatform::OnTriggered(Simian::DelegateParameter&)
    {
        sink_ = !sink_;
        // Create water particles
        Simian::Entity* waterParticles = ParentEntity()->ParentScene()->CreateEntity(E_PUZZLE_LEVERONPLATFORM_2_WATEREFFECT);
        ParentEntity()->ParentScene()->AddEntity(waterParticles);
        // Set water particles transform
        Simian::CTransform* waterTransform_;
        waterParticles->ComponentByType(Simian::C_TRANSFORM, waterTransform_);
        waterTransform_->Position(Simian::Vector3(transform_->Position().X(),
                -1.0f, transform_->Position().Z()));
        if (sounds_)
            sounds_->Play(1);
    }

    void GCFloatingPlatform::Serialize( Simian::FileObjectSubNode& data)
    {
        data.AddData("Speed", speed_);
        data.AddData("Height", height_);
        data.AddData("Sink", sink_);
    }

    void GCFloatingPlatform::Deserialize( const Simian::FileObjectSubNode& data)
    {
        data.Data("Speed", speed_, speed_);
        data.Data("Height", height_, height_);
        data.Data("Sink", sink_, sink_);
    }
}
