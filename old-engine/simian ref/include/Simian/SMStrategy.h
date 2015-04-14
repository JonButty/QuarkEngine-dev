/************************************************************************/
/*!
\file		SMStrategy.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_SMSTRATEGY_H_
#define SIMIAN_SMSTRATEGY_H_

#include "SimianPlatform.h"

#include <vector>

namespace Simian
{
    class Entity;
    class Scene;

    class SIMIAN_EXPORT SMStrategy
    {
    private:
        Simian::Scene* scene_;
        std::vector<Simian::Entity*>* rootEntities_;
    private:
        void Scene(Simian::Scene* val);
        void RootEntities(std::vector<Simian::Entity*>& rootEntities);
    public:
        Simian::Scene* Scene() const;

        std::vector<Simian::Entity*>& RootEntities() const;
    public:
        SMStrategy();
        ~SMStrategy();

        virtual void Cull() = 0;

        friend class Scene;
    };
}

#endif
