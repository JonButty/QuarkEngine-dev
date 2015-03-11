/******************************************************************************/
/*!
\file		GCCrystalSwap.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/12/07
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "GCCrystalSwap.h"
#include "ComponentTypes.h"
#include "GCEditor.h"
#include "EntityTypes.h"

#include "Simian/CTransform.h"
#include "Simian/CModel.h"
#include "Simian/EnginePhases.h"
#include "Simian/LogManager.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCCrystalSwap> GCCrystalSwap::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_CRYSTALSWAP);

    void GCCrystalSwap::update_( Simian::DelegateParameter& )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        if(trans_ && targetTrans_)
            trans_->WorldPosition(targetTrans_->WorldPosition());
    }

    //-Public-------------------------------------------------------------------

    GCCrystalSwap::GCCrystalSwap()
        :   targetName_(""),
            targetTrans_(0),
            trans_(0)
    {
    }

    GCCrystalSwap::~GCCrystalSwap()
    {
    }

    void GCCrystalSwap::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCCrystalSwap, &GCCrystalSwap::update_>(this));
    }

    void GCCrystalSwap::OnInit()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        ParentEntity()->ComponentByType(Simian::C_TRANSFORM, trans_);
        ParentEntity()->ParentEntity()->ChildByName(targetName_)->ComponentByType(Simian::C_TRANSFORM,targetTrans_);
        Simian::Entity* ent = ParentEntity()->ChildByType(E_FIRECRYSTAL_ATTACHMENT);
        if(ent)
        {
            ent->ComponentByType(Simian::C_MODEL, crystals[CT_FIRE]);
            //crystals[CT_FIRE]->Visible(false);
        }

        ent = ParentEntity()->ChildByType(E_LIGHTNINGCRYSTAL_ATTACHMENT);
        if(ent)
        {
            ent->ComponentByType(Simian::C_MODEL, crystals[CT_LIGHTNING]);
            //crystals[CT_LIGHTNING]->Visible(false);
        }
    }

    void GCCrystalSwap::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("TargetName",targetName_);
    }

    void GCCrystalSwap::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("TargetName",targetName_, targetName_);
    }

    void GCCrystalSwap::SetActiveCrystal( unsigned int type )
    {
        for(Simian::u32 i = 0; i < CT_TOTAL; ++i)
        {
            if(i == type)
                crystals[i]->Visible(true);
            else
                crystals[i]->Visible(false);
        }
    }
}
