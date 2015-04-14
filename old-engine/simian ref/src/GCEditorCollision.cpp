/*************************************************************************/
/*!
\file		GCEditorCollision.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCEditor.h"

#include "Simian/CTransform.h"
#include "Simian/Vector3.h"
#include "Simian/LogManager.h"

namespace Descension
{
    static const Simian::f32 COLLISION_PAINTTHRESHOLD = 0.05f;
    static const Simian::f32 COLLISION_WALLPAINTTHRESHOLD = 0.25f;

    void GCEditor::collisionModeEnter_( Simian::DelegateParameter& )
    {

    }

    void GCEditor::collisionModeUpdate_( Simian::DelegateParameter& )
    {
    }

    void GCEditor::collisionModeExit_( Simian::DelegateParameter& )
    {

    }

    void GCEditor::collisionModeLButtonStart_()
    {
    }

    void GCEditor::collisionModeLButtonUpdate_()
    {
        if (pickedTile_.X() < 0 || pickedTile_.X() >= width_ || 
            pickedTile_.Z() < 0 || pickedTile_.Z() >= height_)
            return;

        Simian::s32 tileX = static_cast<Simian::s32>(pickedTile_.X());
        Simian::s32 tileY = static_cast<Simian::s32>(pickedTile_.Z());

        // check if the tile we are on is currently lit
        Tile& currentTile = (*tileMap_)[tileX][tileY];
        if (!SIsFlagSet(currentTile.Flag, Tile::TILE_FLOOR))
        {
            painted_ = true;

            // paint the tile!
            SFlagSet(currentTile.Flag, Tile::TILE_FLOOR);

#define SETUP_TILE(x, y, f1, f2) \
            if (x >= 0 && x < static_cast<Simian::s32>(width_) && \
                y >= 0 && y < static_cast<Simian::s32>(height_) && \
                SIsFlagSet((*tileMap_)[x][y].Flag, Tile::TILE_FLOOR)) \
                SFlagUnset((*tileMap_)[x][y].Flag, f1); \
            else \
                SFlagSet(currentTile.Flag, f2);

            SETUP_TILE(tileX - 1, tileY, Tile::TILE_EAST, Tile::TILE_WEST);
            SETUP_TILE(tileX + 1, tileY, Tile::TILE_WEST, Tile::TILE_EAST);
            SETUP_TILE(tileX, tileY + 1, Tile::TILE_SOUTH, Tile::TILE_NORTH);
            SETUP_TILE(tileX, tileY - 1, Tile::TILE_NORTH, Tile::TILE_SOUTH);
#undef SETUP_TILE
        }
    }

    void GCEditor::collisionModeLButtonEnd_()
    {
        if (!painted_)
        {
            // reject if out of range
            if (pickedTile_.X() < 0 || pickedTile_.X() >= width_ || 
                pickedTile_.Z() < 0 || pickedTile_.Z() >= height_)
                return;

            // toggle walls
            Simian::s32 tileX = static_cast<Simian::s32>(pickedTile_.X());
            Simian::s32 tileY = static_cast<Simian::s32>(pickedTile_.Z());

            Simian::Vector3 tileOffset(pickedCoordinates_ - pickedTile_);

            Tile& currentTile = (*tileMap_)[tileX][tileY];

#define SETUP_WALL(x, y, wall) \
            if (x >= 0 && x < static_cast<Simian::s32>(width_) &&\
                y >= 0 && y < static_cast<Simian::s32>(height_) &&\
                SIsFlagSet((*tileMap_)[x][y].Flag, Tile::TILE_FLOOR))\
                SFlagToggle((*tileMap_)[x][y].Flag, wall);

            // toggle west wall
            if (tileOffset.X() < COLLISION_WALLPAINTTHRESHOLD)
            {
                SFlagToggle(currentTile.Flag, Tile::TILE_WEST);

                // toggle east wall of neighbour
                SETUP_WALL(tileX - 1, tileY, Tile::TILE_EAST);
            }

            // toggle west wall
            else if (tileOffset.X() > 1.0f - COLLISION_WALLPAINTTHRESHOLD)
            {
                SFlagToggle(currentTile.Flag, Tile::TILE_EAST);

                // toggle west wall of neighbour
                SETUP_WALL(tileX + 1, tileY, Tile::TILE_WEST);
            }

            // toggle west wall
            else if (tileOffset.Z() < COLLISION_WALLPAINTTHRESHOLD)
            {
                SFlagToggle(currentTile.Flag, Tile::TILE_SOUTH);

                // toggle west wall of neighbour
                SETUP_WALL(tileX, tileY - 1, Tile::TILE_NORTH);
            }

            // toggle west wall
            else if (tileOffset.Z() > 1.0f - COLLISION_WALLPAINTTHRESHOLD)
            {
                SFlagToggle(currentTile.Flag, Tile::TILE_NORTH);

                // toggle west wall of neighbour
                SETUP_WALL(tileX, tileY + 1, Tile::TILE_SOUTH);
            }

            // toggle diagonals
            else
            {
                if (SIsFlagSet(currentTile.Flag, Tile::TILE_DIAGDOWN))
                {
                    SFlagUnset(currentTile.Flag, Tile::TILE_DIAGDOWN);
                    SFlagSet(currentTile.Flag, Tile::TILE_DIAGUP);
                }
                else if (SIsFlagSet(currentTile.Flag, Tile::TILE_DIAGUP))
                {
                    SFlagUnset(currentTile.Flag, Tile::TILE_DIAGUP);
                }
                else
                {
                    SFlagSet(currentTile.Flag, Tile::TILE_DIAGDOWN);
                }
            }

#undef SETUP_WALL
        }
    }

    void GCEditor::collisionModeRButtonStart_()
    {

    }

    void GCEditor::collisionModeRButtonUpdate_()
    {
        if (pickedTile_.X() < 0 || pickedTile_.X() >= width_ || 
            pickedTile_.Z() < 0 || pickedTile_.Z() >= height_)
            return;

        Simian::s32 tileX = static_cast<Simian::s32>(pickedTile_.X());
        Simian::s32 tileY = static_cast<Simian::s32>(pickedTile_.Z());

        // check if the tile we are on is currently lit
        Tile& currentTile = (*tileMap_)[tileX][tileY];
        if (SIsFlagSet(currentTile.Flag, Tile::TILE_FLOOR))
        {
            // unpaint the tile!
            currentTile.Flag = 0;

#define SETUP_TILE(x, y, f1) \
            if (x >= 0 && x < static_cast<Simian::s32>(width_) && \
                y >= 0 && y < static_cast<Simian::s32>(height_) && \
                SIsFlagSet((*tileMap_)[x][y].Flag, Tile::TILE_FLOOR)) \
                SFlagSet((*tileMap_)[x][y].Flag, f1);

            SETUP_TILE(tileX - 1, tileY, Tile::TILE_EAST);
            SETUP_TILE(tileX + 1, tileY, Tile::TILE_WEST);
            SETUP_TILE(tileX, tileY + 1, Tile::TILE_SOUTH);
            SETUP_TILE(tileX, tileY - 1, Tile::TILE_NORTH);
#undef SETUP_TILE
        }
    }

    void GCEditor::collisionModeRButtonEnd_()
    {

    }
}
