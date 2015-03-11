/*************************************************************************/
/*!
\file		AISkillDash.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_AISKILLDASH_H_
#define DESCENSION_AISKILLDASH_H_

#include "Simian/Delegate.h"
#include "Simian/FiniteStateMachine.h"
#include "Simian/Vector3.h"

#include "AISkillBase.h"

namespace Simian
{
    class CTransform;
    class CModel;
    class CSoundEmitter;
}

namespace Descension
{
    class GCAttributes;
    class GCTileMap;
    class GCPhysicsController;

    class AISkillDash : public AISkillBase
    {
        enum DashStates
        {
            DS_START,
            DS_DASH,
            DS_END,
            DS_TOTAL
        };
    private:
        GCTileMap* map_;
        Simian::CModel* model_;
        Simian::CTransform* sourceTransform_;
        Simian::CTransform* targetTransform_;
        GCPhysicsController* targetPhysics_;
        Simian::CSoundEmitter* sounds_;
        bool dash_, attacked_;
        Simian::u32 animationFPS_, dashSoundNum_;
        Simian::f32 timer_, accelTimer_, deaccelTimer_;
        Simian::f32 startDelay_, endDelay_, totalTime_, dashTime_;
        Simian::f32 combinedRadius_;
        Simian::f32 dashStartDist_, dashStopDist_;
        Simian::f32 dashStartDistSqr_, dashStopDistSqr_;
        Simian::f32 speed_;
        Simian::f32 scaledTargetRadius_;
        Simian::f32 attackRadius_;
        Simian::f32 targetRadius_;
        Simian::s32 damage_;
        Simian::Vector3 vecToPlayer_;
        Simian::Vector3 destination_;
        Simian::FiniteStateMachine fsm_;

        static Simian::FactoryPlant<AIEffectBase, AISkillDash> factoryPlant_;
    protected:
        void onAwake_();
        void onInit_( GCAttributes* source );
        void onUnload_();
        void serialize_( Simian::FileObjectSubNode& node );
        void deserialize_( const Simian::FileObjectSubNode& node );
        void startOnEnter_( Simian::DelegateParameter& param );
        void startOnUpdate_( Simian::DelegateParameter& param );
        void dashOnEnter_( Simian::DelegateParameter& param );
        void dashOnUpdate_( Simian::DelegateParameter& param );
        void endOnEnter_( Simian::DelegateParameter& param );
        void endOnUpdate_( Simian::DelegateParameter& param );
        Simian::f32 vectorToDegreeFloat_(const Simian::Vector3& vec);

        bool checkPath_();
    public:
        AISkillDash();
        virtual ~AISkillDash();
        bool Update( Simian::f32 dt );
    };
}
#endif
