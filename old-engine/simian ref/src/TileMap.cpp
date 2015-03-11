/******************************************************************************/
/*!
\file		TileMap.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "TileMap.h"
#include "Collision.h"
#include "PathFinder.h"

#include <sstream>
#include <fstream>
#include <string>

namespace Descension
{
    Tile::Tile(Simian::u8 flag) : Flag(flag)
    {
        //member init
    }

    //--------------------------------------------------------------------------
    //A map of an entire set of tiles
    TileMap::TileMap() 
        :   width_(0),
            height_(0)
    {
    }

    TileMap::~TileMap()
    {
    }

    std::vector<Tile>& TileMap::operator[]( Simian::u32 index )
    {
        return tileArray_.at(index);
    }

    const std::vector<Tile>& TileMap::operator[]( Simian::u32 index )const
    {
        return tileArray_.at(index);
    }

    Simian::u32 TileMap::Width() const
    {
        return width_;
    }

    Simian::u32 TileMap::Height() const
    {
        return height_;
    }

    void TileMap::Create(Simian::u32 width, Simian::u32 height)
    {
        Unload();

        width_ = width;
        height_ = height;

        tileArray_.resize(width);
        for (Simian::u32 i = 0; i < width; ++i)
            tileArray_[i].resize(height);
    }

    bool TileMap::Load( const Simian::DataLocation& data )
    {
        std::stringstream ss(std::string(data.Memory(),data.Memory() + data.Size()));

        ss >> width_;
        ss >> height_;

        if(!width_ || !height_)
        {
            Unload();
            return false;
        }
        tileArray_.resize(width_);

        for( Simian::u32 x = 0; x < width_ && !ss.eof(); ++x)
        {
            tileArray_[x].resize(height_);

            for(Simian::u32 y = 0; y < height_; ++y)
            {
                Simian::u32 num;
                ss >> num;
                tileArray_[x][y].Flag = static_cast<Simian::u8>(num);
            }
        }
        PathFinder::Instance().Map(*this);
        return true;
    }

    void TileMap::Save( const std::string& path )
    {
        std::ofstream file;
        file.open(path.c_str());

        file << width_ << " " << height_ << std::endl;
        for( Simian::u32 x = 0; x < width_; ++x)
        {
            for(Simian::u32 y = 0; y < height_; ++y)
                file << static_cast<Simian::s32>(tileArray_[x][y].Flag) << " ";
            file << std::endl;
        }
    }

    void TileMap::Unload()
    {
        for( size_t x = 0; x < width_; ++x)
            tileArray_[x].clear();
        tileArray_.clear();
    }
}
