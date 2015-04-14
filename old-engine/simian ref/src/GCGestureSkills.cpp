/*************************************************************************/
/*!
\file		GCGestureSkills.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/30
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/CTransform.h"
#include "Simian/Scene.h"
#include "Simian/Entity.h"

#include "EntityTypes.h"
#include "ComponentTypes.h"
#include "Collision.h"

#include "GCGestureSkills.h"
#include "GCPhysicsController.h"
#include "GCAIBase.h"
#include "GCPlayerLogic.h"
#include "Gesture.h"
#include "GCGestureInterpreter.h"
#include "GCUI.h"
#include "GCEditor.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCGestureSkills> GCGestureSkills::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_GESTURESKILLS);
    
    GCGestureSkills::GCGestureSkills()
        : transform_(0),
          physics_(0),
          rotationSpeed_(0),
          angle_(0)
    {
    }

    //--------------------------------------------------------------------------

    Simian::CTransform& GCGestureSkills::Transform()
    {
        return *transform_;
    }

    //--------------------------------------------------------------------------

    void GCGestureSkills::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;

        angle_ += rotationSpeed_*dt;
        transform_->Rotation(Simian::Vector3(0, -1, 0), 
            Simian::Degree( angle_ ) );

        /* Touch and destroy code: uncomment if you need this
        if (physics_->Collided())
        {
            GCUI::Instance()->UnloadSkills();
            GCGestureInterpreter::Instance()->SetSkills(
                GestureSkills( GCGestureInterpreter::Instance()->GetSkills().Skills()
                | (1<<skillID_) ));
            GCUI::Instance()->LoadSkills(true, skillID_);
            physics_->Enabled(false);
            Simian::Entity* parentEnt = ParentEntity();
            ParentEntity()->ParentScene()->RemoveEntity(parentEnt);
        }
        */
    }

    //--------------------------------------------------------------------------

    void GCGestureSkills::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate
            <GCGestureSkills, &GCGestureSkills::update_>(this));
    }

    void GCGestureSkills::OnAwake()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(GC_PHYSICSCONTROLLER, physics_);
    }

    void GCGestureSkills::OnInit()
    {
        //--nothing
    }

    void GCGestureSkills::OnTriggered(Simian::DelegateParameter&)
    {
        //GCUI::Instance()->UnloadSkills();
        //GCGestureInterpreter::Instance()->SetSkills(
        //    GestureSkills( GCGestureInterpreter::Instance()->GetSkills().Skills()
        //    | (1<<skillID_) ));
        //GCUI::Instance()->LoadSkills(true, skillID_);
        //physics_->Enabled(false);
        //Simian::Entity* parentEnt = ParentEntity();
        //ParentEntity()->ParentScene()->RemoveEntity(parentEnt);
    }

    void GCGestureSkills::Serialize( Simian::FileObjectSubNode& data)
    {
        //data.AddData("SkillID", skillID_);
        data.AddData("RotationSpeed", rotationSpeed_);
    }

    void GCGestureSkills::Deserialize( const Simian::FileObjectSubNode& data)
    {
        //const Simian::FileDataSubNode* node = data.Data("SkillID");
        //if (node)
            //skillID_ = node->AsU32();
        const Simian::FileDataSubNode* rotSpeed = data.Data("RotationSpeed");
        if (rotSpeed)
            rotationSpeed_ = rotSpeed->AsF32();
    }
}

