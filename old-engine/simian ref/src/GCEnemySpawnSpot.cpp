/*************************************************************************/
/*!
\file		GCEnemySpawnSpot.cpp
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	2012/08/10
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
#include "Simian/Math.h"
#include "Simian/Entity.h"
#include "Simian/DebugRenderer.h"

#include "GCEnemySpawnSpot.h"
#include "GCAIBase.h"
#include "EntityTypes.h"
#include "ComponentTypes.h"
#include "GCEditor.h"
#include "GCAttributes.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCEnemySpawnSpot> GCEnemySpawnSpot::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_ENEMYSPAWNSPOT);

    GCEnemySpawnSpot::GCEnemySpawnSpot()
        : transform_(0),
          radius_(0)
    {
    }

    //--------------------------------------------------------------------------

    Simian::CTransform& GCEnemySpawnSpot::Transform()
    {
        return *transform_;
    }
    
    //--------------------------------------------------------------------------

#ifdef _DESCENDED
    void GCEnemySpawnSpot::update_(Simian::DelegateParameter&)
    {
        DebugRendererDrawCircle(transform_->World().Position(), radius_, Simian::Color(1.0f, 0.0f, 0.0f));
    }

    void GCEnemySpawnSpot::OnSharedLoad()
    {
        RegisterCallback(Simian::P_RENDER, 
            Simian::Delegate::CreateDelegate<GCEnemySpawnSpot, &GCEnemySpawnSpot::update_>(this));
    }
#endif

    //--------------------------------------------------------------------------

    void GCEnemySpawnSpot::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);
    }

    // spawn one enemy
    GCAttributes* GCEnemySpawnSpot::Spawn( Simian::s32 entityId )
    {
        // randomize a position in the radius
        Simian::f32 dir = Simian::Math::Random(0.0f, Simian::Math::PI);
        Simian::Vector3 offset(std::cos(dir), 0, std::sin(dir));
        Simian::Vector3 pos = transform_->World().Position() + radius_ * offset;

        // randomize a rotation
        Simian::f32 rot = transform_->Angle().Radians() + 
            Simian::Math::Random(-angularDifference_.Radians(), angularDifference_.Radians());
        Simian::Radian angle = Simian::Math::WrapAngle(Simian::Radian(rot));

        // spawn the entity
        Simian::Entity* entity = ParentScene()->CreateEntity(entityId);
        Simian::CTransform* transform;
        entity->ComponentByType(Simian::C_TRANSFORM, transform);

        // set the transform
        transform->Position(pos);
        transform->Rotation(Simian::Vector3::UnitY, angle);

        // add entity to scene
        ParentScene()->AddEntity(entity);

        // get attributes
        GCAttributes* attribs;
        entity->ComponentByType(GC_ATTRIBUTES, attribs);
        // set birthing to true
        attribs->StartFromBirth(true);
        attribs->ForceSeek(true);

        return attribs;
    }

    void GCEnemySpawnSpot::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("Radius", radius_);
        data.AddData("AngularDifference", angularDifference_.Degrees());
    }

    void GCEnemySpawnSpot::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("Radius", radius_, radius_);
        float angularDiff = 0;
        data.Data("AngularDifference", angularDiff, angularDifference_.Degrees());
        angularDifference_ = Simian::Degree(angularDiff);
    }
}
