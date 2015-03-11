/*************************************************************************/
/*!
\file		GCTileMap.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCTileMap.h"
#include "TileMap.h"
#include "EntityTypes.h"
#include "ComponentTypes.h"

#include "Simian/LogManager.h"
#include "Simian/Scene.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCTileMap> GCTileMap::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_TILEMAP);

    GCTileMap::GCTileMap()
        : tileMap_(new Descension::TileMap()),
          loaded_(false)
    {
    }

    GCTileMap::~GCTileMap()
    {
        delete *tileMap_;
    }

    //--------------------------------------------------------------------------

    GCTileMap* GCTileMap::TileMap( Simian::Scene* scene )
    {
        Simian::Entity* entity = scene->EntityByType(E_TILEMAP);
        if (!entity)
            return NULL;
        GCTileMap* ret = NULL;
        entity->ComponentByType(GC_TILEMAP, ret);
        return ret;
    }

    TileMap& GCTileMap::TileMap() const
    {
        return *tileMap_;
    }

    void GCTileMap::TileMap( Descension::TileMap* tilemap )
    {
        **tileMap_ = *tilemap;
        loaded_ = true;
    }

    const std::string& GCTileMap::TileMapFile() const
    {
        return tileMapFile_;
    }

    //--------------------------------------------------------------------------

    void GCTileMap::OnAwake()
    {
        if (loaded_)
            return;
        loaded_ = true;
        
        if (tileMapFile_ != "")
            tileMap_->Load(tileMapFile_);
        else
        {
            SWarn("Tile map has no valid path.");
        }
    }

    void GCTileMap::OnDeinit()
    {
        tileMap_->Unload();
    }

    void GCTileMap::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("TileMapFile", tileMapFile_);
    }

    void GCTileMap::Deserialize( const Simian::FileObjectSubNode& data )
    {
        const Simian::FileDataSubNode* tileMapFile = data.Data("TileMapFile");
        tileMapFile_ = tileMapFile ? tileMapFile->AsString() : "";
    }
}
