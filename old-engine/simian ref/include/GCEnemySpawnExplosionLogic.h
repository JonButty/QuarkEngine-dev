/******************************************************************************/
/*!
\file		GCEnemySpawnExplosionLogic.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GCENEMYSPAWNEXPLOSIONLOGIC_H_
#define DESCENSION_GCENEMYSPAWNEXPLOSIONLOGIC_H_

#include "Simian/EntityComponent.h"

#include <vector>

namespace Descension
{
    class GCEnemySpawnExplosionLogic : public Simian::EntityComponent
    {
    private:
        enum SpawnStates
        {
            SS_INIT,
            SS_SPAWN,
            SS_DESTROY,
            SS_TOTAL
        };
    private:
        std::vector<std::pair<Simian::u32, Simian::u32> > enemyChances_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCEnemySpawnExplosionLogic> factoryPlant_;
    private:
        void update_(Simian::DelegateParameter& param);
        void spawn_();
    public:
        GCEnemySpawnExplosionLogic();
        void OnSharedLoad();
        void OnAwake();
        void OnInit();
        void Serialize(Simian::FileObjectSubNode& node);
        void Deserialize(const Simian::FileObjectSubNode& node);
    };
}

#endif
