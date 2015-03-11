/*************************************************************************/
/*!
\file		TriggerEvent.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_TRIGGEREVENT_H_
#define DESCENSION_TRIGGEREVENT_H_

#include "SpawnEvent.h"

namespace Simian
{
    class Entity;
}

namespace Descension
{
    class GCInputListener;

    class TriggerEvent: public SpawnEvent
    {
    private:
        Simian::Entity* callee_;
        GCInputListener* listener_;

        static SpawnFactory::Plant<TriggerEvent>::Type plant_;
    public:
        TriggerEvent();

        void ResolveTal(Simian::Entity* entity, const std::string& listener, const std::string& target = "");
        bool Execute( Simian::f32 dt );
    };
}

#endif
