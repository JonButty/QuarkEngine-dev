/*************************************************************************/
/*!
\file		GCInputListener.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCINPUTLISTENER_H_
#define DESCENSION_GCINPUTLISTENER_H_

#include "Simian/EntityComponent.h"

namespace Descension
{
    class GCInputListener: public Simian::EntityComponent
    {
    public:
        struct Parameter: public Simian::DelegateParameter
        {
            Simian::Entity* Callee;
            void* Data;
        };
    public:
        virtual void OnTriggered(Simian::DelegateParameter&) {}
    };
}

#endif
