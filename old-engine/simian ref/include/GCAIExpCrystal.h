/******************************************************************************/
/*!
\file		GCAIExpCrystal.h
\author 	Ngai Wen Sheng Jason, jason.ngai, 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/11/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GCAIEXPCRYSTAL_H_
#define DESCENSION_GCAIEXPCRYSTAL_H_

#include "Simian/Delegate.h"
#include "Simian/EntityComponent.h"
#include "Simian/Vector3.h"
#include "Simian/Vector2.h"

#include "GCAIBase.h"

namespace Simian
{
    class CTransform;
    class CModel;
    class CSoundEmitter;
}

namespace Descension
{
    class GCPhysicsController;

    enum EnemyStates
    {
        ES_IDLE,
        ES_DEATH,
        ES_TOTAL
    };

    class GCAIExpCrystal : public GCAIBase
    {
	private:
        Simian::u32 animFrameRate_, gemID_;
        Simian::CSoundEmitter* sounds_;
        Simian::s32 totalDestroySounds_, prevHP_;
        Simian::u32 numAnimation_, expReward_, healthPhase_;
        std::string animationName_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCAIExpCrystal> factoryPlant_;
        Simian::Vector2 oldMin_;
        Simian::Vector2 oldMax_;
    private:
        void idleOnEnter_( Simian::DelegateParameter& param );
        void idleOnUpdate_( Simian::DelegateParameter& param );
        void deathOnEnter_( Simian::DelegateParameter& param );
        void deathOnUpdate_( Simian::DelegateParameter& param );
        void overRideStateUpdate_( Simian::f32 dt );
        
	public:
		GCAIExpCrystal();
        ~GCAIExpCrystal();
        void OnSharedLoad();
        void Deserialize( const Simian::FileObjectSubNode& data );
        void Serialize( Simian::FileObjectSubNode& data );
        void OnAwake();
        void OnInit();
        void OnDeinit();
	};
}

#endif
