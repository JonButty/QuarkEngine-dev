/*************************************************************************/
/*!
\file		GCAttributes.h
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCATTRIBUTES_H_
#define DESCENSION_GCATTRIBUTES_H_

#include "Simian/EntityComponent.h"
#include "Simian/Vector3.h"
#include "Simian/Delegate.h"

#include <vector>

namespace Simian
{
    class Entity;
    class CModelColorOperation;
}

namespace Descension
{
    class AIEffectBase;
    class GCAshFadeEffect;

    class GCAttributes: public Simian::EntityComponent
    {
    public:
        enum SpellTypes
        {
            ST_PHYSICAL,
            ST_LIGHTNING,
            ST_FIRE,
            ST_HEALING,
            ST_ICE,
            ST_WIND,
            ST_EARTH,
            ST_SHADOW,
            ST_TOTAL
        };

        struct DealDamageParameter: public Simian::DelegateParameter
        {
            GCAttributes* Source;
            GCAttributes* Target;
            Simian::s32 Damage;
            bool Handled;

            DealDamageParameter(GCAttributes* source, GCAttributes* target, Simian::s32 damage)
                : Source(source), Target(target), Damage(damage), Handled(false) {}
        };

        struct DeathParameter: public Simian::DelegateParameter
        {
            GCAttributes* Source;
            GCAttributes* Target;
            DeathParameter(GCAttributes* source, GCAttributes* target): Source(source), Target(target) {}
        };
    private:
        enum GetHitColorStates
        {
            GHC_NONE,
            GHC_FADEIN,
            GHC_FADEOUT,
            GHC_FADEDEATH,
            GHC_DEATHEND,
            GHC_TOTAL
        };
    private:
        GCAttributes* victim_;
        bool forceSeek_;
        bool ashSpawned_;
        Simian::f32 ashSpawnTimer_;
        Simian::f32 ashSpawnOffset_;
        Simian::f32 ashSpawnRadius_;
        Simian::u32 currentState_;
        Simian::s32 maxHealth_, minMaxHealth_, maxMaxHealth_;
        Simian::s32 health_;
        Simian::u32 damage_;
        Simian::u32 damageRanged_;
        Simian::u32 armour_;
        Simian::f32 movementSpeed_;
        Simian::f32 movementSpeedMax_;
        Simian::f32 mana_;
        Simian::f32 maxMana_, minMaxMana_, maxMaxMana_;
        Simian::u32 experience_;
        Simian::u32 experienceGoal_, minExperienceGoal_, maxExperienceGoal_;
        Simian::u32 experienceAwarded_;
        Simian::f32 attackAngle_;
        Simian::f32 attackRange_;
        Simian::u32 heat_;
        bool invulnerable_, startFromBirth_, deathFadeToBlack_, death_;
        Simian::u32 birthState_;
        Simian::f32 birthTimeMax_, birthTimer_, birthShadowTimeRatio_, deathFadeTimer_;
        GetHitColorStates getHitColorState_;
        Simian::CModelColorOperation* modelColor_;
        GCAshFadeEffect* ashFadeEffect_;
        Simian::Vector3 direction_;
        std::vector<AIEffectBase*> timedEffectList_;
        Simian::EntityComponent* logicComponent_;
        bool enableHitColor_;

        // variables for spells
        Simian::s32 spellDamage_[ST_TOTAL];
        Simian::f32 spellCost_[ST_TOTAL];
        Simian::f32 spellContMultiplier_;

        // registerable events
        Simian::DelegateList dealDamageDelegates_;
        Simian::DelegateList deathDelegates_;
        Simian::DelegateList levelUpDelegates_;

        // factory plant
        static Simian::FactoryPlant<Simian::EntityComponent, GCAttributes> factoryPlant_;
    public:
        // Setters and Getters
        bool ForceSeek() const;
        void ForceSeek(bool val);

        GCAttributes* Victim() const;
        void Victim(GCAttributes* val);

        Simian::u32 CurrentState() const;
        void CurrentState(Simian::u32 val);

        Simian::s32 MaxHealth() const;
        void MaxHealth(Simian::s32 val);

        Simian::s32 MinMaxHealth() const;
        void MinMaxHealth(Simian::s32 val);

        Simian::s32 MaxMaxHealth() const;
        void MaxMaxHealth(Simian::s32 val);

        Simian::s32 Health() const;
        void Health(Simian::s32);

        Simian::u32 Damage() const;
        void Damage(Simian::u32);

        Simian::u32 DamageRanged() const;
        void DamageRanged(Simian::u32);

        Simian::u32 Armour() const;
        void Armour(Simian::u32);

        Simian::f32 MovementSpeed() const;
        void MovementSpeed(Simian::f32);

        Simian::f32 MovementSpeedMax() const;
        void MovementSpeedMax(Simian::f32);

        Simian::f32 MaxMana() const;
        void MaxMana(Simian::f32 val);

        Simian::f32 MinMaxMana() const;
        void MinMaxMana(Simian::f32 val);

        Simian::f32 MaxMaxMana() const;
        void MaxMaxMana(Simian::f32 val);

        Simian::f32 Mana() const;
        void Mana(Simian::f32);

        Simian::s32 SpellFireDamage() const;

        void StartFromBirth(bool);
        bool StartFromBirth();

        bool Death() const;
        void Death(bool val);

        bool DeathFadeToBlack() const;
        void DeathFadeToBlack(bool val);

        void BirthState(Simian::u32);
        Simian::u32 BirthState();

		void BirthTimeMax(Simian::f32);
        Simian::f32 BirthTimeMax();

        void BirthTimer(Simian::f32);
        Simian::f32 BirthTimer();

		void BirthShadowTimeRatio(Simian::f32);
        Simian::f32 BirthShadowTimeRatio();

        Simian::u32 Experience() const;
        void Experience(Simian::u32 val);

        Simian::u32 ExperienceGoal() const;
        void ExperienceGoal(Simian::u32);

        Simian::u32 MinExperienceGoal() const;
        void MinExperienceGoal(Simian::u32);

        Simian::u32 MaxExperienceGoal() const;
        void MaxExperienceGoal(Simian::u32);

        Simian::u32 ExperienceAwarded() const;
        void ExperienceAwarded(Simian::u32 val);

        bool Invulnerable() const;
        void Invulnerable(bool invul);

        Simian::f32 AttackAngle() const;
        void AttackAngle(Simian::f32 val);

        Simian::f32 AttackRange() const;
        void AttackRange(Simian::f32 val);

        Simian::u32 Heat() const;
        void Heat(Simian::u32 val);

        Simian::Vector3 Direction() const;
        void Direction( Simian::Vector3 val);

        template <class T>
		T* LogicComponent() const
		{
			return reinterpret_cast<T*>(logicComponent_);
		}
		void LogicComponent(Simian::EntityComponent* logicComponent);

        Simian::DelegateList& DealDamageDelegates();
        Simian::DelegateList& DeathDelegates();
        Simian::DelegateList& LevelUpDelegates();

        // Setter and Getter for spell damage
        Simian::s32 SpellDamage(SpellTypes spellType);
        void SpellDamage(SpellTypes spellType, Simian::s32 spellDamage);

        // Setter and Getter for spell cost
        Simian::f32 SpellCost(SpellTypes spellType);
        void SpellCost(SpellTypes spellType, Simian::f32 spellCost);

        Simian::f32 SpellContMultiplier() const;
        void SpellContMultiplier(Simian::f32);

        bool EnableHitColor() const;
        void EnableHitColor(bool val);

        Simian::CModelColorOperation* ModelColor() const;

    private:
        void update_(Simian::DelegateParameter& param);
        void updateGetHitColor_(Simian::f32 dt);
        void updateDisintegrate_(Simian::DelegateParameter& param);
    public:
        GCAttributes();

        void OnSharedLoad();
        void OnAwake();

        void DealDamage(Simian::s32 damage, GCAttributes* source = 0, bool flash = true);

        void GetHitColorStart();
        void AddTimedEffect();
        void RemoveTimedEffect();

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif

