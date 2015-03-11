/******************************************************************************/
/*!
\file		Pathfinder.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GCPATHFINDER_H_
#define DESCENSION_GCPATHFINDER_H_

#include "Simian/EntityComponent.h"
#include "Simian/SimianTypes.h"
#include "Simian/Vector3.h"
#include "Simian/Vector2.h"
#include "Simian/Singleton.h"
#include "Simian/Color.h"

#include <map>
#include <vector>

#include "PathNode.h"
#include "TileMap.h"

namespace Descension
{
    class GCAIBase;

    struct SortByFScore
    {
        bool operator()(const PathNode& lhs, const PathNode& rhs) const;
    };

    struct SortByCoord
    {
        bool operator()(const PathNode& lhs, const PathNode& rhs) const;
    };

    class PathFinder
        :   public Simian::Singleton<PathFinder>
    {
    private:
        bool targetOnDiagTile_;
        bool continuePathfinding_;
        PathNode start_, target_, initialMin_, initialMax_;
        std::multimap<Simian::u32,PathNode> openListFScore_;
        std::multimap<PathNode,Simian::u32,SortByCoord> openListCoord_;
        std::multimap<PathNode,Simian::u32,SortByCoord> closeList_;
        std::vector<std::vector<Tile> > subtiles_;
        Simian::Vector2 targetWorld_;
        Simian::f32 radiusSqr_;
        Simian::u32 width_;
        Simian::u32 height_;
        Simian::f32 occupiedScore_;
        Simian::f32 maxRange_;
    private:
        const PathNode* constructPath_ (const PathNode& node, Simian::f32 radius);
        bool inOpenList_ (const PathNode& node);
        bool inCloseList_ (const PathNode& node);
        std::multimap<PathNode,Simian::u32,SortByCoord>::iterator getBestNode_ ();
        void analyzeNode_ ( Simian::f32 radius, PathNode node, const PathNode& parent);
        void analyzeNeighbor_ (Simian::f32 radius, const PathNode& node);
        Simian::u32 calcFScore_ (Tile& tile, Simian::u32 gScore, Simian::f32 hScore);
        void addValidTile_(PathNode& node, Simian::f32 radius);
        void generateSubtiles_(TileMap& val);
        Tile::TILE_FLAG determineDir_( const PathNode& prev, const PathNode& next );
    public:
        void Map( TileMap& val);

        Simian::u32 Width() const;
        void Width(Simian::u32 val);
        Simian::u32 Height() const;
        void Height(Simian::u32 val);
        bool ContinuePathfinding() const;
        void ContinuePathfinding(bool val);
    public:
        PathFinder();
        void DrawSubtile(Simian::u32 x, Simian::u32 y, Simian::Color color);
        Simian::Vector3 GeneratePath (Simian::f32 radius, Simian::f32 rangeSqr, const Simian::Vector3& start,
                                    const Simian::Vector3& target);
        void SetNewPosition(const Simian::Vector3& position, Simian::f32 radius, Simian::Vector2& min, Simian::Vector2& max );
        void UnsetPosition(Simian::Vector2& min, Simian::Vector2& max );
        Simian::Vector2 WorldToSubtile(const Simian::Vector2& world);
        PathNode WorldToSubtile(const PathNode& world);
        Simian::f32 WorldToSubtile( Simian::f32 radius );
        Simian::Vector2 SubtileToWorld(const Simian::Vector2& subtile);
        Simian::f32 SubtileToWorld( Simian::f32 radius );
        bool IsSubTileValid(Simian::u32 x, Simian::u32 y);
        bool IsNextSubTileValid(Simian::u32 dir, Simian::u32 x, Simian::u32 y);
        bool CalcMinMax (Simian:: f32 radius, Simian::u32 x, Simian::u32 y,
            Simian::u32& minX, Simian::u32& minY, 
            Simian::u32& maxX, Simian::u32& maxY );

        void SetTileEdge(bool state, Simian::u32 x, Simian::u32 y, Tile::TILE_FLAG direction);
    };
}

#endif
