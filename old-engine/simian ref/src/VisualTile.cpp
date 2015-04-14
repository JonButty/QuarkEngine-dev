/*************************************************************************/
/*!
\file		VisualTile.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "VisualTile.h"
#include "TileMap.h"

#include "Simian/VertexBuffer.h"
#include "Simian/Material.h"
#include "Simian/Game.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/MaterialCache.h"
#include "Simian/RenderQueue.h"
#include "Simian/Delegate.h"
#include "Simian/Utility.h"
#include "Simian/Angle.h"

namespace Descension
{
    static const Simian::Color VISUALTILE_COLOR(1.0f, 1.0f, 1.0f);
    static const Simian::f32 VISUALTILE_THICKNESS = 0.12f;
    static const Simian::f32 VISUALTILE_HEIGHT = 0.3f;
    static const Simian::f32 VISUALTILE_FLOORHEIGHT = 0.08f;

    Simian::VertexBuffer* VisualTile::unitCube_ = 0;

    VisualTile::VisualTile( const Simian::Vector2& position, Tile* tile, Simian::Material* material )
        : tile_(tile),
          position_(position),
          material_(material)
    {
    }

    //--------------------------------------------------------------------------

    void VisualTile::draw_( Simian::DelegateParameter& )
    {
        Simian::Game::Instance().GraphicsDevice().Draw(unitCube_, Simian::GraphicsDevice::PT_TRIANGLES, 12);
    }

    void VisualTile::drawPiece_( const Simian::Matrix& transform )
    {
        // draw base
        Simian::Game::Instance().RenderQueue().AddRenderObject(Simian::RenderObject(
            *material_, Simian::RenderQueue::RL_SCENE, transform,
            Simian::Delegate::CreateDelegate<VisualTile, &VisualTile::draw_>(this), NULL));
    }

    //--------------------------------------------------------------------------

    void VisualTile::LoadShared()
    {
        Simian::Game::Instance().GraphicsDevice().CreateVertexBuffer(unitCube_);
        unitCube_->Load(Simian::PositionColorUVVertex::Format, 36);
        Simian::PositionColorUVVertex verts[] =
        {
            // front
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f, -0.5f, 0.5f), VISUALTILE_COLOR, Simian::Vector2(0, 1)),
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f, 0.5f, 0.5f), VISUALTILE_COLOR, Simian::Vector2(0, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3(0.5f, 0.5f, 0.5f), VISUALTILE_COLOR, Simian::Vector2(1, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3(0.5f, 0.5f, 0.5f), VISUALTILE_COLOR, Simian::Vector2(1, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3(0.5f, -0.5f, 0.5f), VISUALTILE_COLOR, Simian::Vector2(1, 1)),
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f, -0.5f, 0.5f), VISUALTILE_COLOR, Simian::Vector2(0, 1)),

            // back
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f, -0.5f, -0.5f), VISUALTILE_COLOR, Simian::Vector2(0, 1)),
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f, 0.5f, -0.5f), VISUALTILE_COLOR, Simian::Vector2(0, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3(0.5f, 0.5f, -0.5f), VISUALTILE_COLOR, Simian::Vector2(1, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3(0.5f, 0.5f, -0.5f), VISUALTILE_COLOR, Simian::Vector2(1, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3(0.5f, -0.5f, -0.5f), VISUALTILE_COLOR, Simian::Vector2(1, 1)),
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f, -0.5f, -0.5f), VISUALTILE_COLOR, Simian::Vector2(0, 1)),

            // left
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f, -0.5f, -0.5f), VISUALTILE_COLOR, Simian::Vector2(0, 1)),
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f, -0.5f,  0.5f), VISUALTILE_COLOR, Simian::Vector2(0, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f,  0.5f,  0.5f), VISUALTILE_COLOR, Simian::Vector2(1, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f,  0.5f,  0.5f), VISUALTILE_COLOR, Simian::Vector2(1, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f,  0.5f, -0.5f), VISUALTILE_COLOR, Simian::Vector2(1, 1)),
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f, -0.5f, -0.5f), VISUALTILE_COLOR, Simian::Vector2(0, 1)),

            // right
            Simian::PositionColorUVVertex(Simian::Vector3(0.5f, -0.5f, -0.5f), VISUALTILE_COLOR, Simian::Vector2(0, 1)),
            Simian::PositionColorUVVertex(Simian::Vector3(0.5f, -0.5f,  0.5f), VISUALTILE_COLOR, Simian::Vector2(0, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3(0.5f,  0.5f,  0.5f), VISUALTILE_COLOR, Simian::Vector2(1, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3(0.5f,  0.5f,  0.5f), VISUALTILE_COLOR, Simian::Vector2(1, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3(0.5f,  0.5f, -0.5f), VISUALTILE_COLOR, Simian::Vector2(1, 1)),
            Simian::PositionColorUVVertex(Simian::Vector3(0.5f, -0.5f, -0.5f), VISUALTILE_COLOR, Simian::Vector2(0, 1)),

            // top
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f, 0.5f, -0.5f), VISUALTILE_COLOR, Simian::Vector2(0, 1)),
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f, 0.5f,  0.5f), VISUALTILE_COLOR, Simian::Vector2(0, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3( 0.5f, 0.5f,  0.5f), VISUALTILE_COLOR, Simian::Vector2(1, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3( 0.5f, 0.5f,  0.5f), VISUALTILE_COLOR, Simian::Vector2(1, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3( 0.5f, 0.5f, -0.5f), VISUALTILE_COLOR, Simian::Vector2(1, 1)),
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f, 0.5f, -0.5f), VISUALTILE_COLOR, Simian::Vector2(0, 1)),

            // bottom
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f, -0.5f, -0.5f), VISUALTILE_COLOR, Simian::Vector2(0, 1)),
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f, -0.5f,  0.5f), VISUALTILE_COLOR, Simian::Vector2(0, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3( 0.5f, -0.5f,  0.5f), VISUALTILE_COLOR, Simian::Vector2(1, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3( 0.5f, -0.5f,  0.5f), VISUALTILE_COLOR, Simian::Vector2(1, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3( 0.5f, -0.5f, -0.5f), VISUALTILE_COLOR, Simian::Vector2(1, 1)),
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f, -0.5f, -0.5f), VISUALTILE_COLOR, Simian::Vector2(0, 1))
        };
        unitCube_->Data(verts);
    }

    void VisualTile::UnloadShared()
    {
        unitCube_->Unload();
        Simian::Game::Instance().GraphicsDevice().DestroyVertexBuffer(unitCube_);
    }

    void VisualTile::Draw()
    {
        Simian::Matrix translation = Simian::Matrix::Translation(position_.X() + 0.5f, 0, position_.Y() + 0.5f);

        // draw base
        if (SIsFlagSet(tile_->Flag, Tile::TILE_FLOOR))
            drawPiece_(translation * Simian::Matrix::Scale(1.0f, VISUALTILE_FLOORHEIGHT, 1.0f));

        Simian::f32 wallYOffset = VISUALTILE_HEIGHT * 0.5f + VISUALTILE_FLOORHEIGHT;

        // left wall
        if (SIsFlagSet(tile_->Flag, Tile::TILE_WEST))
            drawPiece_(Simian::Matrix::Translation(-0.5f + VISUALTILE_THICKNESS * 0.5f, wallYOffset, 0) *
                translation * Simian::Matrix::Scale(VISUALTILE_THICKNESS, VISUALTILE_HEIGHT, 1.0f));

        // right wall
        if (SIsFlagSet(tile_->Flag, Tile::TILE_EAST))
            drawPiece_(Simian::Matrix::Translation(0.5f - VISUALTILE_THICKNESS * 0.5f, wallYOffset, 0) *
                translation * Simian::Matrix::Scale(VISUALTILE_THICKNESS, VISUALTILE_HEIGHT, 1.0f));

        // top wall
        if (SIsFlagSet(tile_->Flag, Tile::TILE_NORTH))
            drawPiece_(Simian::Matrix::Translation(0, wallYOffset, 0.5f - VISUALTILE_THICKNESS * 0.5f) *
                translation * Simian::Matrix::Scale(1.0f, VISUALTILE_HEIGHT, VISUALTILE_THICKNESS));

        // bottom wall
        if (SIsFlagSet(tile_->Flag, Tile::TILE_SOUTH))
            drawPiece_(Simian::Matrix::Translation(0, wallYOffset, -0.5f + VISUALTILE_THICKNESS * 0.5f) *
                translation * Simian::Matrix::Scale(1.0f, VISUALTILE_HEIGHT, VISUALTILE_THICKNESS));

        // diag down wall
        if (SIsFlagSet(tile_->Flag, Tile::TILE_DIAGDOWN))
            drawPiece_(Simian::Matrix::Translation(0, wallYOffset, 0) *
            translation * Simian::Matrix::RotationAxisAngle(Simian::Vector3(0, 1, 0), Simian::Degree(45.0f)) *
            Simian::Matrix::Scale(std::sqrt(2.0f), VISUALTILE_HEIGHT, VISUALTILE_THICKNESS));

        // diag up wall
        if (SIsFlagSet(tile_->Flag, Tile::TILE_DIAGUP))
            drawPiece_(Simian::Matrix::Translation(0, wallYOffset, 0) *
            translation * Simian::Matrix::RotationAxisAngle(Simian::Vector3(0, 1, 0), Simian::Degree(-45.0f)) *
            Simian::Matrix::Scale(std::sqrt(2.0f), VISUALTILE_HEIGHT, VISUALTILE_THICKNESS));
    }
}
