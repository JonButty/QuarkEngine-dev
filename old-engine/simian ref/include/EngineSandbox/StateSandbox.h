/*************************************************************************/
/*!
\file		StateSandbox.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef SANDBOX_STATESANDBOX_H_
#define SANDBOX_STATESANDBOX_H_

#include "Simian/GameStateManager.h"
#include "Simian/Singleton.h"
#include "Simian/Material.h"
#include "Simian/Model.h"
#include "Simian/AnimationController.h"
#include "Simian/Camera.h"
#include "Simian/RenderTexture.h"
#include "Simian/ParticleSystem.h"

namespace Sandbox
{
    class StateSandbox: public Simian::GameState, public Simian::Singleton<StateSandbox>
    {
    private:
        Simian::AnimationController* controller_;
        Simian::Model* model_;
        Simian::Material* material_;
        Simian::Camera* camera_;
        Simian::RenderTexture* renderTexture_;

        // particle system test
        Simian::ParticleSystem* particleSystem_;
        Simian::Material* fireMaterial_;
    public:
        StateSandbox();

        void OnEnter();
        void OnLoad();
        void OnInit();
        void OnUpdate(Simian::f32 dt);
        void OnDeinit();
        void OnUnload();
        void OnExit();
    };
}

#endif
