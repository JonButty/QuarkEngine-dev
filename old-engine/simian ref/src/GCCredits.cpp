/*************************************************************************/
/*!
\file		GCCredits.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/10/12
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCCredits.h"
#include "GCUIButton.h"
#include "ComponentTypes.h"

#include "Simian\EnginePhases.h"
#include "Simian\EngineComponents.h"
#include "Simian\Scene.h"
#include "Simian\Vector3.h"
#include "Simian\Game.h"
#include "Simian\CTransform.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCCredits> GCCredits::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_CREDITSMENU);

    GCCredits::GCCredits()
        : transform_(0)
    {
    }

    //--------------------------------------------------------------------------

    void GCCredits::CloseCallback( const Simian::Delegate& val )
    {
        closeCallback_ = val;
    }

    //--------------------------------------------------------------------------

    void GCCredits::registerButton_( const std::string& name )
    {
        // find buttons and register them
        Simian::Entity* entity = ParentEntity()->ChildByName(name);
        GCUIButton* button;
        entity->ComponentByType(GC_UIBUTTON, button);
        button->AddListener(this);
    }

    //--------------------------------------------------------------------------

    void GCCredits::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);

        registerButton_("CancelButton");
    }

    void GCCredits::OnTriggered( Simian::DelegateParameter& param )
    {
        GCInputListener::Parameter* p;
        param.As(p);

        if (p->Callee->Name() == "CancelButton")
        {
            // remove!
            if (ParentEntity()->ParentEntity())
                ParentEntity()->ParentEntity()->RemoveChild(ParentEntity());
            else
                ParentScene()->RemoveEntity(ParentEntity());

            // do the callback
            closeCallback_();
        }
    }

    void GCCredits::Serialize( Simian::FileObjectSubNode& )
    {
    }

    void GCCredits::Deserialize( const Simian::FileObjectSubNode& )
    {
    }
}
