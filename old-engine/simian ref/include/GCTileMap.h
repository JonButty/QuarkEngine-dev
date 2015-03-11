/*************************************************************************/
/*!
\file		GCTileMap.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCTILEMAP_H_
#define DESCENSION_GCTILEMAP_H_

#include "Simian/EntityComponent.h"
#include "Simian/ExplicitType.h"

namespace Descension
{
    class TileMap;

    class GCTileMap: public Simian::EntityComponent
    {
    private:
        std::string tileMapFile_;
        Simian::ExplicitType<TileMap*> tileMap_;
        bool loaded_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCTileMap> factoryPlant_;
    public:
        static GCTileMap* TileMap(Simian::Scene* scene);
        Descension::TileMap& TileMap() const;
        void TileMap(Descension::TileMap* tilemap);

        const std::string& TileMapFile() const;
    public:
        GCTileMap();
        virtual ~GCTileMap();

        void OnAwake();
        void OnDeinit();

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
