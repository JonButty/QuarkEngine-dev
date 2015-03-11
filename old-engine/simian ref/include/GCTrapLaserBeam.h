/*************************************************************************/
/*!
\file		GCTrapLaserBeam.h
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCTRAPLASERBEAM_H_
#define DESCENSION_GCTRAPLASERBEAM_H_

#include "Simian/EntityComponent.h"

namespace Simian
{
    class CTransform;
    class CParticleSystem;
    class CSoundEmitter;
}

namespace Descension
{
    class GCAttributes;

    class GCTrapLaserBeam: public Simian::EntityComponent
    {
    private:
        Simian::CTransform *transform_, *lightningBeamTransform_;
        Simian::CTransform *playerTransform_;
        Simian::Entity *lightningBeam_;
        GCAttributes *attributes_, *playerAttributes_;
        Simian::CParticleSystem *fireParticles1_, *fireParticles2_, *laserParticles_;
        Simian::CSoundEmitter* sounds_;

        Simian::f32 startAngle_, finalAngle_, spanAngle_;
        Simian::f32 midX_, midZ_, offsetX_, offsetZ_, offsetRotation_;
        Simian::f32 beamTimer_, rotateSpanTime_;
        Simian::f32 hitTimer_, hitDelay_;
        Simian::f32 turnPauseTimer_, turnPauseTimeMax_;
        Simian::f32 laserSoundTimer_;
        Simian::s32 laserDamage_;
        bool enabled_;

        // factory plant
        static Simian::FactoryPlant<Simian::EntityComponent, GCTrapLaserBeam> factoryPlant_;
    public:
        Simian::CTransform& Transform();
        bool Enabled() const;
        void Enabled(bool);
    private:
        void update_(Simian::DelegateParameter& param);
        Simian::f32 vectorToDegreeFloat_(const Simian::Vector3& vec);
    public:
        GCTrapLaserBeam();

        void OnSharedLoad();
        void OnAwake();
        void OnInit();

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
