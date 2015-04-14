/******************************************************************************/
/*!
\file		TileMap.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_TILEMAP_H_
#define DESCENSION_TILEMAP_H_

#include "Simian/SimianTypes.h"
#include "Simian/Utility.h" //bits operations
#include "Simian/DataLocation.h"

#include <vector>

namespace Descension
{
    /* In a Tile, there should be 4 walls and a floor.
    Their flags are accessed by comparing with TILE_FLAG */
    struct Tile
    {
    public:
        //bit flags for class file
        enum TILE_FLAG
        {
            TILE_FLOOR = 0,
            TILE_EAST,
            TILE_NORTH,
            TILE_WEST,
            TILE_SOUTH,
            TILE_DIAGUP,
            TILE_DIAGDOWN,
            TILE_OCCUPIED,

            TILE_FLAG_TOTAL
        };

    public:
        //Use TILE_FLAG to check bits
        Simian::u8 Flag;

    public:
        Tile(Simian::u8 flag = 0);
    };

    //--------------------------------------------------------------------------
    //The entire map of tiles for pathfinding/collision
    class TileMap
    {
    private:
        Simian::u32 width_; //dimensions
        Simian::u32 height_;
        std::vector<std::vector<Tile> > tileArray_;
    public:
        Simian::u32 Width() const;
        Simian::u32 Height() const;

        std::vector<Tile>& operator[](Simian::u32 index);
        const std::vector<Tile>& operator[](Simian::u32 index) const;
    public:
        //Default constructor
        TileMap();
        ~TileMap();

        void Create(Simian::u32 width, Simian::u32 height);
        bool Load(const Simian::DataLocation& data);
        void Save(const std::string& path);
        void Unload();
    };
}

#endif
