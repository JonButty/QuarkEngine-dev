/*****************************************************************************/
/*!
\file		StateMachine.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/StateMachine.h"

namespace Simian
{
    StateMachine::StateMachine(State* initialState)
    : currentState_(0), 
      pendingState_(initialState)
    {
    }

    StateMachine::~StateMachine()
    {
        if (currentState_)
            currentState_->OnExit();
    }

    //--------------------------------------------------------------------------

    State* StateMachine::CurrentState() const
    {
        return currentState_;
    }

    void StateMachine::CurrentState(State* state)
    {
        if (currentState_)
            currentState_->OnExit();
        currentState_ = state;
        currentState_->OnEnter();
    }

    State* StateMachine::PendingState() const
    {
        return pendingState_;
    }

    //--------------------------------------------------------------------------

    void StateMachine::ChangeState(State* state)
    {
        pendingState_ = state;
    }

    void StateMachine::Update(f32 dt)
    {
        // change states if there is a pending state
        if (pendingState_)
        {
            CurrentState(pendingState_);
            pendingState_ = 0;
        }

        // update the current state if it exists
        if (currentState_)
            currentState_->OnUpdate(dt);
    }
}
