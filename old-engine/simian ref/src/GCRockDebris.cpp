/******************************************************************************/
/*!
\file		GCRockDebris.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/12/07
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "GCRockDebris.h"
#include "GCEditor.h"
#include "Simian\EnginePhases.h"
#include "ComponentTypes.h"
#include "GCAttributes.h"
#include "Simian\Delegate.h"
#include "GCPlayerLogic.h"
#include "Simian\Math.h"
#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"
#include "Simian\Scene.h"
#include "Simian\LogManager.h"
#include "EntityTypes.h"
#include "Simian\DebugRenderer.h"
#include "Simian\CSoundEmitter.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCRockDebris> GCRockDebris::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_ROCKDEBRIS);

    GCRockDebris::GCRockDebris()
        :   shadow_(0),
            rockTrans_(0),
            shadowTrans_(0),
            sounds_(0),
            attribs_(0),
            radius_(0),
            radiusSqr_(0),
            speed_(0),
            randAngularVel_(0),
            dShadowScale_(1,1,1),
            randRotationAxis_(Simian::Vector3::Zero)
    {
    }

    GCRockDebris::~GCRockDebris()
    {

    }

    //--------------------------------------------------------------------------

    void GCRockDebris::Radius( Simian::f32 val )
    {
        radius_ = val;
        radiusSqr_ = val * val;
    }

    Simian::f32 GCRockDebris::Speed() const
    {
        return speed_;
    }

    void GCRockDebris::Speed( Simian::f32 val )
    {
        speed_ = val;
    }

    Simian::f32 GCRockDebris::StartHeight() const
    {
        return startHeight_;
    }

    void GCRockDebris::StartHeight( Simian::f32 val )
    {
        startHeight_ = val;
    }

    //--------------------------------------------------------------------------

    void GCRockDebris::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        Simian::Vector3 pos(rockTrans_->Position());
        pos.Y(pos.Y() - (speed_ * dt));
        Simian::f32 distSqr =
            ( playerTrans_.WorldPosition() - pos).LengthSquared();
        if(pos.Y() < 0 || distSqr < radiusSqr_)
        {
            if(distSqr < radiusSqr_)
                dealDamage_();
            
            // spawn the rock breaking effect
            Simian::Entity* rockBreak = ParentScene()->CreateEntity(E_ROCKBREAKEFFECT);
            Simian::CTransform* transform;
            rockBreak->ComponentByType(Simian::C_TRANSFORM, transform);
            Simian::Vector3 pos = rockTrans_->WorldPosition();
            pos.Y(0.0f);
            transform->WorldPosition(pos);
            ParentScene()->AddEntity(rockBreak);

            if (sounds_)
                sounds_->Play(Simian::Math::Random(0, 2));

            ParentScene()->RemoveEntity(shadow_);
            ParentScene()->RemoveEntity(ParentEntity());
        }
        DebugRendererDrawCircle(pos,radius_,Simian::Color(0,0,1,1));
        rockTrans_->WorldPosition(pos);
        rockTrans_->Rotation(randRotationAxis_,Simian::Degree(rockTrans_->Angle().Degrees() + randAngularVel_ * dt));

        shadowTrans_->Scale(pos.Y() * dShadowScale_ + Simian::Vector3(2,2,2));
    }

    void GCRockDebris::dealDamage_()
    {
        CombatCommandSystem::Instance().AddCommand(attribs_, GCPlayerLogic::Instance()->Attributes(),
            CombatCommandDealDamage::Create(damage_));
    }

    //--------------------------------------------------------------------------
    
    void GCRockDebris::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCRockDebris, &GCRockDebris::update_>(this));
    }

    void GCRockDebris::OnInit()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif

        ParentEntity()->ComponentByType(Simian::C_TRANSFORM, rockTrans_);
        ParentEntity()->ComponentByType(Simian::C_SOUNDEMITTER, sounds_);
        shadow_ = ParentScene()->CreateEntity(E_SHADOW);
        shadow_->ComponentByType(Simian::C_TRANSFORM, shadowTrans_);
        ParentScene()->AddEntity(shadow_);

        ParentScene()->EntityByType(E_ENEMYCRYSTALGUARDIAN)->ComponentByType(GC_ATTRIBUTES, attribs_);
        playerTrans_ = GCPlayerLogic::Instance()->Transform();

        Simian::Vector3 pos(Simian::Math::Random(minXPos_,maxXPos_),startHeight_,Simian::Math::Random(minZPos_,maxZPos_));
        rockTrans_->WorldPosition(pos);
        attribs_->Damage(damage_);
        randRotationAxis_.X(Simian::Math::Random(0.0f,1.0f));
        randRotationAxis_.Y(Simian::Math::Random(0.0f,1.0f));
        randRotationAxis_.Z(Simian::Math::Random(0.0f,1.0f));
        randAngularVel_ = Simian::Math::Random(180.0f,360.0f);

        dShadowScale_ *= -1.0f/(0.5f * startHeight_);
        shadowTrans_->Scale(Simian::Vector3::Zero);
        shadowTrans_->WorldPosition(Simian::Vector3(rockTrans_->Position().X(),0.05f,rockTrans_->Position().Z()));
    }

    void GCRockDebris::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("Radius",radiusSqr_);
        radiusSqr_ *= radiusSqr_;
        data.AddData("Speed", speed_);
        data.AddData("StartHeight",startHeight_);
    }

    void GCRockDebris::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("Radius",radiusSqr_,radiusSqr_);
        radiusSqr_ *= radiusSqr_;
        data.Data("Speed", speed_, speed_);
        data.Data("StartHeight",startHeight_,startHeight_);
    }

    void GCRockDebris::SetRandomSpawnPos( Simian::f32 minX, Simian::f32 minZ, Simian::f32 maxX, Simian::f32 maxZ )
    {
        minXPos_ = minX;
        minZPos_ = minZ;
        maxXPos_ = maxX;
        maxZPos_ = maxZ;
    }

    void GCRockDebris::Damage( Simian::u32 damage )
    {
        damage_ = damage;
    }
}
