/******************************************************************************/
/*!
\file		AIEffectFleeLocation.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_AIEFFECTFLEELOCATION_H_
#define DESCENSION_AIEFFECTFLEELOCATION_H_

#include "Simian/SimianTypes.h"
#include "Simian/Delegate.h"
#include "Simian/Vector3.h"
#include "Simian/FiniteStateMachine.h"

#include "AIEffectBase.h"

namespace Simian
{
	class CTransform;
    class CModel;
}

namespace Descension
{
    class GCPhysicsController;

	class AIEffectFleeLocation : public AIEffectBase
	{
    private:
        enum FleeState
        {
            FS_PREFLEE,
            FS_FLEE,
            FS_TOTAL
        };
	private:
        bool flee_;
        Simian::CTransform* playerTrans_;
        Simian::CTransform* tarTrans_;
        GCPhysicsController * tarPhys_;
        Simian::CModel* model_;
        Simian::s32 animationSpeed_;
        Simian::f32 timer_;
        Simian::f32 fleeTime_;
        Simian::f32 currDistSqr_;
        Simian::f32 fleeSpeed_;
        Simian::f32 radius_;
        Simian::f32 fleeRadius_, fleeRadiusSqr_;
        Simian::f32 steerTimer_;
        Simian::f32 maxSteerTime_;
        Simian::Vector3 startDir_;
        Simian::Vector3 finalDir_;
        Simian::Vector3 nextTile_;
        Simian::Vector3 fleeLocation_;
		Simian::FiniteStateMachine fsm_;

		static Simian::FactoryPlant<AIEffectBase, AIEffectFleeLocation> factoryPlant_;
    private:
        void onAwake_();
		void onInit_( GCAttributes* source );
		void onUnload_();
        void serialize_( Simian::FileObjectSubNode& node);
        void deserialize_( const Simian::FileObjectSubNode& node);
        void preFleeOnUpdate_( Simian::DelegateParameter& param);
        void fleeOnEnter_( Simian::DelegateParameter& param);
        void fleeOnUpdate_( Simian::DelegateParameter& param);

        void updateNextTile_();
        void steer_( Simian::f32 dt );
        void determineFleeLocation_();
        Simian::f32 vectorToDegreeFloat_(const Simian::Vector3& vec);
        
	public:
		AIEffectFleeLocation();
        ~AIEffectFleeLocation();
		bool Update( Simian::f32 dt);
	};
}

#endif

