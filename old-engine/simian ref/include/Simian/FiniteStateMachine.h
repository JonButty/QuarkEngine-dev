/*****************************************************************************/
/*!
\file		FiniteStateMachine.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_FINITESTATEMACHINE_H_
#define SIMIAN_FINITESTATEMACHINE_H_

#include "SimianPlatform.h"
#include "Delegate.h"
#include <vector>

namespace Simian
{
    struct SIMIAN_EXPORT FiniteStateUpdateParameter: public DelegateParameter
    {
        float Dt;
    };

    struct SIMIAN_EXPORT FiniteState
    {
        Delegate OnEnter;
        Delegate OnUpdate;
        Delegate OnExit;
    };

    class SIMIAN_EXPORT FiniteStateMachine
    {
    private:
        std::vector<FiniteState> states_;
        s32 currentState_;
        s32 pendingState_;
    public:
        void CurrentState(s32 state);
        void CurrentStateDirect(s32 state);
        s32 CurrentState() const;
    public:
        FiniteState& operator[](s32 index);
        const FiniteState& operator[](s32 index) const;
    public:
        FiniteStateMachine(s32 initialState, s32 totalStates);
        virtual ~FiniteStateMachine();

        void ChangeState(s32 state);
        void Update(f32 dt);
    };
}

#endif
