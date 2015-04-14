/*************************************************************************/
/*!
\file		GCUISlider.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCUISlider.h"
#include "ComponentTypes.h"
#include "GCUIButton.h"

#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/CTransform.h"
#include "Simian/Vector3.h"
#include "Simian/Mouse.h"
#include "Simian/Game.h"
#include "Simian/Math.h"
#include "Simian/CSoundEmitter.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCUISlider> GCUISlider::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_UISLIDER);

    GCUISlider::GCUISlider()
        : value_(0.0f),
          limit_(0.0f),
          sounds_(0)
    {
    }

    //--------------------------------------------------------------------------

    Simian::f32 GCUISlider::Value() const
    {
        return value_;
    }

    void GCUISlider::Value( Simian::f32 val )
    {
        value_ = val;

        // set the transform
        Simian::f32 x = value_ * limit_ * 2.0f - limit_;
        control_->Position(Simian::Vector3(x, control_->Position().Y(), control_->Position().Z()));
		control_->RecomputeWorld();
    }

    Simian::f32 GCUISlider::Limit() const
    {
        return limit_;
    }

    void GCUISlider::Limit( Simian::f32 val )
    {
        limit_ = val;

        // set the transform
        Simian::f32 x = value_ * limit_ * 2.0f - limit_;
        control_->Position(Simian::Vector3(x, control_->Position().Y(), control_->Position().Z()));
    }

    //--------------------------------------------------------------------------

    void GCUISlider::update_( Simian::DelegateParameter& )
    {
        if (moving_ && Simian::Mouse::IsDepressed(Simian::Mouse::KEY_LBUTTON))
        {
            moving_ = false;
            if (sounds_)
                sounds_->Play(0);
        }

        if (!moving_)
            return;

        // moving, so get the mouse position
        Simian::Vector2 mousePos = Simian::Mouse::GetMouseClientPosition();
        mousePos -= Simian::Game::Instance().Size() * 0.5f;
        mousePos.Y(-mousePos.Y());
        mousePos *= Simian::Vector2(1.0f/transform_->World()[0][0], 1.0f/transform_->World()[1][1]);

        float xLocal = mousePos.X() - transform_->World().Position().X();
        xLocal = Simian::Math::Clamp(xLocal, -limit_, limit_);
        value_ = (xLocal + limit_)/(2.0f * limit_);

        control_->Position(Simian::Vector3(xLocal, control_->Position().Y(), control_->Position().Z()));
    }

    //--------------------------------------------------------------------------

    void GCUISlider::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE,
            Simian::Delegate::CreateDelegate<GCUISlider, &GCUISlider::update_>(this));
    }

    void GCUISlider::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);

        Simian::Entity* entity = ParentEntity()->ChildByName("Control");
        entity->ComponentByType(Simian::C_TRANSFORM, control_);

        GCUIButton* button;
        entity->ComponentByType(GC_UIBUTTON, button);
        button->AddListener(this);

        ComponentByType(Simian::C_SOUNDEMITTER, sounds_);

        // set the transform
        Simian::f32 x = value_ * limit_ * 2.0f - limit_;
        control_->Position(Simian::Vector3(x, control_->Position().Y(), control_->Position().Z()));
		control_->RecomputeWorld();
    }

    void GCUISlider::OnTriggered( Simian::DelegateParameter& )
    {
        moving_ = true;
        if (sounds_)
            sounds_->Play(1);
    }

    void GCUISlider::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("Value", value_);
        data.AddData("Limit", limit_);
    }

    void GCUISlider::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("Value", value_, value_);
        data.Data("Limit", limit_, limit_);
    }
}
