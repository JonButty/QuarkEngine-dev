/*************************************************************************/
/*!
\file		GCDestructiveUI.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCDestructiveUI.h"
#include "GCUIButton.h"
#include "ComponentTypes.h"

#include "Simian\EnginePhases.h"
#include "Simian\Scene.h"
#include "Simian\Game.h"
#include "Simian\CTransform.h"
#include "Simian\CBitmapText.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCDestructiveUI> GCDestructiveUI::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_DESTRUCTIVEUI);

    GCDestructiveUI::GCDestructiveUI()
        : transform_(0),
          bitmapText_(0)
    {
    }

    //--------------------------------------------------------------------------

    Simian::DelegateList& GCDestructiveUI::YesCallbacks()
    {
        return yesCallbacks_;
    }

    Simian::DelegateList& GCDestructiveUI::NoCallbacks()
    {
        return noCallbacks_;
    }

    void GCDestructiveUI::Text( const std::string& text )
    {
        text_ = text;
    }

    //--------------------------------------------------------------------------

    void GCDestructiveUI::update_( Simian::DelegateParameter& )
    {
        // make sure the height is a scale of the game's height
        Simian::f32 height = Simian::Game::Instance().Size().Y() * 0.5f;

        // scale the transform
        transform_->Scale(Simian::Vector3(height/512.0f, height/512.0f, 1.0f));

        // set dialog text
        bitmapText_->Text(text_);
    }

    void GCDestructiveUI::registerButton_( const std::string& name )
    {
        GCUIButton* button;
        Simian::Entity* entity = ParentEntity()->ChildByName(name);
        entity->ComponentByType(GC_UIBUTTON, button);
        button->AddListener(this);
    }

    void GCDestructiveUI::remove_()
    {
        if (ParentEntity()->ParentEntity())
            ParentEntity()->ParentEntity()->RemoveChild(ParentEntity());
        else
            ParentScene()->RemoveEntity(ParentEntity());
    }

    //--------------------------------------------------------------------------

    void GCDestructiveUI::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, 
            Simian::Delegate::CreateDelegate<GCDestructiveUI, &GCDestructiveUI::update_>(this));
    }

    void GCDestructiveUI::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);

        Simian::Entity* entity = ParentEntity()->ChildByName("Text");
        entity->ComponentByType(Simian::C_BITMAPTEXT, bitmapText_);

        if (text_ == "")
            text_ = bitmapText_->Text();

        registerButton_("YesButton");
        registerButton_("NoButton");

        update_(Simian::Delegate::NoParameter);
        transform_->RecomputeWorld();
    }

    void GCDestructiveUI::OnTriggered( Simian::DelegateParameter& param)
    {
        GCInputListener::Parameter* p;
        param.As(p);

        if (p->Callee->Name() == "YesButton")
        {
            yesCallbacks_();
            remove_();
        }
        else if (p->Callee->Name() == "NoButton")
        {
            noCallbacks_();
            remove_();
        }
    }
}
