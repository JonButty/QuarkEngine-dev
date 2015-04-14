/*************************************************************************/
/*!
\file		GCTrapLaserBeam.cpp
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Simian/EngineComponents.h"
#include "Simian/EnginePhases.h"
#include "Simian/CTransform.h"
#include "Simian/Scene.h"
#include "Simian/Entity.h"
#include "Simian/Interpolation.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"
#include "Simian/CSoundEmitter.h"

#include "EntityTypes.h"
#include "ComponentTypes.h"
#include "Collision.h"
#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"

#include "GCTrapLaserBeam.h"
#include "GCPhysicsController.h"
#include "GCAttributes.h"
#include "GCPlayerLogic.h"
#include "GCEditor.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCTrapLaserBeam> GCTrapLaserBeam::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_TRAPLASERBEAM);
    
    const Simian::f32 START_Y = 0.35f;
    const Simian::f32 LASER_SOUND_TIMER_MAX = 2.0f;

    GCTrapLaserBeam::GCTrapLaserBeam()
        : transform_(0),
          lightningBeamTransform_(0),
          lightningBeam_(0),
          playerTransform_(0),
          attributes_(0),
          playerAttributes_(0),
          fireParticles1_(0),
          fireParticles2_(0),
          laserParticles_(0),
          sounds_(0),
          startAngle_(0),
          finalAngle_(0),
          beamTimer_(0),
          laserSoundTimer_(LASER_SOUND_TIMER_MAX),
          rotateSpanTime_(3.0f),
          spanAngle_(17.0f),
          midX_(1.0f),
          midZ_(0.0f),
          offsetX_(0.3f),
          offsetZ_(0.0f),
          offsetRotation_(270.0f),
          hitTimer_(0),
          hitDelay_(1.5f),
          turnPauseTimer_(-1.0f),
          turnPauseTimeMax_(2.0f),
          laserDamage_(30),
          enabled_(false)
    {
    }

    //--------------------------------------------------------------------------

    Simian::CTransform& GCTrapLaserBeam::Transform()
    {
        return *transform_;
    }

    bool GCTrapLaserBeam::Enabled() const
    {
        return enabled_;
    }

    void GCTrapLaserBeam::Enabled(bool val)
    {
        enabled_ = val;
        // Play particles
        fireParticles1_->ParticleSystem()->Enabled(enabled_);
        fireParticles2_->ParticleSystem()->Enabled(enabled_);
        laserParticles_->ParticleSystem()->Enabled(enabled_);
    }

    //--------------------------------------------------------------------------

    void GCTrapLaserBeam::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif

        // Don't execute anything if trap is not enabled
        if (!enabled_)
            return;

        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        
        hitTimer_ += dt;
        
        Simian::f32 intp = beamTimer_/rotateSpanTime_;
        intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);

        Simian::f32 angle = 0.0f;
        // If it is not at the end of a swing
        if (turnPauseTimer_ < 0.0f)
        {
            beamTimer_ += dt;
            if(beamTimer_<= rotateSpanTime_)
            {
                Simian::f32 intp = beamTimer_/rotateSpanTime_;
                intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
                angle = Simian::Interpolation::Interpolate
                    <Simian::f32>(startAngle_,finalAngle_,intp,Simian::Interpolation::EaseSquareInOut);
                transform_->Rotation(Simian::Vector3(0, -1, 0), Simian::Degree(angle + 90.0f));
                lightningBeamTransform_->Rotation(Simian::Vector3(0, -1, 0), Simian::Degree(angle + 270.0f));
            }
            else
                turnPauseTimer_ = turnPauseTimeMax_;
        }
        // Else if it is at the end of a swing
        else
        {
            turnPauseTimer_ -= dt;
            if (turnPauseTimer_ < 0.0f)
            {
                turnPauseTimer_ = -1.0f;
                beamTimer_ = 0.0f;
                Simian::f32 temp = startAngle_;
                startAngle_ = finalAngle_;
                finalAngle_ = temp;
            }
        }

        lightningBeamTransform_->Position(Simian::Vector3(transform_->Position().X()+offsetX_,START_Y,transform_->Position().Z()+offsetZ_));

        Simian::Vector3 vecToPlayer_(playerTransform_->World().Position() - transform_->World().Position());
        Simian::f32 dotProduct = transform_->Direction().Dot(vecToPlayer_.Normalized());
        if( (hitTimer_ > hitDelay_ ) && ((1.0f - std::abs(dotProduct)) < Simian::Math::EPSILON) )
        {
            CombatCommandSystem::Instance().AddCommand(attributes_, playerAttributes_,
                CombatCommandDealDamage::Create(laserDamage_));
            if (!GCPlayerLogic::Instance()->DodgeEffective())
                GCPlayerLogic::Instance()->Sounds()->Play(GCPlayerLogic::Instance()->S_LIGHTNING_GET_HIT);
            hitTimer_ = 0.0f;
        }

        laserSoundTimer_ += dt;
        if (laserSoundTimer_ > LASER_SOUND_TIMER_MAX)
        {
            laserSoundTimer_ = 0.0f;
            if (sounds_)
                sounds_->Play(0);
        }

    }

    Simian::f32 GCTrapLaserBeam::vectorToDegreeFloat_(const Simian::Vector3& vec)
    {
        // Get angle of rotation in degrees for vector
        if(vec.LengthSquared() > Simian::Math::EPSILON)
        {
            Simian::Vector3 tempVec = vec.Normalized();
            Simian::f32 angle = std::acosf(tempVec.X());
            if (tempVec.Z() > 0)
                angle = Simian::Math::TAU - angle;
            // Wrap angle within 360
            angle = Simian::Math::WrapAngle(Simian::Radian(angle)).Degrees();

            return angle;
        }
        return 0;
    }

    //--------------------------------------------------------------------------

    void GCTrapLaserBeam::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCTrapLaserBeam, &GCTrapLaserBeam::update_>(this));
    }

    void GCTrapLaserBeam::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(GC_ATTRIBUTES, attributes_);
    }

    void GCTrapLaserBeam::OnInit()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif

        lightningBeam_ = ParentEntity()->ParentScene()->CreateEntity(E_GHOSTLIGHTNINGBEAM);
        if( lightningBeam_ )
        {
            lightningBeam_->ComponentByType(Simian::C_TRANSFORM, lightningBeamTransform_);
            ParentScene()->AddEntity(lightningBeam_);
            if( lightningBeamTransform_ )
                lightningBeamTransform_->Position(-Simian::Vector3::UnitY);
        }
        Simian::f32 mid = vectorToDegreeFloat_(Simian::Vector3(midX_, 0.0f, midZ_));
        startAngle_ = mid + spanAngle_;
        finalAngle_ = mid - spanAngle_;

        playerAttributes_ = GCPlayerLogic::Instance()->Attributes();
        GCPlayerLogic::Instance()->ParentEntity()->ParentEntity()->ComponentByType(Simian::C_TRANSFORM,playerTransform_);
        
        ParentEntity()->ChildByName("FireParticles_1")->ComponentByType(Simian::C_PARTICLESYSTEM,fireParticles1_);
        ParentEntity()->ChildByName("FireParticles_2")->ComponentByType(Simian::C_PARTICLESYSTEM,fireParticles2_);
        ParentEntity()->ChildByName("LaserParticles")->ComponentByType(Simian::C_PARTICLESYSTEM,laserParticles_);

        lightningBeam_->ComponentByType(Simian::C_SOUNDEMITTER, sounds_);
    }

    void GCTrapLaserBeam::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("SpanRotateTime", rotateSpanTime_);
        data.AddData("SpanAngle", spanAngle_);
        data.AddData("HitDelay", hitDelay_);
        data.AddData("Damage", laserDamage_);
        data.AddData("MidX", midX_);
        data.AddData("MidZ", midZ_);
        data.AddData("OffsetX", offsetX_);
        data.AddData("OffsetZ", offsetZ_);
        data.AddData("Enabled", enabled_);
        data.AddData("TurnPauseTimeMax", turnPauseTimeMax_);
    }

    void GCTrapLaserBeam::Deserialize( const Simian::FileObjectSubNode& data )
    {
        // get the data
        const Simian::FileDataSubNode* modelLocation = data.Data("SpanRotateTime");
        if (modelLocation) rotateSpanTime_ = modelLocation->AsF32();
        beamTimer_ = rotateSpanTime_/2.0f;

        modelLocation = data.Data("SpanAngle");
        if (modelLocation) spanAngle_ = modelLocation->AsF32();

        modelLocation = data.Data("HitDelay");
        if (modelLocation) hitDelay_ = modelLocation->AsF32();

        modelLocation = data.Data("Damage");
        if (modelLocation) laserDamage_ = static_cast<Simian::u32>(modelLocation->AsF32());

        modelLocation = data.Data("MidX");
        if (modelLocation) midX_ = modelLocation->AsF32();

        modelLocation = data.Data("MidZ");
        if (modelLocation) midZ_ = modelLocation->AsF32();

        modelLocation = data.Data("OffsetX");
        if (modelLocation) offsetX_ = modelLocation->AsF32();

        modelLocation = data.Data("OffsetZ");
        if (modelLocation) offsetZ_ = modelLocation->AsF32();

        modelLocation = data.Data("Enabled");
        if (modelLocation) enabled_ = modelLocation->AsBool();

        modelLocation = data.Data("TurnPauseTimeMax");
        if (modelLocation) turnPauseTimeMax_ = modelLocation->AsF32();
    }
}
