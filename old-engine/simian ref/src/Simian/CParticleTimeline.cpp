#include "Simian/CParticleTimeline.h"
#include "Simian/EnginePhases.h"
#include "Simian/Scene.h"
#include "Simian/EngineComponents.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"
#include "Simian/Math.h"

namespace Simian
{
    FactoryPlant<EntityComponent, CParticleTimeline> CParticleTimeline::factoryPlant_(
        &GameFactory::Instance().ComponentFactory(), C_PARTICLETIMELINE);

    CParticleTimeline::CParticleTimeline()
        : repeat_(false),
          autoDestroy_(true),
          autoStart_(true),
          playing_(false),
          loaded_(false),
          timePassed_(0),
          lastTime_(0)
    {
    }

    //--------------------------------------------------------------------------

    void CParticleTimeline::update_( DelegateParameter& param )
    {
        // don't do anything if not playing
        if (!playing_)
            return;

        EntityUpdateParameter* p;
        param.As(p);

        timePassed_ += p->Dt;
        for (Simian::u32 i = 0; i < effects_.size(); ++i)
        {
            if (!effects_[i].ParticleSystem)
                continue;

            // see if its time to start or stop a particle system (stopping with 
            // the higher priority)
            if (timePassed_ > effects_[i].EndTime)
                effects_[i].ParticleSystem->ParticleSystem()->Enabled(false);
            else if (timePassed_ > effects_[i].StartTime)
                effects_[i].ParticleSystem->ParticleSystem()->Enabled(true);
        }

        if (timePassed_ > lastTime_)
        {
            Stop();

            if (autoDestroy_)
            {
                // remove from parent or scene
                if (ParentEntity()->ParentEntity())
                    ParentEntity()->ParentEntity()->RemoveChild(ParentEntity());
                else
                    ParentScene()->RemoveEntity(ParentEntity());
            }
            else if (repeat_)
                Play();
        }
    }

    void CParticleTimeline::load_()
    {
        lastTime_ = 0.0f;

        // find all related particles
        for (Simian::u32 i = 0; i < effects_.size(); ++i)
        {
            Entity* entity = ParentEntity()->ChildByName(effects_[i].EntityName);
            if (entity)
            {
                entity->ComponentByType(C_PARTICLESYSTEM, effects_[i].ParticleSystem);
                if (effects_[i].ParticleSystem)
                {
                    ParticleSystem* particleSystem = effects_[i].ParticleSystem->ParticleSystem();
                    Simian::f32 lastTime = effects_[i].EndTime + 
                        particleSystem->LifeTime() + 
                        particleSystem->RandomLifeTime();
                    lastTime_ = Simian::Math::Max(lastTime_, lastTime);
                }
            }
        }

        // automatically play particles
        if (autoStart_) 
            Play();
    }

    void CParticleTimeline::stopAll_()
    {
        for (Simian::u32 i = 0; i < effects_.size(); ++i)
        {
            if (effects_[i].ParticleSystem)
                effects_[i].ParticleSystem->ParticleSystem()->Enabled(false);
        }
    }

    //--------------------------------------------------------------------------

    void CParticleTimeline::OnSharedLoad()
    {
        RegisterCallback(P_UPDATE, Delegate::CreateDelegate<CParticleTimeline, 
                         &CParticleTimeline::update_>(this));
    }

    void CParticleTimeline::OnAwake()
    {
        loaded_ = true;
        load_();
    }

    void CParticleTimeline::Serialize( FileObjectSubNode& data )
    {
        data.AddData("Repeat", repeat_);
        data.AddData("AutoDestroy", autoDestroy_);
        data.AddData("AutoStart", autoStart_);

        FileArraySubNode* effects = data.AddArray("Effects");
        for (Simian::u32 i = 0; i < effects_.size(); ++i)
        {
            TimedParticleEffect_& timedEffect = effects_[i];
            FileObjectSubNode* effect = effects->AddObject("Effect");

            effect->AddData("EntityName", timedEffect.EntityName);
            effect->AddData("StartTime", timedEffect.StartTime);
            effect->AddData("EndTime", timedEffect.EndTime);
        }
    }

    void CParticleTimeline::Deserialize( const FileObjectSubNode& data )
    {
        data.Data("Repeat", repeat_, repeat_);
        data.Data("AutoDestroy", autoDestroy_, autoDestroy_);
        data.Data("AutoStart", autoStart_, autoStart_);

        const FileArraySubNode* effects = data.Array("Effects");
        if (effects)
        {
            effects_.clear();
            for (Simian::u32 i = 0; i < effects->Size(); ++i)
            {
                const FileObjectSubNode* effect = effects->Object(i);
                
                TimedParticleEffect_ timedEffect;
                timedEffect.ParticleSystem = 0;
                effect->Data("EntityName", timedEffect.EntityName, "");
                effect->Data("StartTime", timedEffect.StartTime);
                effect->Data("EndTime", timedEffect.EndTime);

                effects_.push_back(timedEffect);
            }
        }

        if (loaded_)
        {
            Stop();
            load_();
        }
    }

    void CParticleTimeline::Play()
    {
        stopAll_();

        playing_ = true;
        timePassed_ = 0;
    }

    void CParticleTimeline::Stop()
    {
        stopAll_();

        playing_ = false;
        timePassed_ = 0;
    }
}
