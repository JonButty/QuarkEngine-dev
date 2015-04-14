/*************************************************************************/
/*!
\file		SpawnEvent.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_SPAWNEVENT_H_
#define DESCENSION_SPAWNEVENT_H_

#include "SpawnFactory.h"

namespace Descension
{
    class GCSpawnSequence;

    class SpawnEvent
    {
    protected:
        GCSpawnSequence* sequence_;
    public:
        GCSpawnSequence* Sequence() const;
        void Sequence(GCSpawnSequence* val);
    public:
        SpawnEvent(): sequence_(0) {}
        virtual ~SpawnEvent() {}

        virtual bool Execute(Simian::f32 dt) = 0;
    };
}

#endif
