/*****************************************************************************/
/*!
\file		GameStateManager.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/GameStateManager.h"

namespace Simian
{
    GameStateManager::GameStateManager(State* initial)
        : StateMachine(initial)
    {
    }

    GameStateManager::~GameStateManager()
    {
        if (currentState_)
        {
            GameState* currentState = reinterpret_cast<GameState*>(currentState_);
            currentState->OnDeinit();
            currentState->OnUnload();
        }
    }

    //--------------------------------------------------------------------------

    void GameStateManager::CurrentState(State* s)
    {
        GameState* currentState = reinterpret_cast<GameState*>(currentState_);
        bool load = currentState != s;
        
        if (currentState)
        {
            currentState->OnDeinit();
            if (load)
                currentState->OnUnload();
            currentState->OnExit();
        }
        currentState_ = s;
        currentState = reinterpret_cast<GameState*>(currentState_);
        currentState_->OnEnter();
        if (load)
            currentState->OnLoad();
        currentState->OnInit();
    }

    GameState* GameStateManager::CurrentState() const
    {
        return reinterpret_cast<GameState*>(currentState_);
    }
}
