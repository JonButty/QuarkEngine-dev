/******************************************************************************/
/*!
\file		GCExplosionLogic.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "GCExplosionLogic.h"

#include "ComponentTypes.h"

#include "Simian/EnginePhases.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"
#include "Simian/EntityComponent.h"
#include "Simian/Entity.h"
#include "Simian/Scene.h"
#include "Simian/LogManager.h"
#include "GCEditor.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCExplosionLogic> GCExplosionLogic::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_EXPLOSIONLOGIC);

    GCExplosionLogic::GCExplosionLogic()
        :   targetTrans_(0),
            timer_(0),
            maxTime_(0)
    {
    }

    void GCExplosionLogic::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCExplosionLogic, &GCExplosionLogic::update_>(this));
    }

    void GCExplosionLogic::OnAwake()
    {
    }

    void GCExplosionLogic::OnInit()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        ParentEntity()->ComponentByType(Simian::C_TRANSFORM,targetTrans_);
    }

    void GCExplosionLogic::Serialize( Simian::FileObjectSubNode& node )
    {
        node.AddData("MaxTime", maxTime_);
    }

    void GCExplosionLogic::Deserialize( const Simian::FileObjectSubNode& node )
    {
        const Simian::FileDataSubNode* data = node.Data("MaxTime");
        maxTime_ = data? data->AsF32() : maxTime_;
    }

    //-Public-------------------------------------------------------------------

    void GCExplosionLogic::MaxTime( Simian::f32 val )
    {
        maxTime_ = val;
    }

    //-Private------------------------------------------------------------------

    void GCExplosionLogic::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        param;
        if (GCEditor::EditorMode())
            return;
#endif
        timer_ += param.As<Simian::EntityUpdateParameter>().Dt;
        
        if(timer_ > maxTime_ && ParentEntity())
            ParentEntity()->ParentScene()->RemoveEntity(ParentEntity());
    }
}
