/*************************************************************************/
/*!
\file		SpawnTimedWait.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_SPAWNTIMEDWAIT_H_
#define DESCENSION_SPAWNTIMEDWAIT_H_

#include "SpawnEvent.h"

namespace Descension
{
    class SpawnTimedWait: public SpawnEvent
    {
    private:
        Simian::f32 time_;
        Simian::f32 timePassed_;
        static SpawnFactory::Plant<SpawnTimedWait>::Type factoryPlant_;
    public:
        SpawnTimedWait();

        void Reset(Simian::f32 time);
        bool Execute(Simian::f32 dt);
    };
}

#endif
