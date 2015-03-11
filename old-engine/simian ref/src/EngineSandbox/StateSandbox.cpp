/*************************************************************************/
/*!
\file		StateSandbox.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "EngineSandbox/StateSandbox.h"

#include "Simian/Game.h"
#include "Simian/ModelCache.h"
#include "Simian/MaterialCache.h"
#include "Simian/TextureCache.h"
#include "Simian/Angle.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/ParticleEmitterSphere.h"
#include "Simian/ParticleRendererBillboard.h"
#include "Simian/Shader.h"
#include "Simian/DataFileIO.h"
#include "Simian/ParticleRendererRibbonTrail.h"
#include "Simian/Math.h"

#include "Simian/VideoClip.h"

namespace Sandbox
{
    StateSandbox::StateSandbox()
        : controller_(0)
    {
    }

    //--------------------------------------------------------------------------

    void StateSandbox::OnEnter()
    {
    }

    void StateSandbox::OnLoad()
    {
        //model_ = Simian::Game::Instance().ModelCache().Load("Models/shader-sphere.dae");
        model_ = Simian::Game::Instance().ModelCache().Load("Models/Player.dae", "Animations/Player.anim");
        material_ = Simian::Game::Instance().MaterialCache().Load("Materials/Player_1.mat");
        fireMaterial_ = Simian::Game::Instance().MaterialCache().Load("Materials/Effect_Smoke.mat");
        //fireMaterial_ = Simian::Game::Instance().MaterialCache().Load("Materials/FireParticle.mat");
        //fireMaterial_ = Simian::Game::Instance().MaterialCache().Load("Materials/Effect_Smoke.mat");

        Simian::Game::Instance().LoadCaches();

        controller_ = model_->CreateAnimationController();

        Simian::Game::Instance().GraphicsDevice().CreateRenderTexture(renderTexture_);
        renderTexture_->LoadImmediate(Simian::Game::Instance().Size());

        camera_ = new Simian::Camera(&Simian::Game::Instance().GraphicsDevice());
        camera_->AddLayer(Simian::RenderQueue::RL_SCENE);
        camera_->ClearColor(Simian::Color(0.0f, 0.0f, 0.0f, 1.0f));
        camera_->Composite(true);
        camera_->Projection(Simian::Matrix::PerspectiveFOV(
            Simian::Degree(45), 4.0f/3.0f, 1.0f, 10000.0f));
        camera_->View(Simian::Matrix::LookAt(
            Simian::Vector3(0.0f, 5.0f, 30.0f),
            Simian::Vector3(0.0f, 5.0f, 0.0f),
            Simian::Vector3::UnitY));
        camera_->RenderTexture(renderTexture_);

        Simian::Game::Instance().RenderQueue().AddCamera(camera_);

        // create particle system
        particleSystem_ = new Simian::ParticleSystem();

        // add emitter and renderer
        Simian::ParticleEmitterSphere* emitter = particleSystem_->AddEmitter<Simian::ParticleEmitterSphere>();
        emitter->Radius(1.5f);

        /*Simian::ParticleRendererBillboard* renderer = particleSystem_->AddRenderer<Simian::ParticleRendererBillboard>();
        renderer->Camera(camera_);*/

        Simian::ParticleRendererRibbonTrail* renderer = particleSystem_->AddRenderer<Simian::ParticleRendererRibbonTrail>();
        renderer->SegmentCount(20);
        renderer->InterpolationInterval(0.3f);

        renderer->Device(&Simian::Game::Instance().GraphicsDevice());
        renderer->Material(fireMaterial_);

        // set up first particle frame
        Simian::ParticleSystemFrame start;
        start.Caps = Simian::ParticleSystem::FC_ALL;
        start.Time = 0.0f;
        start.Velocity = Simian::Vector3(0.0f, 3.0f, 0.0f);
        start.RandomVelocity = Simian::Vector3(3.0f, 3.0f, 3.0f);
        start.Scale = Simian::Vector3(3.0f, 3.0f, 3.0f);
        start.RandomRotation = Simian::Vector3(0.0f, 0.0f, Simian::Math::PI);
        //start.RandomAngularVelocity.Z(3.0f);
        start.Color = Simian::Color(0.0f, 0.0f, 0.0f, 0.0f);

        Simian::ParticleSystemFrame mid;
        mid.Caps = Simian::ParticleSystem::FC_COLOR;
        mid.Time = 0.15f;
        mid.Color = Simian::Color(1.0f, 1.0f, 1.0f, 1.0f);

        Simian::ParticleSystemFrame mid2;
        mid2.Caps = Simian::ParticleSystem::FC_COLOR;
        mid2.Time = 0.75f;
        mid2.Color = Simian::Color(1.0f, 1.0f, 1.0f, 1.0f);

        Simian::ParticleSystemFrame end;
        end.Caps = Simian::ParticleSystem::FC_ALL & ~Simian::ParticleSystem::FC_ROTATION;
        end.Time = 1.0f;
        end.Velocity = Simian::Vector3(0.0f, 10.0f, 0.0f);
        end.Color = Simian::Color(0.0f, 0.0f, 0.0f, 0.0f);
        end.Scale = Simian::Vector3(1.5f, 1.5f, 1.5f);
        //end.RandomAngularVelocity.Z(3.0f);

        particleSystem_->LifeTime(2.5f);
        particleSystem_->RandomLifeTime(1.5f);
        /*particleSystem_->SpawnAmount(3);
        particleSystem_->SpawnInterval(0.01f);*/
        particleSystem_->SpawnAmount(1);
        particleSystem_->SpawnInterval(0.05f);

        particleSystem_->AddFrame(start);
        particleSystem_->AddFrame(end);
        particleSystem_->AddFrame(mid);
        particleSystem_->AddFrame(mid2);

        particleSystem_->Load(200);
        particleSystem_->Transform(Simian::Matrix::Translation(0.0f, 0.0f, 10.0f));
    }

    void StateSandbox::OnInit()
    {
        controller_->PlayAnimation("Run");
    }

    void StateSandbox::OnUpdate( Simian::f32 dt )
    {
        controller_->Update(dt);
        model_->Draw(Simian::Game::Instance().RenderQueue(),
            *material_, Simian::Matrix::Scale(1.0f, 1.0f, 1.0f), Simian::RenderQueue::RL_SCENE,
            controller_);

        /*particleSystem_->Update(dt);
        particleSystem_->Draw(Simian::Game::Instance().RenderQueue(), 
            Simian::RenderQueue::RL_SCENE);*/

        static Simian::Vector3 distVec(0.0f, 5.0f, 30.0f);
        Simian::Matrix::RotationAxisAngle(Simian::Vector3::UnitY, Simian::Degree(100.0f * dt)).Transform(distVec);

        camera_->View(Simian::Matrix::LookAt(
            distVec,
            Simian::Vector3(0.0f, 5.0f, 0.0f),
            Simian::Vector3::UnitY));

        static Simian::Vector3 flameVec(0.0f, -2.0f, 10.0f);
        Simian::Matrix::RotationAxisAngle(Simian::Vector3::UnitY, Simian::Degree(50.0f * dt)).Transform(flameVec);
        particleSystem_->Transform(Simian::Matrix::Translation(flameVec));
    }

    void StateSandbox::OnDeinit()
    {

    }

    void StateSandbox::OnUnload()
    {
        particleSystem_->Unload();
        delete particleSystem_;

        Simian::Game::Instance().RenderQueue().RemoveCamera(camera_);
        delete camera_;

        renderTexture_->Unload();
        Simian::Game::Instance().GraphicsDevice().DestroyRenderTexture(renderTexture_);

        model_->DestroyAnimationController(controller_);
    }

    void StateSandbox::OnExit()
    {

    }
}
