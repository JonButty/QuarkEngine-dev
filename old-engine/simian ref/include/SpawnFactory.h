/*************************************************************************/
/*!
\file		SpawnFactory.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_SPAWNFACTORY_H_
#define DESCENSION_SPAWNFACTORY_H_

#include "Simian/TemplateFactory.h"

namespace Descension
{
    class SpawnEvent;

    class SpawnFactory: public Simian::Factory<SpawnEvent>, public Simian::Singleton<SpawnFactory>
    {
    public:
        SpawnFactory();
    };
}

#endif
