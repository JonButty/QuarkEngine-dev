/*****************************************************************************/
/*!
\file		FiniteStateMachine.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/FiniteStateMachine.h"
#include "Simian/Debug.h"

namespace Simian
{
    FiniteStateMachine::FiniteStateMachine(s32 initialState, s32 totalStates)
        : pendingState_(initialState),
          currentState_(-1),
          states_(totalStates)
    {
    }

    FiniteStateMachine::~FiniteStateMachine()
    {
        states_.clear();
    }

    //--------------------------------------------------------------------------

    void FiniteStateMachine::CurrentState(s32 state)
    {
        SAssert(state >= 0 && state < (s32)states_.size(), "Setting current to invalid state.");
        if (currentState_ >= 0)
            states_[currentState_].OnExit();
        currentState_ = state;
        states_[currentState_].OnEnter();
    }

    void FiniteStateMachine::CurrentStateDirect(s32 state)
    {
        currentState_ = state;
    }

    s32 FiniteStateMachine::CurrentState() const
    {
        return currentState_;
    }

    //--------------------------------------------------------------------------

    FiniteState& FiniteStateMachine::operator[](s32 index)
    {
        return states_[index];
    }

    const FiniteState& FiniteStateMachine::operator[](s32 index) const
    {
        return states_[index];
    }

    //--------------------------------------------------------------------------

    void FiniteStateMachine::ChangeState(s32 state)
    {
        pendingState_ = state;
    }

    void FiniteStateMachine::Update(f32 dt)
    {
        FiniteStateUpdateParameter param;
        param.Dt = dt;

        if (currentState_ >= 0)
            states_[currentState_].OnUpdate(param);

        if (pendingState_ != -1)
        {
            CurrentState(pendingState_);
            pendingState_ = -1;
        }
    }
}
