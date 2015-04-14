/*****************************************************************************/
/*!
\file		StateMachine.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_STATEMACHINE_H_
#define SIMIAN_STATEMACHINE_H_

#include "SimianPlatform.h"

namespace Simian
{
    /** Interface for StateMachine states. */
    class SIMIAN_EXPORT State
    {
    public:
        /** Called when a state is activated.
            @remarks
                This callback happens at the end of StateMachine's update.

                States are not changed immediately, you may use 
                StateMachine::CurrentState to acheive this.
        */
        virtual void OnEnter() = 0;
        /** Called when a state is deactivated. 
            @remarks
                This callback happens at the end of StateMachine's update.

                States are not changed immediately, you may use 
                StateMachine::CurrentState to acheive this.
        */
        virtual void OnExit() = 0;
        /** Called every update cycle of the StateMachine while the state is 
            active. */
        virtual void OnUpdate(f32 dt) = 0;
    };

    /** Utility class for creating finite state machines. */
    class SIMIAN_EXPORT StateMachine
    {
    protected:
        State* currentState_;
        State* pendingState_;
    public:
        /** Gets the current state. */
        State* CurrentState() const;
        /** Sets the state immediately. 
            @remarks
                Consider using StateMachine::ChangeState instead to prevent
                initialization/deinitialization issues that may arise if a 
                state is changed mid-update.
            @see
                StateMachine::ChangeState
        */
        virtual void CurrentState(State* state);

        /** Gets the pending state. */
        State* PendingState() const;
    public:
        /** Creates a new StateMachine.
            @remarks
                initialState is not set to the current state until 
                StateMachine::Update is called at least once.
        */
        StateMachine(State* initialState);
        /** Destructs the state machine (calls the onexit of the current state). */
        virtual ~StateMachine();

        /** Requests a state change.
            @remarks
                The state is only changed at the end of the 
                StateMachine::Update. It is not changed immediately. You can
                achieve immediate state change using StateMachine::CurrentState.
            @see
                StateMachine::CurrentState
        */
        void ChangeState(State* state);

        /** Updates and drives the StateMachine. */
        void Update(f32 dt);
    };
}

#endif
