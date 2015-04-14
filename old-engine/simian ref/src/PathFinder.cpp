/******************************************************************************/
/*!
\file		Pathfinder.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "Pathfinder.h"
#include "GCAIBase.h"

#include "ComponentTypes.h"

#include "Simian/LogManager.h"
#include "Simian/Math.h"
#include "Simian/DebugRenderer.h"
#include "GCPlayerLogic.h"

#define DEBUG 1
namespace Descension
{
    static const Simian::u32 GSCORE = 20;
    static const Simian::u32 SUBTILES_PER_TILE = 5;
    static const Simian::f32 SUBTILE_SIZE = 1.0f/SUBTILES_PER_TILE;
    static const Simian::u32 MAX_ANALIZED_NODES = 50;

    static const Simian::u32 NON_VALID_TILE = 10000;
    static const Simian::u8 X_WALLS = (1 << Tile::TILE_EAST) | (1 << Tile::TILE_WEST);
    static const Simian::u8 Y_WALLS = (1 << Tile::TILE_NORTH) | (1 << Tile::TILE_SOUTH);

    bool SortByFScore::operator()( const PathNode& lhs, const PathNode& rhs ) const
    {
        return lhs.FScore() < rhs.FScore();
    }

    //--------------------------------------------------------------------------

    bool SortByCoord::operator()( const PathNode& lhs, const PathNode& rhs ) const
    {
        return  (lhs.X() < rhs.X())? true :
                (lhs.X() > rhs.X())? false:
                (lhs.Y() < rhs.Y())? true :false;
    }
    //--------------------------------------------------------------------------

    PathFinder::PathFinder()
        : continuePathfinding_(false)
    {
    }

    void PathFinder::Map( TileMap& val )
    {
        generateSubtiles_(val);
    }

    Simian::u32 PathFinder::Width() const
    {
        return width_;
    }

    void PathFinder::Width( Simian::u32 val )
    {
        width_ = val;
    }

    Simian::u32 PathFinder::Height() const
    {
        return height_;
    }

    void PathFinder::Height( Simian::u32 val )
    {
        height_ = val;
    }

    bool PathFinder::ContinuePathfinding() const
    {
        return continuePathfinding_;
    }

    void PathFinder::ContinuePathfinding( bool val )
    {
        continuePathfinding_ = val;
    }

    void PathFinder::DrawSubtile(Simian::u32 x, Simian::u32 y, Simian::Color color)
    {
        color;
#if DEBUG
        Simian::f32 xWorld = x * SUBTILE_SIZE;
        Simian::f32 yWorld = y * SUBTILE_SIZE;
        xWorld += 0.5f * SUBTILE_SIZE;
        yWorld += 0.5f * SUBTILE_SIZE;
        DebugRendererDrawCube(Simian::Vector3(xWorld, 0, yWorld),
                Simian::Vector3(SUBTILE_SIZE, 0.4f, SUBTILE_SIZE),
                color);
#else
        x;y;
#endif
    }

    Simian::Vector3 PathFinder::GeneratePath( Simian::f32 radius, Simian::f32 rangeSqr, const Simian::Vector3& start,
                                              const Simian::Vector3& target )
    {
        if (GCPlayerLogic::Instance()->PlayerControlOverride() && !continuePathfinding_)
            return start;

        radiusSqr_ = radius * radius;
        targetWorld_.X(target.X());
        targetWorld_.Y(target.Z());

        maxRange_ = WorldToSubtile(2 * rangeSqr);
        Simian::Vector2 vecToTarget((target.X() - start.X()),target.Z() - start.Z());
        if(vecToTarget.LengthSquared() < Simian::Math::EPSILON)
            return start;
        vecToTarget = WorldToSubtile(vecToTarget);

        //Simian::f32 combinedRadius = WorldToSubtile(radius + targetRadius);

        occupiedScore_ = (250.0f * std::floor(radius*static_cast<Simian::f32>(SUBTILES_PER_TILE))) + GSCORE;
        Simian::Vector2 subtileStart = WorldToSubtile(Simian::Vector2(start.X(), start.Z()));
        Simian::Vector2 subtileTarget = WorldToSubtile(Simian::Vector2(target.X(), target.Z()));

        start_.X(static_cast<Simian::u32>(subtileStart.X()));
        start_.Y(static_cast<Simian::u32>(subtileStart.Y()));

        target_.X(static_cast<Simian::u32>(subtileTarget.X()));
        target_.Y(static_cast<Simian::u32>(subtileTarget.Y()));

        Simian::u32 x = target_.X(), y = target_.Y();
        if(!SIsFlagSet(subtiles_[x][y].Flag,Tile::TILE_FLOOR))
            return start;
        if ((start_.X() >= width_) || (start_.Y() >= height_) &&
            (target_.X() >= width_) || (target_.Y() >= height_))
            return start;

        Simian::u32 minX = 0, minY = 0, maxX = 0, maxY = 0; 
        CalcMinMax(radius,start_.X(), start_.Y(),minX, minY, maxX, maxY);
        initialMin_.X(minX);
        initialMin_.Y(minY);
        initialMax_.X(maxX);
        initialMax_.Y(maxY);

        //Simian::f32 newTargetRadius = std::sqrt(2* std::pow(combinedRadius,2));
        //vecToTarget.Normalize();
        //target_.X(static_cast<Simian::u32>(target_.X() + 0.5f - (vecToTarget.X() * newTargetRadius)));
        //target_.Y(static_cast<Simian::u32>(target_.Y() + 0.5f - (vecToTarget.Y() * newTargetRadius)));    
        //DrawSubtile(target_.X(),target_.Y(),Simian::Color(1.0f,0.0f,0.0f));

        openListFScore_.clear();
        openListCoord_.clear();
        closeList_.clear();

        openListFScore_.insert(std::pair<Simian::u32,PathNode>(0,start_));
        openListCoord_.insert(std::pair<PathNode,Simian::u32>(start_,0));

        std::multimap<PathNode,Simian::u32,SortByCoord>::iterator it;
        size_t size = openListFScore_.size();
        while(size > 0 && size < MAX_ANALIZED_NODES)
        {
            it = getBestNode_();
            if ((it->first.X() == target_.X()) && (it->first.Y() == target_.Y()))
                break;

            analyzeNeighbor_(radius, it->first);
            size = openListFScore_.size();
        }
        
        const PathNode* final = constructPath_(it->first, radius);
        Simian::f32 hsize = SUBTILE_SIZE * 0.5f;
        Simian::Vector2 halfTile(hsize,hsize);
        Simian::Vector2 world = SubtileToWorld(Simian::Vector2(
            static_cast<Simian::f32>(final->X()),
            static_cast<Simian::f32>(final->Y()))) + halfTile;
       
        return Simian::Vector3(world.X(),0,world.Y());
    }

    //-Private------------------------------------------------------------------

    const PathNode* PathFinder::constructPath_( const PathNode& node, Simian::f32 radius )
    {
        const PathNode* prev = &node;
        const PathNode* next = node.Parent();

        Simian::u32 x = start_.X(), y = start_.Y();
        Simian::u32 minX = 0, maxX = 0, minY = 0, maxY = 0;
        CalcMinMax(radius,x,y,minX,minY,maxX,maxY);

        if(!next)
            return &start_;
        if((prev->X() == start_.X()) && (prev->Y() == start_.Y()))
            return &start_;
        
        Tile* subtile;
        
        while(next && (next->X() != start_.X() || next->Y() != start_.Y()))
        {
            //if(SIsFlagSet(subtiles_[next->X()][next->Y()].Flag, Tile::TILE_DIAGDOWN) || 
            //    SIsFlagSet(subtiles_[next->X()][next->Y()].Flag, Tile::TILE_DIAGUP))
            //    return &start_;

            DrawSubtile(next->X(),next->Y(),Simian::Color(1.0f,0.0f,0.0f));
            
            prev = next;
            next = next->Parent();
        }

        x = prev->X();
        y = prev->Y();

        CalcMinMax(radius,x,y,minX,minY,maxX,maxY);

        if(minX + 1 < width_)
        {
            subtile = &subtiles_[minX + 1][minY];
            if (SIsFlagSet(subtile->Flag, Tile::TILE_WEST))
                return prev;
            
            if( maxY < height_ )
            {
                subtile = &subtiles_[minX + 1][maxY];
                if (SIsFlagSet(subtile->Flag, Tile::TILE_WEST))
                    return prev;
            }
        }
        if(minY + 1 < height_)
        {
            subtile = &subtiles_[minX][minY + 1];
            if (SIsFlagSet(subtile->Flag, Tile::TILE_SOUTH))
                return prev;

            if( maxX < width_ )
            {
                subtile = &subtiles_[maxX][minY + 1];
                if (SIsFlagSet(subtile->Flag, Tile::TILE_SOUTH))
                    return prev;
            }
        }

        if(maxX)
        {
            subtile = &subtiles_[maxX - 1][minY];
            if (SIsFlagSet(subtile->Flag, Tile::TILE_EAST))
                return prev;

            if( maxY < height_ )
            {
                subtile = &subtiles_[maxX - 1][maxY];
                if (SIsFlagSet(subtile->Flag, Tile::TILE_EAST))
                    return prev;
            }
        }

        if(maxY)
        {        
            subtile = &subtiles_[minX][maxY - 1];
            if (SIsFlagSet(subtile->Flag, Tile::TILE_NORTH))
                return prev;

            if( maxX < width_ )
            {
                subtile = &subtiles_[maxX][maxY - 1];
                if (SIsFlagSet(subtile->Flag, Tile::TILE_NORTH))
                    return prev;
            }
        }

        if(prev->X() != start_.X())
        {
            Simian::u32 xLimit = 0;

            // East
            if(prev->X() > start_.X() && maxX < width_)
                xLimit = maxX;

            // West
            else
                xLimit = minX;

            if( maxY > height_ )
                return &start_;
            for(Simian::u32 y = minY; y <= maxY; ++y)
            {
                Tile& tile = subtiles_[xLimit][y];
                if (SIsFlagSet(tile.Flag, Tile::TILE_OCCUPIED))
                    return &start_;
            }
        }
        else
        {
            Simian::u32 yLimit = 0;

            // North
            if(prev->Y() > start_.Y() && maxY < height_)
                yLimit = maxY;

            // South
            else
                yLimit = minY;
            
            if(maxX > width_)
                return &start_;
            for(Simian::u32 x = minX; x <= maxX; ++x)
            {
                Tile& tile = subtiles_[x][yLimit];
                if (SIsFlagSet(tile.Flag, Tile::TILE_OCCUPIED))
                    return &start_;
            }
        }
        return prev;
    }

    bool PathFinder::inOpenList_( const PathNode& node )
    {
        return (openListCoord_.find(node) == openListCoord_.end())?false: true;
    }

    bool PathFinder::inCloseList_( const PathNode& node )
    {
        return (closeList_.find(node) == closeList_.end())? false : true;
    }

    std::multimap<PathNode,Simian::u32,SortByCoord>::iterator PathFinder::getBestNode_()
    {
        std::multimap<Simian::u32,PathNode>::iterator oList = openListFScore_.begin();
        std::multimap<PathNode,Simian::u32,SortByCoord>::iterator cList = 
            closeList_.insert(std::pair<PathNode,Simian::u32>(oList->second, oList->second.FScore()));
        return cList;
    }

    void PathFinder::analyzeNode_( Simian::f32 radius, PathNode node, const PathNode& parent )
    {
        Simian::u32 x = node.X();
        Simian::u32 y = node.Y();
        Simian::u32 flag = subtiles_[node.X()][node.Y()].Flag;

        if ((x + radius >= width_) || (y + radius >= height_) || 
            (x - radius < Simian::Math::EPSILON) ||(y - radius < Simian::Math::EPSILON))
            return;

        if (!SIsFlagSet(flag, Tile::TILE_FLOOR))
            return;
        
        Simian::u32 fScore = 0;
        Simian::Vector2 world = SubtileToWorld(Simian::Vector2(static_cast<Simian::f32>(x),static_cast<Simian::f32>(y)));
        Simian::f32 length = (world - targetWorld_).LengthSquared();
        if(length > radiusSqr_)
        {
            if(x > 0)
            {
                Simian::u32 left = x - 1;
                if(SIsFlagSet(subtiles_[left][y].Flag,Tile::TILE_DIAGUP) ||
                    SIsFlagSet(subtiles_[left][y].Flag,Tile::TILE_DIAGDOWN))
                    return;
            }
            if(x < width_ - 1)
            {
                Simian::u32 right = x + 1;
                if(SIsFlagSet(subtiles_[right][y].Flag,Tile::TILE_DIAGUP) ||
                    SIsFlagSet(subtiles_[right][y].Flag,Tile::TILE_DIAGDOWN))
                    return;
            }
            if(y > 0)
            {
                Simian::u32 down = y - 1;
                if(SIsFlagSet(subtiles_[x][down].Flag,Tile::TILE_DIAGUP) ||
                    SIsFlagSet(subtiles_[x][down].Flag,Tile::TILE_DIAGDOWN))
                    return;
            }
            if(y < height_ + 1)
            {
                Simian::u32 up = y - 1;
                if(SIsFlagSet(subtiles_[x][up].Flag,Tile::TILE_DIAGUP) ||
                    SIsFlagSet(subtiles_[x][up].Flag,Tile::TILE_DIAGDOWN))
                    return;
            }
        }

        if(inCloseList_(node) || inOpenList_(node))
            return;

        Simian::u32 parentGScore = parent.GScore();

        node.Parent(parent);
        //node.GScore(SIsFlagSet(flag,Tile::TILE_OCCUPIED)? 
        //    parentGScore + static_cast<Simian::u32>(occupiedScore_): 0);
        

        Simian::u32 minX = 0, maxX = 0, minY = 0, maxY = 0;
        if(!CalcMinMax(radius,x,y,minX,minY,maxX,maxY))
            return;
        Tile* subtile;
        if(minX + 1 < width_)
        {
            subtile = &subtiles_[minX + 1][minY];
            if (SIsFlagSet(subtile->Flag, Tile::TILE_WEST))
                fScore += NON_VALID_TILE;

            if( maxY < height_ )
            {
                subtile = &subtiles_[minX + 1][maxY];
                if (SIsFlagSet(subtile->Flag, Tile::TILE_WEST))
                    fScore += NON_VALID_TILE;
            }
        }
        if(minY + 1 < height_)
        {
            subtile = &subtiles_[minX][minY + 1];
            if (SIsFlagSet(subtile->Flag, Tile::TILE_SOUTH))
                fScore += NON_VALID_TILE;

            if( maxX < width_ )
            {
                subtile = &subtiles_[maxX][minY + 1];
                if (SIsFlagSet(subtile->Flag, Tile::TILE_SOUTH))
                    fScore += NON_VALID_TILE;
            }
        }

        if(maxX)
        {
            subtile = &subtiles_[maxX - 1][minY];
            if (SIsFlagSet(subtile->Flag, Tile::TILE_EAST))
                fScore += NON_VALID_TILE;

            if( maxY < height_ )
            {
                subtile = &subtiles_[maxX - 1][maxY];
                if (SIsFlagSet(subtile->Flag, Tile::TILE_EAST))
                    fScore += NON_VALID_TILE;
            }
        }

        if(maxY)
        {        
            subtile = &subtiles_[minX][maxY - 1];
            if (SIsFlagSet(subtile->Flag, Tile::TILE_NORTH))
                fScore += NON_VALID_TILE;

            if( maxX < width_ )
            {
                subtile = &subtiles_[maxX][maxY - 1];
                if (SIsFlagSet(subtile->Flag, Tile::TILE_NORTH))
                    fScore += NON_VALID_TILE;
            }
        }

        Simian::f32 targetX = static_cast<Simian::f32>(target_.X());
        Simian::f32 targetY = static_cast<Simian::f32>(target_.Y());
        Simian::f32 startX = static_cast<Simian::f32>(start_.X());
        Simian::f32 startY = static_cast<Simian::f32>(start_.Y());

        // South or North
        if(node.Y() != parent.Y())
        {
            Simian::f32 dYSqr = 0.0f;
            // Scan new boundaries 
            for(Simian::u32 y = minY; y <= maxY; ++y)
            {
                if( y >= height_ )
                    return;
                // If Y is in initial min/max
                bool inInitial = (y >= initialMin_.Y()) && (y <= initialMax_.Y());
                dYSqr = static_cast<Simian::f32>((targetY - y ) * (targetY - y ));
                
                Simian::f32 range = (startY - y) * (startY - y);
                if( range > maxRange_)
                    fScore += NON_VALID_TILE;
                for(Simian::u32 x = minX; x <= maxX; ++x)
                {
                    if( x >= width_ )
                        return;
                    // If X is in initial min/max
                    if(inInitial && (x >= initialMin_.X()) && (x <= initialMax_.X()))
                        continue;
                    Tile& tile = subtiles_[x][y];
                    if (!SIsFlagSet(tile.Flag, Tile::TILE_FLOOR))
                        return;

                    Simian::f32 dX = targetX - x;
                    fScore += calcFScore_(tile,parentGScore,dX * dX + dYSqr);
                }
            }
        }
        // East or West
        else
        {
            Simian::f32 dXSqr = 0;
            // Scan new boundaries 
            for(Simian::u32 x = minX; x <= maxX; ++x)
            {
                if( x >= width_ )
                    return;
                // If X is in initial min/max
                bool inInitial = (x >= initialMin_.X()) && (x <= initialMax_.X());
                dXSqr = static_cast<Simian::f32>((targetX - x ) * (targetX - x ));
                
                Simian::f32 range = (startX - x) * (startX - x);
                if( range > maxRange_)
                    fScore += NON_VALID_TILE;
                for(Simian::u32 y = minY; y <= maxY; ++y)
                {
                    if( y >= height_ )
                        return;
                    // If Y is in initial min/max
                    if(inInitial && (y >= initialMin_.Y()) && (y <= initialMax_.Y()))
                        continue;
                    Tile& tile = subtiles_[x][y];
                    if (!SIsFlagSet(tile.Flag, Tile::TILE_FLOOR))
                        return;

                    Simian::f32 dY = targetY - y;
                    fScore += calcFScore_(tile,parentGScore,dY * dY + dXSqr);
                }                
            }
        }
        DrawSubtile(node.X(),node.Y(),Simian::Color(0.0f,0.0f,1.0f));
        node.FScore(fScore);
        openListFScore_.insert(std::pair<Simian::u32,PathNode>(node.FScore(),node));
        openListCoord_.insert(std::pair<PathNode,Simian::u32>(node,node.FScore()));
        return;
    }

    void PathFinder::analyzeNeighbor_( Simian::f32 radius, const PathNode& node )
    {
        // wall check here
        Tile& currentTile = subtiles_[node.X()][node.Y()];
        Simian::u32 x = node.X(),
                    y = node.Y();

        if (!SIsFlagSet(currentTile.Flag, Tile::TILE_NORTH) )
            analyzeNode_(radius, PathNode(node.X(), node.Y()+1), node); // Up

        if (!SIsFlagSet(currentTile.Flag, Tile::TILE_EAST))
            analyzeNode_(radius, PathNode(node.X()+1, node.Y()), node); // Right

        if (y >= 0 && !SIsFlagSet(currentTile.Flag, Tile::TILE_SOUTH))
            analyzeNode_(radius, PathNode(node.X(), node.Y()-1), node); // Down

        if (x >= 0 && !SIsFlagSet(currentTile.Flag, Tile::TILE_WEST))
            analyzeNode_(radius, PathNode(node.X()-1, node.Y()), node); // Left

        std::multimap<Simian::u32,PathNode>::iterator i = openListFScore_.find(node.FScore());
        std::multimap<PathNode,Simian::u32,SortByCoord>::iterator j = openListCoord_.find(node);
        if(i != openListFScore_.end())
            openListFScore_.erase(i);
        if(j != openListCoord_.end())
            openListCoord_.erase(j);
    }
    
    Tile::TILE_FLAG PathFinder::determineDir_( const PathNode& prev, const PathNode& next )
    {
        Simian::s32 dx = next.X() - prev.X();
        Simian::s32 dy = next.Y() - prev.Y();

        // Horizontal
        if(dx)
            return dx < 0 ? Tile::TILE_WEST : Tile::TILE_EAST;
        else
            return dy < 0 ? Tile::TILE_SOUTH : Tile::TILE_NORTH;
    }

    //-Private------------------------------------------------------------------

    Simian::Vector2 PathFinder::WorldToSubtile( const Simian::Vector2& world )
    {
        return world * static_cast<Simian::f32>(SUBTILES_PER_TILE);
    }

    Simian::f32 PathFinder::WorldToSubtile( Simian::f32 radius )
    {
        return radius * static_cast<Simian::f32>(SUBTILES_PER_TILE);
    }

    Descension::PathNode PathFinder::WorldToSubtile( const PathNode& world )
    {
        return PathNode(world.X() * SUBTILES_PER_TILE, world.Y() * SUBTILES_PER_TILE);
    }

    Simian::Vector2 PathFinder::SubtileToWorld( const Simian::Vector2& subtile )
    {
        //Simian::f32 hsize = SUBTILE_SIZE * 0.5f;
        return subtile * SUBTILE_SIZE;// + Simian::Vector2(hsize, hsize);
    }

    Simian::f32 PathFinder::SubtileToWorld( Simian::f32 radius )
    {
        return radius * SUBTILE_SIZE;
    }

    void PathFinder::generateSubtiles_( TileMap& map )
    {
        width_ = SUBTILES_PER_TILE * map.Width();
        height_ = SUBTILES_PER_TILE * map.Height();

        subtiles_.resize(width_);
        for (Simian::u32 x = 0; x < width_; ++x)
        {
            Simian::u32 tileX = x/SUBTILES_PER_TILE; // world x
            subtiles_[x].resize(height_);
            for (Simian::u32 y = 0; y < height_; ++y)
            {
                Simian::u32 tileY = y/SUBTILES_PER_TILE;
                Simian::u32 ox = x - tileX * SUBTILES_PER_TILE;
                Simian::u32 oy = y - tileY * SUBTILES_PER_TILE;

                Tile& parentTile = map[tileX][tileY];
                Tile tile;

                SFlagSet(tile.Flag, Tile::TILE_FLOOR, SIsFlagSet(parentTile.Flag, Tile::TILE_FLOOR));
                SFlagSet(tile.Flag, Tile::TILE_DIAGDOWN, SIsFlagSet(parentTile.Flag, Tile::TILE_DIAGDOWN));
                SFlagSet(tile.Flag, Tile::TILE_DIAGUP, SIsFlagSet(parentTile.Flag, Tile::TILE_DIAGUP));

                // add walls
                SFlagSet(tile.Flag, Tile::TILE_WEST, 
                    !ox ? SIsFlagSet(parentTile.Flag, Tile::TILE_WEST) : false);
                SFlagSet(tile.Flag, Tile::TILE_EAST,
                    ox == SUBTILES_PER_TILE - 1 ? SIsFlagSet(parentTile.Flag, Tile::TILE_EAST) : false);
                SFlagSet(tile.Flag, Tile::TILE_SOUTH,
                    !oy ? SIsFlagSet(parentTile.Flag, Tile::TILE_SOUTH) : false);
                SFlagSet(tile.Flag, Tile::TILE_NORTH,
                    oy == SUBTILES_PER_TILE - 1 ? SIsFlagSet(parentTile.Flag, Tile::TILE_NORTH) : false);

                subtiles_[x][y] = tile;
            }
        }
    }

    void PathFinder::SetNewPosition( const Simian::Vector3& position, Simian::f32 radius, Simian::Vector2& min, Simian::Vector2& max )
    {
        UnsetPosition(min,max);

        // find the subtile coordinates for the position
        Simian::Vector2 subtile = WorldToSubtile(Simian::Vector2(position.X(),position.Z()));

        // calculate and clamp the new min and max
        Simian::u32 minX = 0, maxX = 0, minY = 0, maxY = 0;

        CalcMinMax(radius, static_cast<Simian::u32>(subtile.X()), static_cast<Simian::u32>(subtile.Y()), minX, minY, maxX, maxY);
        // occupy the new range
        for (Simian::u32 x = minX; x <= maxX; ++x)
        {
            for (Simian::u32 y = minY; y <= maxY; ++y)
            {
                if(!IsSubTileValid(x,y))
                    continue;
                DrawSubtile(x,y,Simian::Color(0.0f,1.0f,0.0f));
                SFlagSet(subtiles_[x][y].Flag, Tile::TILE_OCCUPIED);
            }
        }

        // return the range!
        min = Simian::Vector2(static_cast<Simian::f32>(minX), static_cast<Simian::f32>(minY));
        max = Simian::Vector2(static_cast<Simian::f32>(maxX), static_cast<Simian::f32>(maxY));
    }

    void PathFinder::UnsetPosition( Simian::Vector2& min, Simian::Vector2& max )
    {
        if (min.X() >= 0 && max.X() >= 0 && min.Y() >= 0 && max.Y() >= 0)
        {
            Simian::u32 minX = static_cast<Simian::u32>(min.X());
            Simian::u32 maxX = static_cast<Simian::u32>(max.X());
            Simian::u32 minY = static_cast<Simian::u32>(min.Y());
            Simian::u32 maxY = static_cast<Simian::u32>(max.Y());

            for (Simian::u32 x = minX; x <= maxX; ++x)
            {
                for (Simian::u32 y = minY; y <= maxY; ++y)
                {
                    if( x >= width_ || y >= height_ )
                        continue;
                    SFlagUnset(subtiles_[x][y].Flag, Tile::TILE_OCCUPIED);
                }
            }
        }
    }

    Simian::u32 PathFinder::calcFScore_( Tile& tile, Simian::u32 gScore, Simian::f32 hScore )
    {
        if(SIsFlagSet(tile.Flag, Tile::TILE_OCCUPIED))
            return gScore + static_cast<Simian::u32>(occupiedScore_ + hScore);
        else
            return static_cast<Simian::u32>(hScore);
    }

    bool PathFinder::CalcMinMax( Simian:: f32 radius, Simian::u32 x, Simian::u32 y,
        Simian::u32& minX, Simian::u32& minY, Simian::u32& maxX, Simian::u32& maxY )
    {
        Simian::f32 subtileRadius = std::floor(radius * SUBTILES_PER_TILE);

        if(subtileRadius < Simian::Math::EPSILON)
        {
            if(x + 1 >= width_)
            {
                minX = width_ - 1;
                maxX = minX;
            }
            /*else if( x < 1 )
            {
            minX = 1;
            maxX = 1;
            }*/
            else
            {
                minX = x;
                maxX = minX;
            }

            if(y + 1 >= height_)
            {
                minY = height_ - 1;
                maxY = minY;
            }
            /*else if( y < 1 )
            {
                minY = 1;
                maxY = 1;
            }*/
            else
            {
                minY = y;
                maxY = minY;
            }
        }
        else
        {
            minX = static_cast<Simian::u32>(Simian::Math::Max(x - subtileRadius, 0.0f));
            maxX = static_cast<Simian::u32>(Simian::Math::Min(x + subtileRadius, width_ - 1.0f));
            minY = static_cast<Simian::u32>(Simian::Math::Max(y - subtileRadius, 0.0f));
            maxY = static_cast<Simian::u32>(Simian::Math::Min(y + subtileRadius, height_ - 1.0f));
            
            /*if( !x || !y )
                return false;
            minX = static_cast<Simian::u32>(x - subtileRadius);
            maxX = static_cast<Simian::u32>(x + subtileRadius);
            minY = static_cast<Simian::u32>(y- subtileRadius);
            maxY = static_cast<Simian::u32>(y + subtileRadius);*/
        }   
        return true;
    }

    bool PathFinder::IsSubTileValid( Simian::u32 x, Simian::u32 y )
    {
        if(x >= width_ || y >= height_ )
            return false;
        unsigned int flag = subtiles_[x][y].Flag;
        if(!SIsFlagSet(subtiles_[x][y].Flag,Tile::TILE_FLOOR) || 
          SIsFlagSet(subtiles_[x][y].Flag,Tile::TILE_OCCUPIED) ||
          SIsFlagSet(flag,Tile::TILE_DIAGDOWN) || SIsFlagSet(flag,Tile::TILE_DIAGUP))
            return false;
        return true;
    }

    bool PathFinder::IsNextSubTileValid( Simian::u32 dir, Simian::u32 x, Simian::u32 y )
    {
        if(IsSubTileValid(x,y))
        {
            bool valid = true;
            unsigned int flag = subtiles_[x][y].Flag;
            if(SIsFlagSet(Tile::TILE_NORTH,dir))
                valid = !SIsFlagSet(flag,Tile::TILE_SOUTH);
            if(valid && SIsFlagSet(Tile::TILE_EAST,dir))
                valid = !SIsFlagSet(flag,Tile::TILE_WEST);
            if(valid && SIsFlagSet(Tile::TILE_SOUTH,dir))
                valid = !SIsFlagSet(flag,Tile::TILE_NORTH);
            if(valid && SIsFlagSet(Tile::TILE_WEST,dir))
                valid =  !SIsFlagSet(flag,Tile::TILE_EAST);
            return valid;
        }
        return false;
    }

    void PathFinder::SetTileEdge( bool state, Simian::u32 minX, Simian::u32 minY, Tile::TILE_FLAG direction )
    {
        // find the first subtile at those coordinates
        minX *= SUBTILES_PER_TILE;
        minY *= SUBTILES_PER_TILE;

        Simian::u32 maxX = minX, maxY = minY;
        if (direction == Tile::TILE_NORTH)
        {
            // y is the top most
            minY += SUBTILES_PER_TILE - 1;
            maxY = minY;
            maxX += SUBTILES_PER_TILE - 1;
        }
        else if (direction == Tile::TILE_SOUTH)
        {
            maxX += SUBTILES_PER_TILE - 1;
        }
        else if (direction == Tile::TILE_EAST)
        {
            minX += SUBTILES_PER_TILE - 1;
            maxX = minX;
            maxY += SUBTILES_PER_TILE - 1;
        }
        else if (direction == Tile::TILE_WEST)
        {
            maxY += SUBTILES_PER_TILE - 1;
        }
        else
            return;

        for (Simian::u32 x = minX; x <= maxX; ++x)
            for (Simian::u32 y = minY; y <= maxY; ++y)
                SFlagSet(subtiles_[x][y].Flag, direction, state);
    }
}
