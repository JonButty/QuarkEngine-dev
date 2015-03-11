/*************************************************************************/
/*!
\file		GCCursor.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCCursor.h"
#include "ComponentTypes.h"
#include "GCEditor.h"
#include "GCMousePlaneIntersection.h"

#include "Simian\EnginePhases.h"
#include "Simian\EngineComponents.h"
#include "Simian\Vector3.h"
#include "Simian\Vector2.h"
#include "Simian\Mouse.h"
#include "Simian\Game.h"
#include "Simian\CTransform.h"
#include "Simian\OSW.h"
#include "Simian\CSprite.h"
#include "EntityTypes.h"
#include "Simian\Scene.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCCursor> GCCursor::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_CURSOR);
    GCCursor* GCCursor::instance_ = NULL;

    GCCursor::GCCursor()
        : transform_(0),
          spirit_(0),
          showBoth_(false)
    {
    }

    //--------------------------------------------------------------------------

    void GCCursor::update_( Simian::DelegateParameter& )
    {
#ifdef _DESCENDED
        if (GCEditor::Instance().EditorMode())
            return;
#endif
        if (!showBoth_)
        {
            if (spirit_ && spirit_->Active())
                sprite_->Visible(false);
            else
                sprite_->Visible(true);
        } else {
            sprite_->Visible(true);
        }

        Simian::Vector2 mouseVec = Simian::Mouse::GetMouseClientPosition();
        Simian::Vector2 winSize(Simian::Game::Instance().Size());
        mouseVec.X(mouseVec.X()-winSize.X()/2.0f);
        mouseVec.Y(-(mouseVec.Y()-winSize.Y()/2.0f));

        transform_->Position(Simian::Vector3(mouseVec.X()+offsetX_,
            mouseVec.Y()+offsetY_, transform_->Position().Z()));
    }

    GCCursor* GCCursor::Instance(void)
    {
        return instance_;
    }

    GCMousePlaneIntersection* GCCursor::Spirit(void) const
    {
        return spirit_;
    }
    void GCCursor::ShowBoth(bool showBoth)
    {
        showBoth_ = showBoth;
    }

    //--------------------------------------------------------------------------

    void GCCursor::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, 
            Simian::Delegate::CreateDelegate<GCCursor, &GCCursor::update_>(this));
    }

    void GCCursor::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(Simian::C_SPRITE, sprite_);

        instance_ = this;

        update_(Simian::Delegate::NoParameter);
        transform_->RecomputeWorld();

        // get spirit of demae
        Simian::Entity* entity = ParentScene()->EntityByTypeRecursive(E_SPIRITOFDEMAE);
        if (entity)
            entity->ComponentByType(GC_MOUSEPLANEINTERSECTION, spirit_);
    }

    void GCCursor::OnInit()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
        {
            Simian::OS::Instance().CursorSetVisibility(true);
            Simian::CSprite* sprite;
            ComponentByType(Simian::C_SPRITE, sprite);
            sprite->Visible(false);
            return;
        }
#endif
        Simian::OS::Instance().CursorSetVisibility(false);
    }

    void GCCursor::Serialize( Simian::FileObjectSubNode& data)
    {
        data.AddData("OffsetX", offsetX_);
        data.AddData("OffsetY", offsetY_);
    }

    void GCCursor::Deserialize( const Simian::FileObjectSubNode& data)
    {
        const Simian::FileDataSubNode* xNode = data.Data("OffsetX");
        offsetX_ = xNode ? xNode->AsF32() : 0.0f;

        const Simian::FileDataSubNode* yNode = data.Data("OffsetY");
        offsetY_ = yNode ? yNode->AsF32() : 0.0f;
    }
}
