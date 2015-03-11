/******************************************************************************/
/*!
\file		GCAICrystalBossFist.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/12/07
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "GCAICrystalBossFist.h"
#include "ComponentTypes.h"
#include "Simian\EnginePhases.h"
#include "GCAIBase.h"
#include "GCEditor.h"
#include "Simian\DebugRenderer.h"
#include "Simian\CTransform.h"
#include "Simian\LogManager.h"
#include "GCPlayerLogic.h"
#include "GCPhysicsController.h"
#include "Simian\EntityComponent.h"
#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"
#include "Simian\Scene.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCAICrystalBossFist> GCAICrystalBossFist::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_AICRYSTALGUARDIANFIST);

    GCAICrystalBossFist::GCAICrystalBossFist()
        :   GCAIBase(CGF_UPDATE,CGF_TOTAL),
            attackOn_(false),
            bossAttribs_(0),
            damage_(0),
            radius_(0)
    {
    }

    GCAICrystalBossFist::~GCAICrystalBossFist()
    {
    }

    void GCAICrystalBossFist::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCAICrystalBossFist, &GCAICrystalBossFist::update_>(this));
    }

    void GCAICrystalBossFist::OnInit()
    {
#ifdef _DESCENDED
    if (GCEditor::EditorMode())
        return;
#endif
        GCAIBase::OnInit();
        ParentScene()->EntityByName("EnemyCrystalGuardian")->ComponentByType(GC_ATTRIBUTES,bossAttribs_);
        ParentEntity()->ComponentByType(Simian::C_TRANSFORM,transform_);
        ParentEntity()->ComponentByType(GC_PHYSICSCONTROLLER, physics_);
        playerTrans_ = &GCPlayerLogic::Instance()->Transform();
        attributes_->DealDamageDelegates() += 
            Simian::Delegate::CreateDelegate<GCAICrystalBossFist, &GCAICrystalBossFist::onTakeDamage_>(this);
    }

    void GCAICrystalBossFist::Serialize( Simian::FileObjectSubNode& data )
    {
        GCAIBase::Serialize(data);
        data.AddData("Radius", radius_);
    }

    void GCAICrystalBossFist::Deserialize( const Simian::FileObjectSubNode& data )
    {
        GCAIBase::Deserialize(data);
        data.Data("Radius", radius_,radius_);
    }

    //--------------------------------------------------------------------------

    void GCAICrystalBossFist::AttackOn( bool val )
    {
        attackOn_ = val;
    }

    void GCAICrystalBossFist::Damage( Simian::s32 damage )
    {
        damage_ = damage;
    }

    void GCAICrystalBossFist::Radius( Simian::f32 val )
    {
        radius_ = val;
    }

    //--------------------------------------------------------------------------

    void GCAICrystalBossFist::update_( Simian::DelegateParameter&  )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        Simian::Vector3 pos(transform_->WorldPosition());

        if(attackOn_)
        {
            sqrDist_ = (pos - playerTrans_->WorldPosition()).LengthSquared();

            if(IsPlayerInRange())
            {
                dealDamage_();
                attackOn_ = false;
            }
        }
        else
            pos.Y(0.1f);
        physics_->Position(pos);
        DebugRendererDrawCircle(pos,radius_,Simian::Color(0,0,1,1));
    }

    void GCAICrystalBossFist::overRideStateUpdate_( Simian::f32  )
    {
    }

    void GCAICrystalBossFist::onTakeDamage_( Simian::DelegateParameter& param )
    {
        GCAttributes::DealDamageParameter& p = param.As<GCAttributes::DealDamageParameter>();
        CombatCommandSystem::Instance().AddCommand(attributes_, bossAttribs_,
            CombatCommandDealDamage::Create(p.Damage));
    }

    void GCAICrystalBossFist::dealDamage_()
    {
        if(damage_ == 0)
            return;
        CombatCommandSystem::Instance().AddCommand(attributes_, GCPlayerLogic::Instance()->Attributes(),
            CombatCommandDealDamage::Create(damage_));
    }

    //--------------------------------------------------------------------------

    bool GCAICrystalBossFist::IsPlayerInRange()
    {
        return sqrDist_ < (radius_*radius_);
    }

    Simian::Vector3 GCAICrystalBossFist::Position() const
    {
        return transform_->WorldPosition();
    }
}
