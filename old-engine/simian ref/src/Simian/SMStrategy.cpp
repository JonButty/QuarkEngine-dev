/************************************************************************/
/*!
\file		SMStrategy.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/SMStrategy.h"

namespace Simian
{
    SMStrategy::SMStrategy()
        : scene_(0),
          rootEntities_(0)
    {
    }

    SMStrategy::~SMStrategy()
    {

    }

    //--------------------------------------------------------------------------

    void SMStrategy::Scene( Simian::Scene* val )
    {
        scene_ = val;
    }

    void SMStrategy::RootEntities( std::vector<Simian::Entity*>& rootEntities )
    {
        rootEntities_ = &rootEntities;
    }

    Simian::Scene* SMStrategy::Scene() const
    {
        return scene_;
    }

    std::vector<Simian::Entity*>& SMStrategy::RootEntities() const
    {
        return *rootEntities_;
    }

    //--------------------------------------------------------------------------

    void SMStrategy::Cull()
    {
    }
}
