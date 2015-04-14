/*************************************************************************/
/*!
\file		GCUICheckbox.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCUICheckbox.h"
#include "ComponentTypes.h"
#include "Simian\Color.h"
#include "Simian\CSprite.h"
#include "Simian\CSoundEmitter.h"
#include "GCUIButton.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCUICheckbox> GCUICheckbox::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_UICHECKBOX);

    GCUICheckbox::GCUICheckbox()
        : tick_(0),
          checked_(false),
          sounds_(0)
    {
    }

    //--------------------------------------------------------------------------

    bool GCUICheckbox::Checked() const
    {
        return checked_;
    }

    void GCUICheckbox::Checked( bool val )
    {
        checked_ = val;
        tick_->Visible(checked_);
    }

    //--------------------------------------------------------------------------

    void GCUICheckbox::OnAwake()
    {
        Simian::Entity* entity = ParentEntity()->ChildByName("Tick");
        entity->ComponentByType(Simian::C_SPRITE, tick_);

        tick_->Visible(checked_);

        GCUIButton* button;
        ComponentByType(GC_UIBUTTON, button);
        button->AddListener(this);

        ComponentByType(Simian::C_SOUNDEMITTER, sounds_);
    }

    void GCUICheckbox::OnTriggered( Simian::DelegateParameter& )
    {
        checked_ = !checked_;
        tick_->Visible(checked_);
        if (sounds_)
            sounds_->Play(1);
    }

    void GCUICheckbox::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("Checked", checked_);
    }

    void GCUICheckbox::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("Checked", checked_, checked_);
    }
}
