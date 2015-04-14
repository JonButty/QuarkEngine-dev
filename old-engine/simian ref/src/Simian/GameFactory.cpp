/************************************************************************/
/*!
\file		GameFactory.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/GameFactory.h"
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/Scene.h"

#include "Simian/CModel.h"
#include "Simian/CCamera.h"
#include "Simian/CParticleSystem.h"
#include "Simian/CParticleRendererBillboard.h"
#include "Simian/CParticleRendererRibbonTrail.h"
#include "Simian/CParticleRendererModel.h"
#include "Simian/CBitmapText.h"
#include "Simian/CSprite.h"
#include "Simian/MaterialCache.h"
#include "Simian/CVideo.h"
#include "Simian/CUIResize.h"
#include "Simian/CBoneTrail.h"

namespace Simian
{
    GameFactory::GameFactory()
        : componentFactory_(C_TOTAL, true),
          totalPhases_(P_TOTAL),
          totalComponents_(C_TOTAL),
          totalEntities_(0)
    {
    }

    //--------------------------------------------------------------------------

    Factory<EntityComponent>& GameFactory::ComponentFactory()
    {
        return componentFactory_;
    }

    Simian::u32 GameFactory::TotalPhases() const
    {
        return totalPhases_;
    }

    Simian::u32 GameFactory::TotalComponents() const
    {
        return totalComponents_;
    }

    Simian::u32 GameFactory::TotalEntities() const
    {
        return totalEntities_;
    }

    const Simian::DefinitionTable& GameFactory::ComponentTable() const
    {
        return componentTable_;
    }

    const Simian::DefinitionTable& GameFactory::EntityTable() const
    {
        return entityTable_;
    }

    //--------------------------------------------------------------------------

    void GameFactory::Init( u32 totalPhases, u32 totalComponents, u32 totalEntities, const std::vector<DataLocation>& components, const std::vector<DataLocation>& entities )
    {
        totalPhases_ = totalPhases;
        totalComponents_ = totalComponents;
        totalEntities_ = totalEntities;

        componentTable_.Load(components);
        entityTable_.Load(entities);
    }

    void GameFactory::PrepareEngineComponents( GraphicsDevice* graphicsDevice, RenderQueue* renderQueue, MaterialCache* materialCache, ModelCache* modelCache )
    {
        Scene::TextureCache(materialCache->TextureCache());
        Scene::ShaderCache(materialCache->ShaderCache());
        Scene::ModelCache(modelCache);

        CModel::RenderQueue(renderQueue);
        CModel::ModelCache(modelCache);
        CModel::MaterialCache(materialCache);
        CCamera::Device(graphicsDevice);
        CCamera::RenderQueue(renderQueue);
        CParticleSystem::RenderQueue(renderQueue);
        CParticleRendererBillboard::MaterialCache(materialCache);
        CParticleRendererBillboard::GraphicsDevice(graphicsDevice);
        CParticleRendererRibbonTrail::MaterialCache(materialCache);
        CParticleRendererRibbonTrail::GraphicsDevice(graphicsDevice);
        CParticleRendererModel::GraphicsDevice(graphicsDevice);
        CParticleRendererModel::ModelCache(modelCache);
        CParticleRendererModel::MaterialCache(materialCache);
        CBitmapText::GraphicsDevice(graphicsDevice);
        CBitmapText::MaterialCache(materialCache);
        CBitmapText::RenderQueue(renderQueue);
        CSprite::GraphicsDevice(graphicsDevice);
        CSprite::MaterialCache(materialCache);
        CSprite::RenderQueue(renderQueue);
        CVideo::GraphicsDevice(graphicsDevice);
        CUIResize::Device(graphicsDevice);
        CBoneTrail::Device(graphicsDevice);
        CBoneTrail::MaterialCache(materialCache);
        CBoneTrail::RenderQueue(renderQueue);
    }

    bool GameFactory::CreateScene( Scene*& scene )
    {
        scene = new Scene();
        return scene ? true : false;
    }

    void GameFactory::DestroyScene( Scene*& scene )
    {
        delete scene;
        scene = 0;
    }
}
