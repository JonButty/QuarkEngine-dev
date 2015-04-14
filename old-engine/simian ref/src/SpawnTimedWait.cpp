/*************************************************************************/
/*!
\file		SpawnTimedWait.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "SpawnTimedWait.h"
#include "SpawnEventTypes.h"

namespace Descension
{
    SpawnFactory::Plant<SpawnTimedWait>::Type SpawnTimedWait::factoryPlant_(
        SpawnFactory::InstancePtr(), SE_TIMEDWAIT);

    SpawnTimedWait::SpawnTimedWait()
        : time_(0),
          timePassed_(0)
    {
    }
    
    //--------------------------------------------------------------------------

    void SpawnTimedWait::Reset( Simian::f32 time )
    {
        time_ = time;
        timePassed_ = 0.0f;
    }

    bool SpawnTimedWait::Execute( Simian::f32 dt )
    {
        timePassed_ += dt;
        return timePassed_ >= time_;
    }
}
