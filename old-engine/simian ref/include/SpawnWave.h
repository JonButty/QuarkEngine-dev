/*************************************************************************/
/*!
\file		SpawnWave.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_SPAWNWAVE_H_
#define DESCENSION_SPAWNWAVE_H_

#include "SpawnEvent.h"

namespace Descension
{
    class SpawnWave: public SpawnEvent
    {
    public:
        struct SpawnPair
        {
            Simian::s32 EntityId;
            Simian::u32 SpawnAmount;
        };
        static const Simian::s32 SI_NOID = -1;

        static SpawnFactory::Plant<SpawnWave>::Type factoryPlant_;
    private:
        Simian::s32 spawnerId_;
        std::vector<SpawnPair> spawnPairs_;
    public:
        Simian::s32 SpawnerId() const;
        void SpawnerId(Simian::s32 val);
    public:
        SpawnWave();

        void AddSpawn(const std::string& entityName, Simian::u32 spawnAmount = 1);
        void AddSpawn(Simian::s32 entityId, Simian::u32 spawnAmount = 1);
        bool Execute(Simian::f32 dt);
    };
}

#endif
