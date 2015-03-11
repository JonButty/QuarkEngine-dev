/*************************************************************************/
/*!
\file		GCGameOver.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/12
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/

#include "GCGameOver.h"
#include "ComponentTypes.h"
#include "GCUI.h"
#include "GCPlayerLogic.h"

#include "Simian\EnginePhases.h"
#include "Simian\Game.h"
#include "Simian\MaterialCache.h"
#include "Simian\Math.h"
#include "Simian\Color.h"
#include "Simian\CSprite.h"
#include "Simian\Material.h"
#include "Simian\Shader.h"
#include "Simian\ShaderFloatParameter.h"
#include "Simian\CCamera.h"
#include "Simian\Plane.h"
#include "Simian\Ray.h"
#include "Simian\Camera.h"
#include "Simian\WMouse.h"
#include "Simian\CTransform.h"
#include "StateLoading.h"
#include "StateGame.h"

namespace Descension
{
    static const Simian::f32 FADE_BLACK_DURATION = 1.0f;
    static const Simian::f32 GAMEOVER_DELAY = 0.5f;
    static const Simian::f32 GAMEOVER_DURATION = 1.5f;
    static const Simian::f32 REVIVE_DELAY = 1.0f;
    static const Simian::f32 REVIVE_DURATION = 1.5f;
    static const Simian::f32 CLICK_CONTROL_DURATION = 3.0f;
    static const Simian::f32 FADE_DISTANCE = 2.5f;
    static const Simian::f32 MIN_BLACKNESS = 0.1f;

    Simian::FactoryPlant<Simian::EntityComponent, GCGameOver> GCGameOver::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_GAMEOVER);

    GCGameOver::GCGameOver()
        : title_(0),
          revive_(0),
          background_(0),
          healthCriticalMaterial_(0),
          timer_(0.0f),
          activated_(false),
          radius_(1.0f)
    {
    }

    //--------------------------------------------------------------------------

    void GCGameOver::update_( Simian::DelegateParameter& param)
    {
        Simian::EntityUpdateParameter* p;
        param.As(p);

        if (activated_)
        {
            timer_ += p->Dt;

            Simian::f32 intp = timer_/FADE_BLACK_DURATION;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
            Simian::f32 amount = 1.0f - intp;
            redness_->SetFloat(&amount, 1);

            intp = (timer_ - GAMEOVER_DELAY)/GAMEOVER_DURATION;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
            title_->Color(Simian::Color(1.0f, 1.0f, 1.0f, intp));
            background_->Color(Simian::Color(1.0f, 1.0f, 1.0f, intp));

            intp = (timer_ - REVIVE_DELAY)/REVIVE_DURATION;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
            revive_->Color(Simian::Color(1.0f, 1.0f, 1.0f, intp));

            Simian::CCamera* camera;
            Simian::CCamera::SceneCamera(camera);
            Simian::Vector3 intersection;

            // allow player to restart by clicking the player's model
            // find the mouse intersection with the floor
            if (Simian::Plane::Z.Intersect(Simian::Ray::FromProjection(
                camera->Camera().View().Inversed(), camera->Camera().Projection(),
                Simian::Mouse::GetMouseNormalizedPosition()), intersection))
            {
                // find the distance
                Simian::f32 dist = (intersection - GCPlayerLogic::Instance()->Transform().WorldPosition()).Length();
                    
                // send blackness based on distance
                amount = dist/FADE_DISTANCE + MIN_BLACKNESS;
                amount = 1.0f - Simian::Math::Clamp(amount, 0.0f, 1.0f);
                redness_->SetFloat(&amount, 1);

                if (timer_ > CLICK_CONTROL_DURATION)
                {
                    if (dist < radius_)
                    {
                        // show aura particles.
                        GCPlayerLogic::Instance()->ShowReviveBeam();

                        if (Simian::Mouse::IsTriggered(Simian::Mouse::KEY_LBUTTON))
                            GCPlayerLogic::Instance()->RevivePlayer();
                    }
                    else
                    {
                        // hide aura particles
                        GCPlayerLogic::Instance()->HideReviveBeam();
                    }
                }
            }
        }
    }

    //--------------------------------------------------------------------------

    void GCGameOver::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCGameOver, &GCGameOver::update_>(this));

        healthCriticalMaterial_ = 
            Simian::Game::Instance().MaterialCache().Load("Materials/Compositor_HealthCritical.mat");
    }

    void GCGameOver::OnAwake()
    {
        Simian::Entity* entity = ParentEntity()->ChildByName("Title");
        entity->ComponentByType(Simian::C_SPRITE, title_);

        entity = ParentEntity()->ChildByName("Revive");
        entity->ComponentByType(Simian::C_SPRITE, revive_);

        entity = ParentEntity()->ChildByName("Background");
        entity->ComponentByType(Simian::C_SPRITE, background_);

        (*healthCriticalMaterial_)[0].Shader()->FragmentProgramConstant("Redness", redness_);
        Simian::f32 amount = 1.0f;
        redness_->SetFloat(&amount, 1);
    }

    void GCGameOver::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("Radius", radius_);
    }

    void GCGameOver::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("Radius", radius_, radius_);
    }

    void GCGameOver::Show()
    {
        activated_ = true;
        GCUI::Instance()->Enabled(false);
    }
}
