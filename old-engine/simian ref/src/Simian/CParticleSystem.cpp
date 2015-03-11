/************************************************************************/
/*!
\file		CParticleSystem.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"
#include "Simian/ParticleSystemFrame.h"
#include "Simian/EngineComponents.h"
#include "Simian/EnginePhases.h"
#include "Simian/RenderQueue.h"
#include "Simian/CTransform.h"
#include "Simian/LogManager.h"

namespace Simian
{
    FactoryPlant<EntityComponent, CParticleSystem> CParticleSystem::factoryPlant_(
        &GameFactory::Instance().ComponentFactory(), C_PARTICLESYSTEM);

    Simian::RenderQueue* CParticleSystem::renderQueue_ = 0;

    CParticleSystem::CParticleSystem()
        : particleSystem_(new Simian::ParticleSystem()),
          transform_(0),
          totalParticles_(50),
          layer_(Simian::RenderQueue::RL_SCENE)
    {
    }

    CParticleSystem::~CParticleSystem()
    {
        delete *particleSystem_;
        particleSystem_.Set(0);
    }

    //--------------------------------------------------------------------------

    Simian::ParticleSystem* CParticleSystem::ParticleSystem() const
    {
        return &(*particleSystem_);
    }

    Simian::u8 CParticleSystem::Layer() const
    {
        return layer_;
    }

    Simian::RenderQueue* CParticleSystem::RenderQueue()
    {
        return renderQueue_;
    }

    void CParticleSystem::RenderQueue( Simian::RenderQueue* val )
    {
        renderQueue_ = val;
    }

    //--------------------------------------------------------------------------

    CParticleSystem& CParticleSystem::operator=( const CParticleSystem& particles )
    {
        EntityComponent::operator=(particles);

        totalParticles_ = particles.totalParticles_;
        layer_ = particles.layer_;

        particleSystem_->SpawnInterval(particles.ParticleSystem()->SpawnInterval());
        particleSystem_->RandomSpawnInterval(particles.ParticleSystem()->RandomSpawnInterval());
        particleSystem_->SpawnAmount(particles.ParticleSystem()->SpawnAmount());
        particleSystem_->RandomSpawnAmount(particles.ParticleSystem()->RandomSpawnAmount());
        particleSystem_->LifeTime(particles.ParticleSystem()->LifeTime());
        particleSystem_->RandomLifeTime(particles.ParticleSystem()->RandomLifeTime());

        // copy all the frames for the particle system and we should be dandy.
        for (u32 i = 0; i < particles.ParticleSystem()->Frames().size(); ++i)
            particleSystem_->AddFrame(particles.ParticleSystem()->Frames()[i]);

        return *this;
    }

    //--------------------------------------------------------------------------

    void CParticleSystem::parseColor_( Color& color, const FileObjectSubNode* colorNode )
    {
        if (colorNode)
        {
            f32 r;
            colorNode->Data("R", r, color.R());
            color.R(r);

            f32 g;
            colorNode->Data("G", g, color.G());
            color.G(g);

            f32 b;
            colorNode->Data("B", b, color.B());
            color.B(b);

            f32 a;
            colorNode->Data("A", a, color.A());
            color.A(a);
        }
    }

    void CParticleSystem::parseVec3_( Vector3& vec, const FileObjectSubNode* vecNode )
    {
        if (vecNode)
        {
            f32 x;
            vecNode->Data("X", x, vec.X());
            vec.X(x);

            f32 y;
            vecNode->Data("Y", y, vec.Y());
            vec.Y(y);

            f32 z;
            vecNode->Data("Z", z, vec.Z());
            vec.Z(z);
        }
    }

    void CParticleSystem::writeColor_( const Color& vec, FileObjectSubNode* colorNode )
    {
        colorNode->AddData("R", vec.R());
        colorNode->AddData("G", vec.G());
        colorNode->AddData("B", vec.B());
        colorNode->AddData("A", vec.A());
    }

    void CParticleSystem::writeVec3_( const Vector3& vec, FileObjectSubNode* vecNode )
    {
        vecNode->AddData("X", vec.X());
        vecNode->AddData("Y", vec.Y());
        vecNode->AddData("Z", vec.Z());
    }

    void CParticleSystem::parseFrame_( ParticleSystemFrame& frame, const FileObjectSubNode* frameNode )
    {
        // read time
        frameNode->Data("Time", frame.Time, frame.Time);

        // reset caps
        frame.Caps = 0;

        // read velocity
        bool cap;
        frameNode->Data("UseVelocity", cap, false);
        frame.Caps |= cap ? Simian::ParticleSystem::FC_VELOCITY : 0;
        parseVec3_(frame.Velocity, frameNode->Object("Velocity"));

        frameNode->Data("UseRandomVelocity", cap, false);
        frame.Caps |= cap ? Simian::ParticleSystem::FC_RANDOMVELOCITY : 0;
        parseVec3_(frame.RandomVelocity, frameNode->Object("RandomVelocity"));

        frameNode->Data("UseTangentVelocity", cap, false);
        frame.Caps |= cap ? Simian::ParticleSystem::FC_TANGENTVELOCITY : 0;
        parseVec3_(frame.TangentVelocity, frameNode->Object("TangentVelocity"));

        frameNode->Data("UseRandomTangentVelocity", cap, false);
        frame.Caps |= cap ? Simian::ParticleSystem::FC_RANDOMTANGENTVELOCITY : 0;
        parseVec3_(frame.RandomTangentVelocity, frameNode->Object("RandomTangentVelocity"));

        frameNode->Data("UseScale", cap, false);
        frame.Caps |= cap ? Simian::ParticleSystem::FC_SCALE : 0;
        parseVec3_(frame.Scale, frameNode->Object("Scale"));

        frameNode->Data("UseRandomScale", cap, false);
        frame.Caps |= cap ? Simian::ParticleSystem::FC_RANDOMSCALE : 0;
        parseVec3_(frame.RandomScale, frameNode->Object("RandomScale"));

        frameNode->Data("UseRotation", cap, false);
        frame.Caps |= cap ? Simian::ParticleSystem::FC_ROTATION : 0;
        parseVec3_(frame.Rotation, frameNode->Object("Rotation"));

        frameNode->Data("UseRandomRotation", cap, false);
        frame.Caps |= cap ? Simian::ParticleSystem::FC_RANDOMROTATION : 0;
        parseVec3_(frame.RandomRotation, frameNode->Object("RandomRotation"));

        frameNode->Data("UseAngularVelocity", cap, false);
        frame.Caps |= cap ? Simian::ParticleSystem::FC_ANGULARVELOCITY : 0;
        parseVec3_(frame.AngularVelocity, frameNode->Object("AngularVelocity"));

        frameNode->Data("UseRandomAngularVelocity", cap, false);
        frame.Caps |= cap ? Simian::ParticleSystem::FC_RANDOMANGULARVELOCITY : 0;
        parseVec3_(frame.RandomAngularVelocity, frameNode->Object("RandomAngularVelocity"));

        frameNode->Data("UseColor", cap, false);
        frame.Caps |= cap ? Simian::ParticleSystem::FC_COLOR : 0;
        parseColor_(frame.Color, frameNode->Object("Color"));

        frameNode->Data("UseRandomColor", cap, false);
        frame.Caps |= cap ? Simian::ParticleSystem::FC_RANDOMCOLOR : 0;
        parseColor_(frame.RandomColor, frameNode->Object("RandomColor"));
    }

    void CParticleSystem::update_( DelegateParameter& param )
    {
        EntityUpdateParameter* p;
        param.As(p);
        particleSystem_->Transform(transform_->World());
        particleSystem_->Update(p->Dt);
    }

    void CParticleSystem::render_( DelegateParameter& )
    {
        particleSystem_->Draw(*renderQueue_, layer_, transform_->World());
    }

    //--------------------------------------------------------------------------

    void CParticleSystem::OnSharedLoad()
    {
        RegisterCallback(P_UPDATE, Delegate::CreateDelegate<CParticleSystem, &CParticleSystem::update_>(this));
        RegisterCallback(P_RENDER, Delegate::CreateDelegate<CParticleSystem, &CParticleSystem::render_>(this));
    }

    void CParticleSystem::OnInit()
    {
        // all renderers and emitters must be added by this point
        particleSystem_->Load(totalParticles_);

        // get the transform component
        ComponentByType(C_TRANSFORM, transform_);
    }

    void CParticleSystem::OnDeinit()
    {
        particleSystem_->Unload();
    }

    void CParticleSystem::Deserialize( const FileObjectSubNode& data )
    {
        data.Data("TotalParticles", totalParticles_, totalParticles_);

        // read a float
        float f;
        data.Data("SpawnInterval", f, particleSystem_->SpawnInterval());
        particleSystem_->SpawnInterval(f);

        data.Data("RandomSpawnInterval", f, particleSystem_->RandomSpawnInterval());
        particleSystem_->RandomSpawnInterval(f);

        u32 u;
        data.Data("SpawnAmount", u, particleSystem_->SpawnAmount());
        particleSystem_->SpawnAmount(u);

        data.Data("RandomSpawnAmount", u, particleSystem_->RandomSpawnAmount());
        particleSystem_->RandomSpawnAmount(u);

        data.Data("LifeTime", f, particleSystem_->LifeTime());
        particleSystem_->LifeTime(f);

        data.Data("RandomLifeTime", f, particleSystem_->RandomLifeTime());
        particleSystem_->RandomLifeTime(f);

        bool b;
        data.Data("Enabled", b, particleSystem_->Enabled());
        particleSystem_->Enabled(b);

        std::string layer;
        data.Data("Layer", layer, "");
        layer_ = static_cast<u8>(
                 layer == "PRESCENE" ? Simian::RenderQueue::RL_PRESCENE :
                 layer == "SCENE" ? Simian::RenderQueue::RL_SCENE :
                 layer == "POSTSCENE" ? Simian::RenderQueue::RL_POSTSCENE :
                 layer == "PREUI" ? Simian::RenderQueue::RL_PREUI :
                 layer == "UI" ? Simian::RenderQueue::RL_UI :
                 layer == "POSTUI" ? Simian::RenderQueue::RL_POSTUI :
                 layer_);

        // frames
        const FileArraySubNode* frames = data.Array("Frames");
        if (frames)
        {
            particleSystem_->ClearFrames();

            // add first frame and last frame explicitly
            ParticleSystemFrame firstFrame;
            parseFrame_(firstFrame, frames->Object(0));
            particleSystem_->AddFrame(firstFrame);

            // if there are more than 1 frame
            if (frames->Size() > 1)
            {
                ParticleSystemFrame lastFrame;
                parseFrame_(lastFrame, frames->Object(frames->Size() - 1));
                particleSystem_->AddFrame(lastFrame);
            }

            // add any inbetweens
            for (u32 i = 1; i < frames->Size() - 1; ++i)
            {
                ParticleSystemFrame frame;
                parseFrame_(frame, frames->Object(i));
                particleSystem_->AddFrame(frame);
            }
        }
    }

    void CParticleSystem::Serialize( FileObjectSubNode& data )
    {
        data.AddData("TotalParticles", totalParticles_);

        data.AddData("SpawnInterval", particleSystem_->SpawnInterval());
        data.AddData("RandomSpawnInterval", particleSystem_->RandomSpawnInterval());

        data.AddData("SpawnAmount", particleSystem_->SpawnAmount());
        data.AddData("RandomSpawnAmount", particleSystem_->RandomSpawnAmount());

        data.AddData("LifeTime", particleSystem_->LifeTime());
        data.AddData("RandomLifeTime", particleSystem_->RandomLifeTime());

        data.AddData("Enabled", particleSystem_->Enabled());

        std::string layer = layer_ == Simian::RenderQueue::RL_PRESCENE ? "PRESCENE" :
                            layer_ == Simian::RenderQueue::RL_SCENE ? "SCENE" :
                            layer_ == Simian::RenderQueue::RL_POSTSCENE ? "POSTSCENE" :
                            layer_ == Simian::RenderQueue::RL_PREUI ? "PREUI" :
                            layer_ == Simian::RenderQueue::RL_UI ? "UI" :
                            layer_ == Simian::RenderQueue::RL_POSTUI ? "POSTUI" :
                            "SCENE";
        data.AddData("Layer", layer);

        // add frames
        FileArraySubNode* frames = data.AddArray("Frames");
        for (u32 i = 0; i < particleSystem_->Frames().size(); ++i)
        {
            FileObjectSubNode* frameNode = frames->AddObject("Frame");
            const ParticleSystemFrame& frame = particleSystem_->Frames()[i];

            frameNode->AddData("Time", frame.Time);

            // write frame caps
            frameNode->AddData<bool>("UseVelocity", frame.Caps & Simian::ParticleSystem::FC_VELOCITY);
            writeVec3_(frame.Velocity, frameNode->AddObject("Velocity"));

            frameNode->AddData<bool>("UseRandomVelocity", frame.Caps & Simian::ParticleSystem::FC_RANDOMVELOCITY);
            writeVec3_(frame.RandomVelocity, frameNode->AddObject("RandomVelocity"));

            frameNode->AddData<bool>("UseTangentVelocity", frame.Caps & Simian::ParticleSystem::FC_TANGENTVELOCITY);
            writeVec3_(frame.TangentVelocity, frameNode->AddObject("TangentVelocity"));

            frameNode->AddData<bool>("UseRandomTangentVelocity", frame.Caps & Simian::ParticleSystem::FC_RANDOMTANGENTVELOCITY);
            writeVec3_(frame.RandomTangentVelocity, frameNode->AddObject("RandomTangentVelocity"));
            
            frameNode->AddData<bool>("UseScale", frame.Caps & Simian::ParticleSystem::FC_SCALE);
            writeVec3_(frame.Scale, frameNode->AddObject("Scale"));

            frameNode->AddData<bool>("UseRandomScale", frame.Caps & Simian::ParticleSystem::FC_RANDOMSCALE);
            writeVec3_(frame.RandomScale, frameNode->AddObject("RandomScale"));

            frameNode->AddData<bool>("UseRotation", frame.Caps & Simian::ParticleSystem::FC_ROTATION);
            writeVec3_(frame.Rotation, frameNode->AddObject("Rotation"));

            frameNode->AddData<bool>("UseRandomRotation", frame.Caps & Simian::ParticleSystem::FC_RANDOMROTATION);
            writeVec3_(frame.RandomRotation, frameNode->AddObject("RandomRotation"));

            frameNode->AddData<bool>("UseAngularVelocity", frame.Caps & Simian::ParticleSystem::FC_ANGULARVELOCITY);
            writeVec3_(frame.AngularVelocity, frameNode->AddObject("AngularVelocity"));

            frameNode->AddData<bool>("UseRandomAngularVelocity", frame.Caps & Simian::ParticleSystem::FC_RANDOMANGULARVELOCITY);
            writeVec3_(frame.RandomAngularVelocity, frameNode->AddObject("RandomAngularVelocity"));

            frameNode->AddData<bool>("UseColor", frame.Caps & Simian::ParticleSystem::FC_COLOR);
            writeColor_(frame.Color, frameNode->AddObject("Color"));

            frameNode->AddData<bool>("UseRandomColor", frame.Caps & Simian::ParticleSystem::FC_RANDOMCOLOR);
            writeColor_(frame.RandomColor, frameNode->AddObject("RandomColor"));
        }
    }
}
