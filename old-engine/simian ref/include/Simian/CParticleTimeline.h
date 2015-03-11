#ifndef SIMIAN_CPARTICLETIMELINE_H_
#define SIMIAN_CPARTICLETIMELINE_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"

#include <string>

namespace Simian
{
    class CParticleSystem;

    class SIMIAN_EXPORT CParticleTimeline: public EntityComponent
    {
    private:
        struct TimedParticleEffect_
        {
            CParticleSystem*    ParticleSystem;
            Simian::f32         StartTime;
            Simian::f32         EndTime;
            std::string         EntityName;
        };
    private:
        std::vector<TimedParticleEffect_> effects_;
        bool repeat_;
        bool autoDestroy_;
        bool autoStart_;
        bool playing_;
        bool loaded_;
        Simian::f32 timePassed_;
        Simian::f32 lastTime_;

        static FactoryPlant<EntityComponent, CParticleTimeline> factoryPlant_;
    private:
        void update_(DelegateParameter&);

        void load_();
        void stopAll_();
    public:
        CParticleTimeline();

        void OnSharedLoad();
        void OnAwake();

        void Serialize( FileObjectSubNode& data );
        void Deserialize( const FileObjectSubNode& data );

        void Play();
        void Stop();
    };
}

#endif
