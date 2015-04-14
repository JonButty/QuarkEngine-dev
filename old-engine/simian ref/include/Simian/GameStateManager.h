/*****************************************************************************/
/*!
\file		GameStateManager.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_GAMESTATEMANAGER_H_
#define SIMIAN_GAMESTATEMANAGER_H_

#include "SimianPlatform.h"
#include "StateMachine.h"

namespace Simian
{
    class SIMIAN_EXPORT GameState: public State
    {
    public:
        virtual void OnLoad() = 0;
        virtual void OnInit() = 0;
        virtual void OnDeinit() = 0;
        virtual void OnUnload() = 0;
    };

    class SIMIAN_EXPORT GameStateManager: public StateMachine
    {
    public:
        void CurrentState(State* state);
        GameState* CurrentState() const;
    public:
        GameStateManager(State* initialState);
        ~GameStateManager();
    };
}

#endif
