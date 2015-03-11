/*************************************************************************/
/*!
\file		GCOBBTrigger.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCOBBTrigger.h"
#include "GCPlayerLogic.h"
#include "ComponentTypes.h"
#include "GCEditor.h"

#include "Simian/EngineComponents.h"
#include "Simian/CTransform.h"
#include "Simian/DebugRenderer.h"
#include "Simian/Color.h"
#include "Simian/EnginePhases.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCOBBTrigger> GCOBBTrigger::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_OBBTRIGGER);

    GCOBBTrigger::GCOBBTrigger()
        : repeat_(false),
          entered_(false),
          inside_(false)
    {
    }

    //--------------------------------------------------------------------------

    const Simian::Vector2& GCOBBTrigger::Size() const
    {
        return size_;
    }

    void GCOBBTrigger::Size( const Simian::Vector2& val )
    {
        size_ = val;
    }

    bool GCOBBTrigger::Repeat() const
    {
        return repeat_;
    }

    void GCOBBTrigger::Repeat( bool val )
    {
        repeat_ = val;
    }

    //--------------------------------------------------------------------------

    void GCOBBTrigger::update_( Simian::DelegateParameter& )
    {
        DebugRendererDrawCubeEx(transform_->World().Position(), Simian::Vector3(size_.X(), 1.0f, size_.Y()), 
            Simian::Color(0, 0, 1.0f, 1.0f), transform_->Angle(), Simian::Vector3::UnitY);

#ifdef _DESCENDED
        if (GCEditor::Instance().EditorMode())
            return;
#endif

        const Simian::Vector3& playerPos3 = GCPlayerLogic::Instance()->Transform().World().Position();
        Simian::Vector2 playerPosition = Simian::Vector2(playerPos3.X(), playerPos3.Z());

        Simian::Vector2 obbPos(transform_->World().Position().X(), transform_->World().Position().Z());
        Simian::Vector2 right(std::cos(-transform_->Angle().Radians()), std::sin(-transform_->Angle().Radians()));
        Simian::Vector2 up(-right.Y(), right.X());

        Simian::f32 hwidth = size_.X() * 0.5f;
        Simian::f32 hheight = size_.Y() * 0.5f;

        // create edges
        Simian::Vector2 corners[4] = {
            // top left
            obbPos + up * hheight - right * hwidth,
            // bottom left
            obbPos - up * hheight - right * hwidth,
            // bottom right
            obbPos - up * hheight + right * hwidth,
            // top right
            obbPos + up * hheight + right * hwidth
        };
        Simian::Vector2 normals[4] = {
            up,
            -right,
            -up,
            right
        };

        // test with each edge
        for (Simian::u32 i = 0; i < 4; ++i)
        {
            if ((playerPosition - corners[i]).Dot(normals[i]) > 0)
            {
                inside_ = false;
                return;
            }
        }

        // inside..
        if (!inside_ && (repeat_ || !entered_))
        {
            entered_ = true;
            inside_ = true;

            // call tals
            Trigger();
        }
    }

    //--------------------------------------------------------------------------

    void GCOBBTrigger::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, 
            Simian::Delegate::CreateDelegate<GCOBBTrigger, &GCOBBTrigger::update_>(this));
    }

    void GCOBBTrigger::OnAwake()
    {
        GCTrigger::OnAwake();
        ComponentByType(Simian::C_TRANSFORM, transform_);
    }

    void GCOBBTrigger::OnDeinit()
    {
        GCTrigger::OnDeinit();
    }

    void GCOBBTrigger::Reset()
    {
        entered_ = false;
    }

    void GCOBBTrigger::Deserialize( const Simian::FileObjectSubNode& data )
    {
        const Simian::FileObjectSubNode* sizeNode = data.Object("Size");
        if (sizeNode)
        {
            float x, y;
            sizeNode->Data("X", x, size_.X());
            sizeNode->Data("Y", y, size_.Y());
            size_ = Simian::Vector2(x, y);
        }

        data.Data("Repeat", repeat_, repeat_);

        GCTrigger::Deserialize(data);
    }

    void GCOBBTrigger::Serialize( Simian::FileObjectSubNode& data )
    {
        Simian::FileObjectSubNode* size = data.AddObject("Size");
        size->AddData("X", size_.X());
        size->AddData("Y", size_.Y());

        data.AddData("Repeat", repeat_);

        GCTrigger::Serialize(data);
    }
}
