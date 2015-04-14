/*************************************************************************/
/*!
\file		GCUIButton.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCUIButton.h"
#include "ComponentTypes.h"

#include "Simian\EnginePhases.h"
#include "Simian\Game.h"
#include "Simian\MaterialCache.h"
#include "Simian\EngineComponents.h"
#include "Simian\Mouse.h"
#include "Simian\CSprite.h"
#include "Simian\CTransform.h"
#include "Simian\LogManager.h"
#include "Simian\CSoundEmitter.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCUIButton> GCUIButton::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_UIBUTTON);

    GCUIButton::GCUIButton()
        : normal_(0),
          hover_(0),
          transform_(0),
          sprite_(0),
          sounds_(0),
          onHover_(false),
          enabled_(true),
          hitArea_(1.0f, 1.0f)
    {
    }

    //--------------------------------------------------------------------------

    bool GCUIButton::Enabled() const
    {
        return enabled_;
    }

    void GCUIButton::Enabled( bool val )
    {
        enabled_ = val;
    }

    //--------------------------------------------------------------------------

    void GCUIButton::update_( Simian::DelegateParameter& )
    {
        if (!enabled_ || !sprite_->Visible())
        {
            sprite_->Material(normal_);
            onHover_ = false;
            return;
        }

        Simian::Vector2 mousePos = Simian::Mouse::GetMouseClientPosition();
        // convert to ui coordinate system
        mousePos.X(mousePos.X() - Simian::Game::Instance().Size().X() * 0.5f);
        mousePos.Y(Simian::Game::Instance().Size().Y() * 0.5f - mousePos.Y());

        Simian::Vector2 scale = Simian::Vector2(transform_->World()[0][0],
                                                transform_->World()[1][1]);
        Simian::Vector2 hsize = scale * sprite_->Size() * hitArea_ * 0.5f;
        Simian::Vector2 pos = Simian::Vector2(transform_->World().Position().X(),
            transform_->World().Position().Y());

        // outside
        if (mousePos.X() < pos.X() - hsize.X() || mousePos.X() > pos.X() + hsize.X() ||
            mousePos.Y() < pos.Y() - hsize.Y() || mousePos.Y() > pos.Y() + hsize.Y())
        {
            sprite_->Material(normal_);
            onHover_ = false;
            return;
        }
        
        sprite_->Material(hover_);
        if (sounds_ && !onHover_)
        {
            onHover_ = true;
            sounds_->Play(0);
        }

        // if the left trigger is pressed trigger the button
        if (Simian::Mouse::IsTriggered(Simian::Mouse::KEY_LBUTTON))
        {
            Trigger(this);
            if (sounds_)
                sounds_->Play(1);
        }
    }

    //--------------------------------------------------------------------------

    void GCUIButton::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, 
            Simian::Delegate::CreateDelegate<GCUIButton, &GCUIButton::update_>(this));

        normal_ = Simian::Game::Instance().MaterialCache().Load(materialFileNormal_);
        hover_ = Simian::Game::Instance().MaterialCache().Load(materialFileHover_);
    }

    void GCUIButton::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(Simian::C_SPRITE, sprite_);
        ComponentByType(Simian::C_SOUNDEMITTER, sounds_);
    }

    void GCUIButton::Serialize( Simian::FileObjectSubNode& data )
    {
        GCTrigger::Serialize(data);

        data.AddData("NormalMaterialFile", materialFileNormal_);
        data.AddData("HoverMaterialFile", materialFileHover_);

        Simian::FileObjectSubNode* hitArea = data.AddObject("HitArea");
        hitArea->AddData("Width", hitArea_.X());
        hitArea->AddData("Height", hitArea_.Y());
    }

    void GCUIButton::Deserialize( const Simian::FileObjectSubNode& data )
    {
        GCTrigger::Deserialize(data);

        data.Data("NormalMaterialFile", materialFileNormal_, materialFileNormal_);
        data.Data("HoverMaterialFile", materialFileHover_, materialFileHover_);

        const Simian::FileObjectSubNode* hitArea = data.Object("HitArea");
        if (hitArea)
        {
            float width, height;
            hitArea->Data("Width", width, hitArea_.X());
            hitArea->Data("Height", height, hitArea_.Y());
            hitArea_ = Simian::Vector2(width, height);
        }
    }
}
