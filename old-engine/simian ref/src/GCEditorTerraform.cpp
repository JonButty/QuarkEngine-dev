/*************************************************************************/
/*!
\file		GCEditorTerraform.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/30
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
#include "Simian/Keyboard.h"
#include "Simian/Math.h"
#include "Simian/Scene.h"
#include "Simian/EngineComponents.h"

namespace Descension
{
    static const Simian::f32 COLLISION_PAINTTHRESHOLD = 0.05f;
    static const Simian::f32 COLLISION_WALLPAINTTHRESHOLD = 0.25f;

    void GCEditor::terraformModeEnter_( Simian::DelegateParameter& )
    {
    }

    void GCEditor::terraformModeUpdate_( Simian::DelegateParameter& )
    {
        if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_ESCAPE))
        {
            // clear all terraforming tiles
            for (Simian::u32 i = 0; i < terraformMap_->Width(); ++i)
                for (Simian::u32 j = 0; j < terraformMap_->Height(); ++j)
                    (*terraformMap_)[i][j].Flag = 0;
        }
    }

    void GCEditor::terraformModeExit_( Simian::DelegateParameter& )
    {
    }

    void GCEditor::terraformModeLButtonStart_()
    {
    }

    void GCEditor::terraformModeLButtonUpdate_()
    {
        if (pickedTile_.X() < 0 || pickedTile_.X() >= width_ || 
            pickedTile_.Z() < 0 || pickedTile_.Z() >= height_)
            return;

        Simian::s32 tileX = static_cast<Simian::s32>(pickedTile_.X());
        Simian::s32 tileY = static_cast<Simian::s32>(pickedTile_.Z());

        // check if the tile we are on is currently lit
        Tile& currentTile = (*terraformMap_)[tileX][tileY];
        if (!SIsFlagSet(currentTile.Flag, Tile::TILE_FLOOR))
        {
            painted_ = true;

            // paint the tile!
            SFlagSet(currentTile.Flag, Tile::TILE_FLOOR);
        }
    }

    void GCEditor::terraformModeLButtonEnd_()
    {
    }

    void GCEditor::terraformModeRButtonStart_()
    {
    }

    void GCEditor::terraformModeRButtonUpdate_()
    {
        if (pickedTile_.X() < 0 || pickedTile_.X() >= width_ || 
            pickedTile_.Z() < 0 || pickedTile_.Z() >= height_)
            return;

        Simian::s32 tileX = static_cast<Simian::s32>(pickedTile_.X());
        Simian::s32 tileY = static_cast<Simian::s32>(pickedTile_.Z());

        // check if the tile we are on is currently lit
        Tile& currentTile = (*terraformMap_)[tileX][tileY];
        currentTile.Flag = 0;
    }

    void GCEditor::terraformModeRButtonEnd_()
    {
    }

    Simian::Entity* GCEditor::terraformCreateEntity_( const std::vector<Simian::s32> entities, 
                                                      Simian::u32 x, Simian::u32 y, 
                                                      Simian::Entity* parent )
    {
        // create a random entity
        Simian::u32 tileId = Simian::Math::Random(0, entities.size());
        // spawn entity
        Simian::Entity* entity = ParentScene()->CreateEntity(entities[tileId]);

        Simian::CTransform* transform, *parentTransform = 0;
        entity->ComponentByType(Simian::C_TRANSFORM, transform);
        if (parent)
            parent->ComponentByType(Simian::C_TRANSFORM, parentTransform);

        if (transform)
        {
            Simian::Vector3 pos(static_cast<Simian::f32>(x) + 0.5f,
                                0.0f,
                                static_cast<Simian::f32>(y) + 0.5f);

            // convert to local space
            if (parentTransform)
                parentTransform->World().Inversed().Transform(pos);

            transform->Position(pos);
        }

        return entity;
    }

    Simian::u8 GCEditor::terraformHasTile_( Simian::u32 x, Simian::u32 y )
    {
        return x < terraformMap_->Width() && 
               y < terraformMap_->Height() &&
               (*terraformMap_)[x][y].Flag ? 1 : 0;
    }

    //--------------------------------------------------------------------------

    void GCEditor::TerraformFill( const std::vector<Simian::s32>& entities, Simian::Entity* parent )
    {
        // scan the map for tiles
        for (Simian::u32 i = 0; i < terraformMap_->Width(); ++i)
        {
            for (Simian::u32 j = 0; j < terraformMap_->Height(); ++j)
            {
                if ((*terraformMap_)[i][j].Flag)
                {
                    Simian::Entity* entity = terraformCreateEntity_(entities, i, j, parent);

                    if (parent)
                        parent->AddChild(entity);
                    else
                        ParentScene()->AddEntity(entity);
                }
            }
        }
    }

    void GCEditor::TerraformFringe(const std::vector<Simian::s32>& side, 
                                   const std::vector<Simian::s32>& corner, 
                                   const std::vector<Simian::s32>& fill, 
                                   const std::vector<Simian::s32>& knob, 
                                   float knobOffset,
                                   Simian::Entity* parent)
    {
        side, fill, knob;

        for (Simian::u32 i = 0; i < terraformMap_->Width(); ++i)
        {
            for (Simian::u32 j = 0; j < terraformMap_->Height(); ++j)
            {
                // skip filled tiles
                if ((*terraformMap_)[i][j].Flag)
                    continue;

                // op code for the tile
                Simian::u16 outCode = 0;
                Simian::u32 foundTiles = 0;

                Simian::u32 startX = i - 1;
                Simian::u32 startY = j - 1;
                for (Simian::u32 x = 0; x < 3; ++x)
                    for (Simian::u32 y = 0; y < 3; ++y)
                    {
                        Simian::u32 code = terraformHasTile_(startX + x, startY + y) << (y * 3 + x);
                        foundTiles += code ? 1 : 0;
                        outCode |= code;
                    }

                // see if we found anything
                if (foundTiles == 0)
                    continue;

                // corner check
                const Simian::u16 CORNER_FLAG = 325;

                // check for special cases
                if (foundTiles == 1 && outCode & CORNER_FLAG && corner.size())
                {
                    const Simian::u16 CORNER_TL = 1 << 0;
                    const Simian::u16 CORNER_TR = 1 << 2;
                    const Simian::u16 CORNER_BL = 1 << 6;
                    const Simian::u16 CORNER_BR = 1 << 8;

                    Simian::Entity* entity = terraformCreateEntity_(corner, i, j, parent);
                    Simian::CTransform* transform;
                    entity->ComponentByType(Simian::C_TRANSFORM, transform);

                    if (transform)
                    {
                        float angle = 0.0f;
                        if (outCode & CORNER_TL) angle = 180.0f;
                        else if (outCode & CORNER_TR) angle = 90.0f;
                        else if (outCode & CORNER_BR) angle = 0.0f;
                        else if (outCode & CORNER_BL) angle = 270.0f;
                        transform->Rotation(Simian::Vector3::UnitY, Simian::Degree(angle));
                    }

                    if (parent)
                        parent->AddChild(entity);
                    else
                        ParentScene()->AddEntity(entity);
                }
                else
                {
                    const Simian::u16 CORNER_TL = 10;
                    const Simian::u16 CORNER_TR = 34;
                    const Simian::u16 CORNER_BL = 136;
                    const Simian::u16 CORNER_BR = 160;

#define HAS_CODE(c) ((outCode & c) == c)
                    if (HAS_CODE(CORNER_TL) || HAS_CODE(CORNER_TR) ||
                        HAS_CODE(CORNER_BL) || HAS_CODE(CORNER_BR))
                    {
                        if (fill.size())
                        {
                            Simian::Entity* entity = terraformCreateEntity_(fill, i, j, parent);

                            if (parent)
                                parent->AddChild(entity);
                            else
                                ParentScene()->AddEntity(entity);
                        }

                        if (knob.size())
                        {
                            Simian::Entity* entity = terraformCreateEntity_(knob, i, j, parent);
                            Simian::CTransform* transform;
                            entity->ComponentByType(Simian::C_TRANSFORM, transform);

                            if (transform)
                            {
                                float angle = 0.0f;
                                if (HAS_CODE(CORNER_TL)) angle = 180.0f;
                                if (HAS_CODE(CORNER_TR)) angle = 90.0f;
                                if (HAS_CODE(CORNER_BR)) angle = 0.0f;
                                if (HAS_CODE(CORNER_BL)) angle = 270.0f;
                                transform->Rotation(Simian::Vector3::UnitY, Simian::Degree(angle));
                                transform->Position(transform->Position() + Simian::Vector3(0.0f, knobOffset, 0.0f));
                            }

                            if (parent)
                                parent->AddChild(entity);
                            else
                                ParentScene()->AddEntity(entity);
                        }
                    }
                    else if (side.size())
                    {
                        const Simian::u16 SIDE_T = 2;
                        const Simian::u16 SIDE_L = 8;
                        const Simian::u16 SIDE_R = 32;
                        const Simian::u16 SIDE_B = 128;

                        Simian::Entity* entity = terraformCreateEntity_(side, i, j, parent);
                        Simian::CTransform* transform;
                        entity->ComponentByType(Simian::C_TRANSFORM, transform);

                        if (transform)
                        {
                            float angle = 0.0f;

                            // its a side, check which side it is
                            if (outCode & SIDE_T) angle = 180.0f;
                            if (outCode & SIDE_L) angle = 270.0f;
                            if (outCode & SIDE_R) angle = 90.0f;
                            if (outCode & SIDE_B) angle = 0.0f;

                            transform->Rotation(Simian::Vector3::UnitY, Simian::Degree(angle));
                        }

                        if (parent)
                            parent->AddChild(entity);
                        else
                            ParentScene()->AddEntity(entity);
                    }
#undef HAS_CODE
                }
            }
        }
    }
}
