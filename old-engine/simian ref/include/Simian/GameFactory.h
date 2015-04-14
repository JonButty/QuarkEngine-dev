/************************************************************************/
/*!
\file		GameFactory.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_GAMEFACTORY_H_
#define SIMIAN_GAMEFACTORY_H_

#include "SimianPlatform.h"
#include "Singleton.h"
#include "TemplateFactory.h"
#include "DefinitionTable.h"
#include "DataLocation.h"

namespace Simian
{
    class Scene;
    class EntityComponent;

    class GraphicsDevice;
    class RenderQueue;
    class ModelCache;
    class MaterialCache;

    class SIMIAN_EXPORT GameFactory: public Singleton<GameFactory>
    {
    private:
        Factory<EntityComponent> componentFactory_;
        u32 totalPhases_;
        u32 totalComponents_;
        u32 totalEntities_;

        DefinitionTable componentTable_;
        DefinitionTable entityTable_;
    public:
        Factory<EntityComponent>& ComponentFactory();
        Simian::u32 TotalPhases() const;
        Simian::u32 TotalComponents() const;
        Simian::u32 TotalEntities() const;
        const Simian::DefinitionTable& ComponentTable() const;
        const Simian::DefinitionTable& EntityTable() const;
    public:
        GameFactory();

        void Init(u32 totalPhases, u32 totalComponents, u32 totalEntities, const std::vector<DataLocation>& components, const std::vector<DataLocation>& entities);

        void PrepareEngineComponents(GraphicsDevice* graphicsDevice, RenderQueue* renderQueue, MaterialCache* materialCache, ModelCache* modelCache);

        bool CreateScene(Scene*& scene);
        void DestroyScene(Scene*& scene);
    };

    SIMIAN_IMPORT template class SIMIAN_EXPORT Singleton<GameFactory>;
}

#endif
