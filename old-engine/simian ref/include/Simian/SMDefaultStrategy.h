/************************************************************************/
/*!
\file		SMDefaultStrategy.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_SMDEFAULTSTRATEGY_H_
#define SIMIAN_SMDEFAULTSTRATEGY_H_

#include "SimianPlatform.h"
#include "SMStrategy.h"

namespace Simian
{
    class Camera;

    // simple hierarchical culling (good if gavin organises stuff)
    class SIMIAN_EXPORT SMDefaultStrategy: public SMStrategy
    {
    private:
        void cullEntity_(Camera* camera, Entity* entity);
    public:
        virtual void Cull();
    };
}

#endif
