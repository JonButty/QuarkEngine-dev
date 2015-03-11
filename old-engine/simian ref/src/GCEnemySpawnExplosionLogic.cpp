/******************************************************************************/
/*!
\file		GCEnemySpawnExplosionLogic.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "Simian\Math.h"
#include "Simian\EntityComponent.h"
#include "Simian\Scene.h"
#include "Simian\CTransform.h"
#include "Simian\EnginePhases.h"

#include "GCEnemySpawnExplosionLogic.h"
#include "GCAttributes.h"

#include "ComponentTypes.h"

#include <string>

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCEnemySpawnExplosionLogic> GCEnemySpawnExplosionLogic::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_ENEMYSPAWNEXPLOSIONLOGIC);

    GCEnemySpawnExplosionLogic::GCEnemySpawnExplosionLogic()
    {

    }

    void GCEnemySpawnExplosionLogic::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCEnemySpawnExplosionLogic, &GCEnemySpawnExplosionLogic::update_>(this));
    }

    void GCEnemySpawnExplosionLogic::OnAwake()
    {
    }

    void GCEnemySpawnExplosionLogic::OnInit()
    {

    }

    void GCEnemySpawnExplosionLogic::Serialize( Simian::FileObjectSubNode&  )
    {
        //IMPLEMENT
    }

    void GCEnemySpawnExplosionLogic::Deserialize( const Simian::FileObjectSubNode& node )
    {
        const Simian::FileArraySubNode* fileArray = node.Array("EnemyChances");
        
        for(Simian::u32 i = 0; i < fileArray->Size(); ++i)
        {
            const Simian::FileObjectSubNode* enemyChanceNode = fileArray->Object(i);
            const Simian::FileDataSubNode* typeNode = enemyChanceNode->Data("Type");

            std::string name = typeNode->AsString();
            if(name.size() < 0)
                break;

            Simian::u32 enemyType = Simian::GameFactory::Instance().EntityTable().Value(name);
           
            const Simian::FileDataSubNode* chanceNode = enemyChanceNode->Data("Chance");
            Simian::u32 enemyChance = chanceNode->AsU32();

            enemyChances_.push_back(std::pair<Simian::u32,Simian::u32>(enemyType,enemyChance));
        }
    }

    //-Private------------------------------------------------------------------

    void GCEnemySpawnExplosionLogic::update_( Simian::DelegateParameter&  )
    {
        spawn_();
    }

    void GCEnemySpawnExplosionLogic::spawn_()
    {
        Simian::s32 chance = Simian::Math::Random(0,100);
        Simian::u32 enemyType = 0;

        for(Simian::u32 i = 0; i < enemyChances_.size(); ++i)
        {
            std::pair<Simian::u32,Simian::u32> enemy(enemyChances_[i]);
            chance -= enemy.second;
            if(chance <= 0)
            {    
                enemyType = enemy.first;
                break;
            }
        }

        Simian::Entity* entity = ParentEntity()->ParentScene()->CreateEntity(enemyType);

        GCAttributes* attrib;
        Simian::CTransform* trans;
        Simian::CTransform* ownerTrans;
        ParentEntity()->ComponentByType(Simian::C_TRANSFORM, ownerTrans);
        entity->ComponentByType(Simian::C_TRANSFORM, trans);
        entity->ComponentByType(GC_ATTRIBUTES, attrib);
        
        trans->Position(ownerTrans->World().Position()); 
        attrib->BirthTimer(0.0f);
        float angle = ownerTrans->Angle().Degrees();
        trans->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(angle + 90.0f));
        ParentEntity()->ParentScene()->AddEntity(entity);
        ParentScene()->RemoveEntity(ParentEntity());
    }
}
