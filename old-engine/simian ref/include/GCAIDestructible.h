/******************************************************************************/
/*!
\file		GCAIDestructible.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GCAIGeneric_H_
#define DESCENSION_GCAIGeneric_H_

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

    class GCAIDestructible : public GCAIBase
    {
	private:
        Simian::u32 animFrameRate_;
        Simian::CTransform* transform_;
        Simian::CTransform* playerTransform_;
        Simian::CSoundEmitter* sounds_;
        Simian::s32 totalDestroySounds_;
        Simian::s32 reactDmg_; //-- negative one to signify no sensitivity
        Simian::s32 reactDmg2_; //-- so that fire barrels can destroy each other
        Simian::s32 explosionDamage_;
        Simian::f32 explosionRadius_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCAIDestructible> factoryPlant_;
        Simian::Vector2 oldMin_;
        Simian::Vector2 oldMax_;
    private:
        void idleOnEnter_( Simian::DelegateParameter& param );
        void idleOnUpdate_( Simian::DelegateParameter& param );
        void deathOnEnter_( Simian::DelegateParameter& param );
        void deathOnUpdate_( Simian::DelegateParameter& param );
        void overRideStateUpdate_( Simian::f32 dt );
        
	public:
		GCAIDestructible();
        ~GCAIDestructible();
        void OnSharedLoad();
        void Deserialize( const Simian::FileObjectSubNode& data );
        void Serialize( Simian::FileObjectSubNode& data );
        void OnAwake();
        void OnInit();
        void OnDeinit();
	};
}

#endif
