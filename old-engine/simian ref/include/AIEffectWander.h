/******************************************************************************/
/*!
\file		AIEffectWander.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_AIEFFECTWANDER_H_
#define DESCENSION_AIEFFECTWANDER_H_

#include "Simian/SimianTypes.h"
#include "Simian/Delegate.h"
#include "Simian/Vector3.h"
#include "Simian/FiniteStateMachine.h"

#include "AIEffectBase.h"
#include <vector>

namespace Simian
{
    class CTransform;
    class CModel;
}

namespace Descension
{
    class GCAttributes;
    class GCPhysicsController;
    class AIEffectWander : public AIEffectBase
    {
    private:
        enum WanderStates
        {
            WS_BIRTH,
            WS_IDLE,
            WS_WANDER,
            WS_TOTAL
        };

        enum Direction
        {
            DIR_NORTH,
            DIR_EAST,
            DIR_SOUTH,
            DIR_WEST,
            DIR_TOTAL
        };
    private:
        Simian::CTransform* transform_;
        GCPhysicsController* physics_;
        Simian::CModel* model_;
        bool wander_;
        Simian::u32 stateTrigger_;
        Simian::u32 prevDir_, currentDir_;
        Simian::u32 minX_, minY_, maxX_, maxY_;
        Simian::f32 wanderRadiusMin_, wanderIntervalMin_, idleIntervalMin_, dirChangeIntervalMin_;
        Simian::f32 wanderRadiusMax_, wanderIntervalMax_, idleIntervalMax_, dirChangeIntervalMax_;
        Simian::f32 steerTimer_, wanderTimer_, idleTimer_ , dirChangeTimer_;
        Simian::Vector2 center_;
        Simian::Vector3 nextTile_;
        Simian::Vector3 startDir_, finalDir_;
        std::vector<Simian::u32> dirSequence_;
        Simian::FiniteStateMachine fsm_;

        static Simian::FactoryPlant<AIEffectBase, AIEffectWander> factoryPlant_;
    private:
        void onAwake_();
        void onInit_( GCAttributes* source );
        void onUnload_();
        void loadVictim_();
        bool reachedTile_(Simian::f32 dt);
        bool updateNextTile_( );
        void steer_( Simian::f32 dt );
        void loadDirSequence_();
        void determineDir_(Simian::Vector2& subTilePos);
        Simian::f32 vectorToDegreeFloat_(const Simian::Vector3& vec);
        bool isWithinBounds_(Simian::u32 x, Simian::u32 y);
        bool checkSpan_(Simian::u32 constAxis, Simian::u32 min, Simian::u32 max, Simian::u32 dir);
        void serialize_( Simian::FileObjectSubNode& node);
        void deserialize_( const Simian::FileObjectSubNode& node);
        void birthOnEnter_( Simian::DelegateParameter& param );
        void birthOnUpdate_( Simian::DelegateParameter& param );
        void idleOnEnter_( Simian::DelegateParameter& param );
        void idleOnUpdate_( Simian::DelegateParameter& param );
        void wanderOnEnter_( Simian::DelegateParameter& param );
        void wanderOnUpdate_( Simian::DelegateParameter& param );
    public:
        AIEffectWander();
        ~AIEffectWander();
        bool Update( Simian::f32 dt);
    };
}

#endif
