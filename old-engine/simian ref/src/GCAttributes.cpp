/*************************************************************************/
/*!
\file		GCAttributes.cpp
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/AnimationController.h"
#include "Simian/CModel.h"
#include "Simian/CModelColorOperation.h"
#include "Simian/Scene.h"
#include "Simian/Entity.h"
#include "Simian/Math.h"

#include "EntityTypes.h"
#include "ComponentTypes.h"

#include "GCAttributes.h"
#include "GCEditor.h"
#include "GCPlayerLogic.h"
#include "GCAIBase.h"
#include "GCAshFadeEffect.h"
#include "Simian/RenderQueue.h"
#include "Simian/Material.h"
#include "Simian/Shader.h"
#include "Simian/CParticleEmitterDisc.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCAttributes> GCAttributes::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_ATTRIBUTES);

    const Simian::f32 GHC_FADEIN_SPEED = 15.0f;
    const Simian::f32 GHC_FADEOUT_SPEED = 9.0f;
    const Simian::f32 GHC_FADEDEATH_SPEED = 0.5f;
    const Simian::f32 DEATH_FADE_TIMER_TOTAL = 1.0f;
    const Simian::f32 DEATH_END_TIMER_TOTAL = 0.0f;
    const Simian::f32 ASH_SPAWN_DELAY = 1.5f;
    const Simian::f32 ASH_Y_OFFSET = 0.1f;

    GCAttributes::GCAttributes()
        :   forceSeek_(false),
            ashSpawned_(false),
            ashSpawnTimer_(0.0f),
            ashSpawnOffset_(0.0f),
            ashSpawnRadius_(0.05f),
            victim_(0),
            maxHealth_(15),
            health_(15),
            damage_(3),
            damageRanged_(3),
            armour_(2),
            movementSpeed_(1.2f),
            movementSpeedMax_(1.2f),
            mana_(0.0f),
            maxMana_(0.0f),
			birthTimeMax_(0.8f),
            birthTimer_(birthTimeMax_),
			birthShadowTimeRatio_(0.3f),
            deathFadeTimer_(0.0f),
            birthState_(0),
            startFromBirth_(false),
            death_(false),
            experience_(0),
            experienceGoal_(800),
            minExperienceGoal_(800),
            maxExperienceGoal_(4000),
            experienceAwarded_(10),
            invulnerable_(false),
            deathFadeToBlack_(true),
            attackAngle_(0.0f),
            heat_(1),
            spellContMultiplier_(2.0f),
            getHitColorState_(GHC_NONE),
            modelColor_(0),
            ashFadeEffect_(0),
            direction_(Simian::Vector3::Zero),
            logicComponent_(0),
            enableHitColor_(true)
    {
        for (Simian::u32 i = 0; i < ST_TOTAL; ++i)
        {
            spellDamage_[i] = 0;
            spellCost_[i] = 0;
        }
    }

    //--------------------------------------------------------------------------
    // Setters and Getters

    bool GCAttributes::ForceSeek() const
    {
        return forceSeek_;
    }

    void GCAttributes::ForceSeek( bool val )
    {
        forceSeek_ = val;
    }

    GCAttributes* GCAttributes::Victim() const
    {
        return victim_;
    }

    void GCAttributes::Victim( GCAttributes* val )
    {
        victim_ = val;
    }

    Simian::u32 GCAttributes::CurrentState() const
    {
        return currentState_;
    }

    void GCAttributes::CurrentState( Simian::u32 val )
    {
        currentState_ = val;
    }

    Simian::s32 GCAttributes::MaxHealth() const
    {
        return maxHealth_;
    }

    void GCAttributes::MaxHealth( Simian::s32 val )
    {
        maxHealth_ = val;
    }

    Simian::s32 GCAttributes::MinMaxHealth() const
    {
        return minMaxHealth_;
    }

    void GCAttributes::MinMaxHealth( Simian::s32 val )
    {
        minMaxHealth_ = val;
    }

    Simian::s32 GCAttributes::MaxMaxHealth() const
    {
        return maxMaxHealth_;
    }

    void GCAttributes::MaxMaxHealth( Simian::s32 val )
    {
        maxMaxHealth_ = val;
    }

    Simian::s32 GCAttributes::Health() const
    {
        return health_;
    }

    void GCAttributes::Health(Simian::s32 health)
    {
        if (death_)
            return;

        health_ = health < 0 ? invulnerable_ ? 1 : 0 : 
                  health > maxHealth_ ? maxHealth_ : 
                  health;

        if (!health_)
            death_ = true;
    }

    Simian::f32 GCAttributes::MaxMana() const
    {
        return maxMana_;
    }

    void GCAttributes::MaxMana( Simian::f32 val )
    {
        maxMana_ = val;
    }

    Simian::f32 GCAttributes::MinMaxMana() const
    {
        return minMaxMana_;
    }

    void GCAttributes::MinMaxMana( Simian::f32 val )
    {
        minMaxMana_ = val;
    }

    Simian::f32 GCAttributes::MaxMaxMana() const
    {
        return maxMaxMana_;
    }

    void GCAttributes::MaxMaxMana( Simian::f32 val )
    {
        maxMaxMana_ = val;
    }

    Simian::f32 GCAttributes::Mana() const
    {
        return mana_;
    }

    void GCAttributes::Mana(Simian::f32 mana)
    {
        if (death_)
            return;

        mana_ = Simian::Math::Clamp(mana, 0.0f, maxMana_);
    }

    Simian::s32 GCAttributes::SpellFireDamage() const
    {
        return spellDamage_[ST_FIRE];
    }

    void GCAttributes::StartFromBirth(bool val)
    {
        startFromBirth_ = val;
    }

    bool GCAttributes::StartFromBirth()
    {
        return startFromBirth_;
    }

    bool GCAttributes::Death() const
    {
        return death_;
    }

    void GCAttributes::Death( bool val )
    {
        death_ = val;
    }

    bool GCAttributes::DeathFadeToBlack() const
    {
        return deathFadeToBlack_;
    }

    void GCAttributes::DeathFadeToBlack( bool val )
    {
        deathFadeToBlack_ = val;
    }

    void GCAttributes::BirthState(Simian::u32 val)
    {
        birthState_ = val;
    }

    Simian::u32 GCAttributes::BirthState()
    {
        return birthState_;
    }

	void GCAttributes::BirthTimeMax(Simian::f32 val)
    {
        birthTimeMax_ = val;
    }

    Simian::f32 GCAttributes::BirthTimeMax()
    {
        return birthTimeMax_;
    }

    void GCAttributes::BirthTimer(Simian::f32 val)
    {
        birthTimer_ = val;
    }

    Simian::f32 GCAttributes::BirthTimer()
    {
        return birthTimer_;
    }

	void GCAttributes::BirthShadowTimeRatio(Simian::f32 val)
    {
        birthShadowTimeRatio_ = val;
    }

    Simian::f32 GCAttributes::BirthShadowTimeRatio()
    {
        return birthShadowTimeRatio_;
    }

    Simian::u32 GCAttributes::ExperienceGoal() const
    {
        return experienceGoal_;
    }

    void GCAttributes::ExperienceGoal( Simian::u32 val )
    {
        experienceGoal_ = val;
    }

    void GCAttributes::MinExperienceGoal( Simian::u32 val )
    {
        minExperienceGoal_ = val;
    }

    Simian::u32 GCAttributes::MinExperienceGoal( void ) const
    {
        return minExperienceGoal_;
    }

    Simian::u32 GCAttributes::MaxExperienceGoal( void ) const
    {
        return maxExperienceGoal_;
    }

    void GCAttributes::MaxExperienceGoal( Simian::u32 val )
    {
        maxExperienceGoal_ = val;
    }

    Simian::u32 GCAttributes::Experience() const
    {
        return experience_;
    }

    void GCAttributes::Experience(Simian::u32 val)
    {
        if (death_)
            return;

        // check if the new exp will cause a new level up
        if (experience_ < experienceGoal_ && val >= experienceGoal_)
            levelUpDelegates_();

        experience_ = val;
        if (experience_ > experienceGoal_)
            experience_ = experienceGoal_;
    }

    Simian::u32 GCAttributes::ExperienceAwarded() const
    {
        return experienceAwarded_;
    }

    void GCAttributes::ExperienceAwarded(Simian::u32 val)
    {
        experienceAwarded_ = val;
    }

    bool GCAttributes::Invulnerable() const
    {
        return invulnerable_;
    }

    void GCAttributes::Invulnerable(bool invul)
    {
        invulnerable_ = invul;
    }

    Simian::u32 GCAttributes::Damage() const
    {
        return damage_;
    }

    void GCAttributes::Damage(Simian::u32 damage)
    {
        damage_ = damage;
    }

    Simian::u32 GCAttributes::DamageRanged() const
    {
        return damageRanged_;
    }

    void GCAttributes::DamageRanged(Simian::u32 damageRanged)
    {
        damageRanged_ = damageRanged;
    }

    Simian::u32 GCAttributes::Armour() const
    {
        return armour_;
    }

    void GCAttributes::Armour(Simian::u32 armour)
    {
        armour_ = armour;
    }

    Simian::f32 GCAttributes::MovementSpeed() const
    {
        return movementSpeed_;
    }

    void GCAttributes::MovementSpeed(Simian::f32 movementSpeed)
    {
        movementSpeed_ = movementSpeed;
    }

    Simian::f32 GCAttributes::MovementSpeedMax() const
    {
        return movementSpeedMax_;
    }

    void GCAttributes::MovementSpeedMax(Simian::f32 movementSpeedMax)
    {
        movementSpeedMax_ = movementSpeedMax;
    }

    Simian::f32 GCAttributes::AttackAngle() const
    {
        return attackAngle_;
    }

    void GCAttributes::AttackAngle( Simian::f32 val )
    {
        attackAngle_ = val;
    }

    Simian::f32 GCAttributes::AttackRange() const
    {
        return attackRange_;
    }

    void GCAttributes::AttackRange( Simian::f32 val )
    {
        attackRange_ = val;
    }

    Simian::u32 GCAttributes::Heat() const
    {
        return heat_;
    }

    void GCAttributes::Heat( Simian::u32 val )
    {
        heat_ = val;
    }

    Simian::Vector3 GCAttributes::Direction() const
    {
        return direction_;
    }

    void GCAttributes::Direction( Simian::Vector3 val )
    {
        val.Y(0.0f);
        if( val.LengthSquared() > 0 )
            direction_ = val.Normalized();
        else
            direction_ = val;
    }

    void GCAttributes::LogicComponent(Simian::EntityComponent* logicComp)
    {
        logicComponent_ = logicComp;
    }

    Simian::DelegateList& GCAttributes::DealDamageDelegates()
    {
        return dealDamageDelegates_;
    }

    Simian::DelegateList& GCAttributes::DeathDelegates()
    {
        return deathDelegates_;
    }

    Simian::DelegateList& GCAttributes::LevelUpDelegates()
    {
        return levelUpDelegates_;
    }

    Simian::s32 GCAttributes::SpellDamage(SpellTypes spellType)
    {
        return spellDamage_[spellType];
    }

    void GCAttributes::SpellDamage(SpellTypes spellType, Simian::s32 spellDamage)
    {
        Simian::s32& currSpellDamage = spellDamage_[spellType];
        currSpellDamage = spellDamage;
        if (currSpellDamage < 0)
            currSpellDamage = 0;
    }

    Simian::f32 GCAttributes::SpellCost(SpellTypes spellType)
    {
        return spellCost_[spellType];
    }

    void GCAttributes::SpellCost(SpellTypes spellType, Simian::f32 spellCost)
    {
        spellCost_[spellType] = spellCost;
    }

    Simian::f32 GCAttributes::SpellContMultiplier() const
    {
        return spellContMultiplier_;
    }

    void GCAttributes::SpellContMultiplier( Simian::f32 val )
    {
        spellContMultiplier_ = val;
    }

    bool GCAttributes::EnableHitColor() const
    {
        return enableHitColor_;
    }

    void GCAttributes::EnableHitColor( bool val )
    {
        enableHitColor_ = val;
    }

    Simian::CModelColorOperation* GCAttributes::ModelColor() const
    {
        return modelColor_;
    }

    //--------------------------------------------------------------------------

    void GCAttributes::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif

        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        dt;

        if (enableHitColor_)
            updateGetHitColor_(dt);
    }

    // Update color of model
    void GCAttributes::updateGetHitColor_(Simian::f32 dt)
    {
        // don't update if there is no model color component
        if (!modelColor_)
            return;

        if (getHitColorState_ == GHC_NONE)
            return;
        else if (getHitColorState_ == GHC_FADEIN)
        {
            modelColor_->Color(Simian::Color(modelColor_->Color().R()+(GHC_FADEIN_SPEED*dt),
                            modelColor_->Color().G()+(GHC_FADEIN_SPEED*dt), modelColor_->Color().B()+(GHC_FADEIN_SPEED*dt)));
            if (modelColor_->Color().R() >= 1.0f)
            {
                modelColor_->Color(Simian::Color(1.0f, 1.0f, 1.0f));
                getHitColorState_ = GHC_FADEOUT;
            }
        }
        else if (getHitColorState_ == GHC_FADEOUT)
        {
            modelColor_->Color(Simian::Color(modelColor_->Color().R()-(GHC_FADEOUT_SPEED*dt),
                modelColor_->Color().G()-(GHC_FADEOUT_SPEED*dt), modelColor_->Color().B()-(GHC_FADEOUT_SPEED*dt)));
            if (modelColor_->Color().R() <= 0.0f)
            {
                modelColor_->Color(Simian::Color(0.0f, 0.0f, 0.0f));
                if (health_ <= 0 && deathFadeToBlack_)
                {
                    getHitColorState_ = GHC_FADEDEATH;
                    modelColor_->Color(Simian::Color(1.0f, 1.0f, 1.0f));
                    modelColor_->Operation(modelColor_->CO_MODULATE);
                    modelColor_->Color(Simian::Color(1.0f, 1.0f, 1.0f));
                    ashSpawnTimer_ = 0.0f;
                }
                else
                    getHitColorState_ = GHC_NONE;
            }
        }
        else if (getHitColorState_ == GHC_FADEDEATH)
        {
            ashSpawnTimer_ += dt;

            if (ashSpawnTimer_ > ASH_SPAWN_DELAY && !ashSpawned_)
            {
                ashSpawned_ = true;

                // Create ash effect
                Simian::Entity* ashEnt = ParentScene()->CreateEntity(E_ASHFADEEFFECT);
                ashEnt->ComponentByType(GC_ASHFADEEFFECT, ashFadeEffect_);

                // Set position and enable particles
                Simian::CTransform* transformSelf;
                ComponentByType(Simian::C_TRANSFORM, transformSelf);

                Simian::CTransform* transformAsh;
                ashEnt->ComponentByType(Simian::C_TRANSFORM, transformAsh);
                transformAsh->Position(transformSelf->Position() + 
                    transformSelf->Direction() * ashSpawnOffset_ + Simian::Vector3(0.0f, ASH_Y_OFFSET, 0.0f));
                ParentEntity()->ParentScene()->AddEntity(ashEnt);
                Simian::CParticleEmitterDisc* discEmitter;
                ashEnt->ComponentByType(Simian::C_PARTICLEEMITTERDISC, discEmitter);
                discEmitter->Radius(ashSpawnRadius_);
                discEmitter->RandomRadius(ashSpawnRadius_ * 2.0f);
            }

            if (deathFadeTimer_ > DEATH_FADE_TIMER_TOTAL)
            {
                modelColor_->Color(Simian::Color(modelColor_->Color().R()-(GHC_FADEDEATH_SPEED*dt),
                    modelColor_->Color().G()-(GHC_FADEDEATH_SPEED*dt), modelColor_->Color().B()-(GHC_FADEDEATH_SPEED*dt)));
                if (modelColor_->Color().R() <= 0.0f)
                {
                    modelColor_->Color(Simian::Color(0.0f, 0.0f, 0.0f));
                    getHitColorState_ = GHC_DEATHEND;
                    deathFadeTimer_ = 0.0f;
                }
            }
            else
                deathFadeTimer_ += dt;
        }
        else if (getHitColorState_ == GHC_DEATHEND)
        {
            if (deathFadeTimer_ > DEATH_END_TIMER_TOTAL)
            {
                if (ashFadeEffect_)
                    ashFadeEffect_->Enabled(false);
                getHitColorState_ = GHC_TOTAL;
                deathFadeTimer_ = 0.0f;

                if (ParentEntity()->ParentEntity())
                    ParentEntity()->ParentEntity()->RemoveChild(ParentEntity());
                else
                    ParentScene()->RemoveEntity(ParentEntity());
            }
            else
                deathFadeTimer_ += dt;
        }
    }

    void GCAttributes::updateDisintegrate_( Simian::DelegateParameter& )
    {
        Simian::Material::Pass* pass = Simian::CModel::RenderQueue()->ActivePass();

        // update the time variable if the guy is dying and has the shader
        if (pass->Shader())
        {
            if (getHitColorState_ >= GHC_FADEDEATH)
                pass->Shader()->FragmentProgramConstant("Time").Set(1.0f - modelColor_->Color().R());
            else
                pass->Shader()->FragmentProgramConstant("Time").Set(0.0f);
        }
    }

    //--------------------------------------------------------------------------

    void GCAttributes::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCAttributes, &GCAttributes::update_>(this));
    }

    void GCAttributes::OnAwake()
    {
        ComponentByType(Simian::C_MODELCOLOROPERATION, modelColor_);
        modelColor_->Operation(modelColor_->CO_ADD);

        Simian::CModel* model;
        ComponentByType(Simian::C_MODEL, model);
        if (model)
        {
            // register callback for updating disintegration
            model->DrawCallback(
                Simian::Delegate::CreateDelegate<GCAttributes, &GCAttributes::updateDisintegrate_>(this));
        }
    }

    void GCAttributes::DealDamage( Simian::s32 damage, GCAttributes* source, bool flash )
    {
        // you can't do damage to a dead guy
        if (!Health())
            return;

        DealDamageParameter param(source, this, damage);
        dealDamageDelegates_(param);

        // only do damage if handled remains false
        if (param.Handled)
            return;

        Health(Health() - damage);
        if (flash)
            GetHitColorStart();

        if (!Health())
        {
            DeathParameter deathParam(source, this);
            deathDelegates_(deathParam);
        }
    }

    void GCAttributes::AddTimedEffect()
    {
    }

    void GCAttributes::RemoveTimedEffect()
    {
    }

    void GCAttributes::GetHitColorStart()
    {
        getHitColorState_ = GHC_FADEIN;
    }

    void GCAttributes::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("MaxHealth",maxHealth_);
        data.AddData("MinMaxHealth",minMaxHealth_);
        data.AddData("MaxMaxHealth",maxMaxHealth_);
        data.AddData("Health",health_);
        data.AddData("Damage",damage_);
        data.AddData("DamageRanged",damageRanged_);
        data.AddData("Armour",armour_);
        data.AddData("MovementSpeed",movementSpeed_);
        data.AddData("MaxMana",maxMana_);
        data.AddData("MinMaxMana",minMaxMana_);
        data.AddData("MaxMaxMana",maxMaxMana_);
        data.AddData("Mana",mana_);

        data.AddData("Invulnerable",invulnerable_);
        data.AddData("ExperienceGoal",experienceGoal_);
        data.AddData("MinExperienceGoal",minExperienceGoal_);
        data.AddData("MaxExperienceGoal",maxExperienceGoal_);
        data.AddData("ExperienceAwarded",experienceAwarded_);
        data.AddData("Heat", heat_);

        // Set the spell damages
        data.AddData("SpellDamagePhysical",spellDamage_[ST_PHYSICAL]);
        data.AddData("SpellDamageLightning",spellDamage_[ST_LIGHTNING]);
        data.AddData("SpellDamageFire",spellDamage_[ST_FIRE]);
        data.AddData("SpellDamageHealing",spellDamage_[ST_HEALING]);
        data.AddData("SpellDamageIce",spellDamage_[ST_ICE]);
        data.AddData("SpellDamageWind",spellDamage_[ST_WIND]);
        data.AddData("SpellDamageEarth",spellDamage_[ST_EARTH]);
        data.AddData("SpellDamageShadow",spellDamage_[ST_SHADOW]);

        // Set the spell costs
        data.AddData("SpellCostPhysical",spellCost_[ST_PHYSICAL]);
        data.AddData("SpellCostLightning",spellCost_[ST_LIGHTNING]);
        data.AddData("SpellCostFire",spellCost_[ST_FIRE]);
        data.AddData("SpellCostHealing",spellCost_[ST_HEALING]);
        data.AddData("SpellCostIce",spellCost_[ST_ICE]);
        data.AddData("SpellCostWind",spellCost_[ST_WIND]);
        data.AddData("SpellCostEarth",spellCost_[ST_EARTH]);
        data.AddData("SpellCostShadow",spellCost_[ST_SHADOW]);

        data.AddData("BirthState", birthState_);
		data.AddData("BirthTimeMax", birthTimeMax_);
        data.AddData("BirthTimer", birthTimer_);
		data.AddData("BirthShadowTimeRatio", birthShadowTimeRatio_);
        data.AddData("StartFromBirth", startFromBirth_);
        data.AddData("DeathFadeToBlack",deathFadeToBlack_);

        data.AddData("ForceSeek",forceSeek_);
        
        data.AddData("AshSpawnOffset", ashSpawnOffset_);
        data.AddData("AshSpawnRadius", ashSpawnRadius_);
        data.AddData("EnableHitColor", enableHitColor_);
    }

    void GCAttributes::Deserialize( const Simian::FileObjectSubNode& data )
    {
        // Get the data
        const Simian::FileDataSubNode* ml = data.Data("MaxHealth");
        ml ? maxHealth_ = static_cast<Simian::s32>(ml->AsF32()) : 0;
        health_ = maxHealth_;
        ml = data.Data("MinMaxHealth");
        ml ? minMaxHealth_ = static_cast<Simian::s32>(ml->AsF32()) : 0;
        ml = data.Data("MaxMaxHealth");
        ml ? maxMaxHealth_ = static_cast<Simian::s32>(ml->AsF32()) : 0;

        ml = data.Data("Damage");
        ml ? damage_ = static_cast<Simian::u32>(ml->AsF32()) : 0;
        ml = data.Data("DamageRanged");
        ml ? damageRanged_ = static_cast<Simian::u32>(ml->AsF32()) : 0;
        ml = data.Data("Armour");
        ml ? armour_ = static_cast<Simian::u32>(ml->AsF32()) : 0;
        ml = data.Data("MovementSpeed");
        ml ? movementSpeedMax_ = ml->AsF32() : 0;
        movementSpeed_ = movementSpeedMax_;
        ml = data.Data("MaxMana");
        ml ? maxMana_ = ml->AsF32() : 0;
        mana_ = maxMana_;
        ml = data.Data("MinMaxMana");
        ml ? minMaxMana_ = ml->AsF32() : 0;
        ml = data.Data("MaxMaxMana");
        ml ? maxMaxMana_ = ml->AsF32() : 0;

        data.Data("BirthState", birthState_, birthState_);
		data.Data("BirthTimeMax", birthTimeMax_, birthTimeMax_);
        data.Data("BirthTimer", birthTimer_, birthTimeMax_);
		data.Data("BirthShadowTimeRatio", birthShadowTimeRatio_, birthShadowTimeRatio_);
        data.Data("StartFromBirth", startFromBirth_, startFromBirth_);

        ml = data.Data("ExperienceGoal");
        ml ? experienceGoal_ = static_cast<Simian::u32>(ml->AsF32()) : 0;
        ml = data.Data("MaxExperienceGoal");
        ml ? maxExperienceGoal_ = static_cast<Simian::u32>(ml->AsF32()) : 0;
        ml = data.Data("MinExperienceGoal");
        ml ? minExperienceGoal_ = static_cast<Simian::u32>(ml->AsF32()) : 0;
        ml = data.Data("ExperienceAwarded");
        ml ? experienceAwarded_ = static_cast<Simian::u32>(ml->AsF32()) : 0;
        ml = data.Data("Invulnerable");
        ml ? invulnerable_ = ml->AsBool() : 0;
        ml = data.Data("DeathFadeToBlack");
        ml ? deathFadeToBlack_ = ml->AsBool() : 0;
        data.Data("Heat", heat_, heat_);
        ml = data.Data("SpellContMultiplier");
        ml ? spellContMultiplier_ = ml->AsF32() : 0;
        ml = data.Data("AttackAngle");
        ml ? attackAngle_ = ml->AsF32() : 0;
        ml = data.Data("AttackRange");
        ml ? attackRange_ = ml->AsF32() : 0;
        
        // Get the spell damages
        ml = data.Data("SpellDamagePhysical");
        ml ? spellDamage_[ST_PHYSICAL] = static_cast<Simian::s32>(ml->AsF32()) : 0;
        ml = data.Data("SpellDamageLightning");
        ml ? spellDamage_[ST_LIGHTNING] = static_cast<Simian::s32>(ml->AsF32()) : 0;
        ml = data.Data("SpellDamageFire");
        ml ? spellDamage_[ST_FIRE] = static_cast<Simian::s32>(ml->AsF32()) : 0;
        ml = data.Data("SpellDamageHealing");
        ml ? spellDamage_[ST_HEALING] = static_cast<Simian::s32>(ml->AsF32()) : 0;
        ml = data.Data("SpellDamageIce");
        ml ? spellDamage_[ST_ICE] = static_cast<Simian::s32>(ml->AsF32()) : 0;
        ml = data.Data("SpellDamageWind");
        ml ? spellDamage_[ST_WIND] = static_cast<Simian::s32>(ml->AsF32()) : 0;
        ml = data.Data("SpellDamageEarth");
        ml ? spellDamage_[ST_EARTH] = static_cast<Simian::s32>(ml->AsF32()) : 0;
        ml = data.Data("SpellDamageShadow");
        ml ? spellDamage_[ST_SHADOW] = static_cast<Simian::s32>(ml->AsF32()) : 0;

        // Get the spell costs
        ml = data.Data("SpellCostPhysical");
        ml ? spellCost_[ST_PHYSICAL] = ml->AsF32() : 0;
        ml = data.Data("SpellCostLightning");
        ml ? spellCost_[ST_LIGHTNING] = ml->AsF32() : 0;
        ml = data.Data("SpellCostFire");
        ml ? spellCost_[ST_FIRE] = ml->AsF32() : 0;
        ml = data.Data("SpellCostHealing");
        ml ? spellCost_[ST_HEALING] = ml->AsF32() : 0;
        ml = data.Data("SpellCostIce");
        ml ? spellCost_[ST_ICE] = ml->AsF32() : 0;
        ml = data.Data("SpellCostWind");
        ml ? spellCost_[ST_WIND] = ml->AsF32() : 0;
        ml = data.Data("SpellCostEarth");
        ml ? spellCost_[ST_EARTH] = ml->AsF32() : 0;
        ml = data.Data("SpellCostShadow");
        ml ? spellCost_[ST_SHADOW] = ml->AsF32() : 0;

        ml = data.Data("ForceSeek");
        ml ? forceSeek_ = ml->AsBool() : forceSeek_;

        data.Data("AshSpawnOffset", ashSpawnOffset_, ashSpawnOffset_);
        data.Data("AshSpawnRadius", ashSpawnRadius_, ashSpawnRadius_);
        data.Data("EnableHitColor", enableHitColor_, enableHitColor_);
    }
}

