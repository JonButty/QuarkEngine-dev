/*************************************************************************/
/*!
\file		GCFlameEffect.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCFlameEffect.h"
#include "ComponentTypes.h"
#include "GCParticleEmitterTriangle.h"

#include "Simian\EnginePhases.h"
#include "Simian\EngineComponents.h"
#include "Simian\ParticleSystem.h"
#include "Simian\CParticleSystem.h"
#include "Simian\Scene.h"
#include "Simian\Math.h"
#include "EntityTypes.h"
#include "GCFlamingSnakeEffect.h"
#include "Simian\CTransform.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCFlameEffect> GCFlameEffect::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_FLAMEEFFECT);

    static const Simian::s32 SNAKE_MIN = 6;
    static const Simian::s32 SNAKE_MAX = 10;

    GCFlameEffect::GCFlameEffect()
        : flames_(0),
          lights_(0),
          autoDestroy_(true),
          timer_(0)
    {
    }

    //--------------------------------------------------------------------------

    void GCFlameEffect::Vertices( const Simian::Vector3* vertices )
    {
        vertices_[0] = vertices[0];
        vertices_[1] = vertices[1];
        vertices_[2] = vertices[2];
    }

    bool GCFlameEffect::AutoDestroy() const
    {
        return autoDestroy_;
    }

    void GCFlameEffect::AutoDestroy( bool val )
    {
        autoDestroy_ = val;
    }

    //--------------------------------------------------------------------------

    void GCFlameEffect::update_( Simian::DelegateParameter& param )
    {
        const Simian::f32 FLAME_DURATION = 0.1f;

        Simian::EntityUpdateParameter* p;
        param.As(p);

        timer_ += p->Dt;

        if (timer_ < FLAME_DURATION)
        {
            flames_->ParticleSystem()->Enabled(true);
            lights_->ParticleSystem()->Enabled(true);
        }
        else if (timer_ < FLAME_DURATION + flames_->ParticleSystem()->LifeTime() + 
                          flames_->ParticleSystem()->RandomLifeTime())
        {
            flames_->ParticleSystem()->Enabled(false);
            lights_->ParticleSystem()->Enabled(false);
        }
        else
        {
            // destroy the entity if its auto destroy
            if (autoDestroy_)
            {
                if (ParentEntity()->ParentEntity())
                    ParentEntity()->ParentEntity()->RemoveChild(ParentEntity());
                else
                    ParentScene()->RemoveEntity(ParentEntity());
            }
            else
                timer_ = 0.0f;
        }
    }

    //--------------------------------------------------------------------------

    void GCFlameEffect::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, 
            Simian::Delegate::CreateDelegate<GCFlameEffect, &GCFlameEffect::update_>(this));
    }

    void GCFlameEffect::OnInit()
    {
        ComponentByType(Simian::C_PARTICLESYSTEM, flames_);
        ComponentByType(GC_PARTICLEEMITTERTRIANGLE, flamesEmitter_);

        Simian::Entity* lightParticles = ParentEntity()->ChildByName("LightParticles");
        lightParticles->ComponentByType(Simian::C_PARTICLESYSTEM, lights_);
        lightParticles->ComponentByType(GC_PARTICLEEMITTERTRIANGLE, lightsEmitter_);

        lightsEmitter_->Vertices(vertices_);
        flamesEmitter_->Vertices(vertices_);

        // spawn some flaming snakes!
        Simian::s32 snakeCount = Simian::Math::Random(SNAKE_MIN, SNAKE_MAX);
        for (int i = 0; i < snakeCount; ++i)
        {
            Simian::Entity* snake = ParentScene()->CreateEntity(E_FLAMINGSNAKEEFFECT);
            GCFlamingSnakeEffect* snakeComp;
            snake->ComponentByType(GC_FLAMINGSNAKEEFFECT, snakeComp);
            snakeComp->Direction(Simian::Vector3(Simian::Math::Random(-1.0f, 1.0f), 0,
                Simian::Math::Random(-1.0f, 1.0f)));
            Simian::CTransform* transform;
            snake->ComponentByType(Simian::C_TRANSFORM, transform);

            // randomize a position in the triangle
            Simian::Vector3 uAxis = vertices_[1] - vertices_[0];
            Simian::Vector3 vAxis = vertices_[2] - vertices_[0];

            Simian::f32 u, v;
            if (Simian::Math::Random(0, 2))
            {
                u = Simian::Math::Random(0.0f, 1.0f);
                v = Simian::Math::Random(0.0f, 1.0f - u);
            }
            else
            {
                v = Simian::Math::Random(0.0f, 1.0f);
                u = Simian::Math::Random(0.0f, 1.0f - v);
            }

            transform->Position(vertices_[0] + u * uAxis + v * vAxis);

            ParentScene()->AddEntity(snake);
        }
    }

    void GCFlameEffect::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("AutoDestroy", autoDestroy_);
    }

    void GCFlameEffect::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("AutoDestroy", autoDestroy_, autoDestroy_);
    }
}
