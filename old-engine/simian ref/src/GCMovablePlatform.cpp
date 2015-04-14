/*************************************************************************/
/*!
\file		GCMovablePlatform.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/09/30
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*************************************************************************/
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/CTransform.h"
#include "Simian/Scene.h"
#include "Simian/Entity.h"
#include "Simian/EntityComponent.h"
#include "Simian/Utility.h"
#include "Simian/Mouse.h"
#include "Simian/Keyboard.h"
#include "Simian/Ray.h"
#include "Simian/Plane.h"
#include "Simian/Game.h"
#include "Simian/Interpolation.h"
#include "Simian/CSoundEmitter.h"

#include "EntityTypes.h"
#include "ComponentTypes.h"
#include "Collision.h"

#include "GCPhysicsController.h"
#include "GCPlayerLogic.h"
#include "GCMovablePlatform.h"
#include "GCMousePlaneIntersection.h"
#include "GCCursor.h"
#include "GCUI.h"
#include "GCEditor.h"
#include "GCDescensionCamera.h"
#include "GCCinematicUI.h"
#include "GCGesturePuzzle.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCMovablePlatform>
        GCMovablePlatform::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_MOVABLEPLATFORM);
    
    static const Simian::f32 WATER_PARTICLES_MAX_TIMER = 0.5f;
    static const Simian::f32 WATER_SOUND_MAX_TIMER = 1.0f;
    static const Simian::u32 TOTAL_WATER_SOUNDS = 4;

    GCMovablePlatform::GCMovablePlatform()
        : bitFlag_(0),
          transform_(0),
          physics_(0),
          speed_(0),
          rotSpeed_(0),
          waterParticlesTimer_(0),
          waterSoundTimer_(WATER_SOUND_MAX_TIMER),
          sounds_(0),
          radius_(0)
    {
    }

    //--------------------------------------------------------------------------
    Simian::CTransform& GCMovablePlatform::Transform()
    {
        return *transform_;
    }

    //--------------------------------------------------------------------------
    void GCMovablePlatform::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        if (GCPlayerLogic::Instance()->PlayerControlOverride() &&
            GCGestureInterpreter::Instance()->PuzzleModeID() != GCGesturePuzzle::GPUZZLE_NONE)
            SFlagSet(bitFlag_, PLATFORM_ENABLED);
        else
            SFlagUnset(bitFlag_, PLATFORM_ENABLED);
        
        if (!SIsFlagSet(bitFlag_, PLATFORM_ENABLED) ||
            GCUI::GameStatus()==GCUI::GAME_PAUSED ||
            SIsFlagSet(bitFlag_, PLATFORM_DISABLED) )
            return;

        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        static Simian::CTransform* spiritTransform;
        static Simian::Vector3 offset;

        static Simian::f32 intp = 0.0;
        vSpeed_ -= speed_*dt;
        vSpeed_ = Simian::Math::Clamp<Simian::f32>(vSpeed_, 0.0f, speed_);
        physics_->Velocity(direction_*vSpeed_);

        Simian::Vector2 mouseVec = Simian::Mouse::GetMouseNormalizedPosition();
        Simian::Vector3 clickSpot;
        if (Simian::Plane::Z.Intersect(
            Simian::Ray::FromProjection(cameraTransform_->World(),
            camera_->Camera().Projection(), mouseVec), clickSpot))
        {
            //--If clicked, check if close enough
            if (Simian::Mouse::IsTriggered(Simian::Mouse::KEY_LBUTTON))
            {
                Simian::f32 dist = (clickSpot-physics_->Position()).LengthSquared();
                //--Enable moving
                if (dist < physics_->Radius()*physics_->Radius())
                {
                    SFlagSet(bitFlag_, PLATFORM_ACTIVE);
                    spiritTransform = spirit_->Transform();
                    offset = spiritTransform->Position() - physics_->Position();
                    GCCursor::Instance()->ShowBoth(true);
                    GCCursor::Instance()->Spirit()->Active(false);
                    GCCursor::Instance()->Spirit()->InterpolateToPos(true);
                    return;
                } else {
                    SFlagUnset(bitFlag_, PLATFORM_ACTIVE);
                    physics_->SetBitFlags(GCPhysicsController::PHY_PROP);
                    physics_->UnsetBitFlags(GCPhysicsController::PHY_PLAYER);
                }
            }
        }

        if (!SIsFlagSet(bitFlag_, PLATFORM_ACTIVE)) //--NOT SELECTED
            return;
        if (!Simian::Mouse::IsPressed(Simian::Mouse::KEY_LBUTTON))
        {
            SFlagUnset(bitFlag_, PLATFORM_ACTIVE);
            GCCursor::Instance()->ShowBoth(false);
            GCCursor::Instance()->Spirit()->Active(true);
            return;
        }
        physics_->SetBitFlags(GCPhysicsController::PHY_PLAYER);
        physics_->UnsetBitFlags(GCPhysicsController::PHY_PROP);
        SpiritSwirlEffect(dt);
        direction_ = (clickSpot - physics_->Position()).Normalized();
        if ((clickSpot-physics_->Position()).LengthSquared() > speed_*0.25f)
            vSpeed_ += 2*dt*speed_;

        waterParticlesTimer_ += dt;

        if (waterParticlesTimer_ > WATER_PARTICLES_MAX_TIMER)
        {
            waterParticlesTimer_ = 0.0f;
            // Create water particles
            Simian::Entity* waterParticles = ParentEntity()->ParentScene()->CreateEntity(E_PUZZLE_LEVERONPLATFORM_WATEREFFECT);
            ParentEntity()->ParentScene()->AddEntity(waterParticles);
            // Set water particles transform
            Simian::CTransform* waterTransform_;
            waterParticles->ComponentByType(Simian::C_TRANSFORM, waterTransform_);
            waterTransform_->Position(Simian::Vector3(transform_->Position().X(),
                -0.5f, transform_->Position().Z()));
        }
        waterSoundTimer_ += dt;

        if (waterSoundTimer_ > WATER_SOUND_MAX_TIMER)
        {
            waterSoundTimer_ = 0.0f;
             if (sounds_)
                sounds_->Play(Simian::Math::Random(1, 1+TOTAL_WATER_SOUNDS));
        }
    }

    void GCMovablePlatform::SpiritSwirlEffect(Simian::f32 dt)
    {
        static Simian::f32 angle = 0.0f;
        GCMousePlaneIntersection* spirit = GCCursor::Instance()->Spirit();
        //--Set final position
        static Simian::f32 xPos = 0.0f, zPos = 0.0f;
        angle += rotSpeed_*dt/Simian::Math::PI;
        xPos = physics_->Position().X() + radius_*std::cos(angle)
            - radius_*std::sin(angle);
        zPos = physics_->Position().Z() + radius_*std::sin(angle)
            + radius_*std::cos(angle);
        spirit->SetTargetPos(Simian::Vector3(xPos,
            spirit->Transform()->Position().Y(), zPos));
    }

    //--------------------------------------------------------------------------

    void GCMovablePlatform::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate
            <GCMovablePlatform, &GCMovablePlatform::update_>(this));
    }

    void GCMovablePlatform::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(Simian::C_SOUNDEMITTER, sounds_);
        ComponentByType(GC_PHYSICSCONTROLLER, physics_);
        bitFlag_ = 0;
    }

    void GCMovablePlatform::OnInit()
    {
        camera_ = GCPlayerLogic::Instance()->Camera();
        camera_->ComponentByType(Simian::C_TRANSFORM, cameraTransform_);

        Simian::Entity* ent;
        ent = ParentScene()->EntityByType(E_SPIRITOFDEMAE);
        ent->ComponentByType(GC_MOUSEPLANEINTERSECTION, spirit_);
    }

    void GCMovablePlatform::OnDeinit()
    {
        //--
    }

    void GCMovablePlatform::OnTriggered(Simian::DelegateParameter&)
    {
        //--
    }

    void GCMovablePlatform::Serialize( Simian::FileObjectSubNode& data)
    {
        data.AddData("Speed", speed_);
        data.AddData("RotationSpeed", rotSpeed_);
        data.AddData("SpiritRadius", radius_);
    }

    void GCMovablePlatform::Deserialize( const Simian::FileObjectSubNode& data)
    {
        data.Data("Speed", speed_, speed_);
        data.Data("RotationSpeed", rotSpeed_, rotSpeed_);
        data.Data("SpiritRadius", radius_, radius_);
    }
}
