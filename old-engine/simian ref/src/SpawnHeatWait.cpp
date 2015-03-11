/*************************************************************************/
/*!
\file		SpawnHeatWait.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "SpawnHeatWait.h"
#include "GCSpawnSequence.h"
#include "SpawnEventTypes.h"

namespace Descension
{
    SpawnFactory::Plant<SpawnHeatWait>::Type SpawnHeatWait::factoryPlant_(
        SpawnFactory::InstancePtr(), SE_HEATWAIT);

    SpawnHeatWait::SpawnHeatWait()
        : heat_(0)
    {
    }

    //--------------------------------------------------------------------------

    Simian::u32 SpawnHeatWait::Heat() const
    {
        return heat_;
    }

    void SpawnHeatWait::Heat( Simian::u32 val )
    {
        heat_ = val;
    }

    bool SpawnHeatWait::Execute( Simian::f32 )
    {
        return sequence_->TotalHeat() <= heat_;
    }
}
