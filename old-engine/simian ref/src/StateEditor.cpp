/*************************************************************************/
/*!
\file		StateEditor.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/

#include "StateEditor.h"
#include "GCEditor.h"
#include "EntityTypes.h"
#include "GCTileMap.h"

#include "Simian/Game.h"
#include "Simian/Camera.h"
#include "Simian/RenderQueue.h"
#include "Simian/GameFactory.h"
#include "Simian/Scene.h"
#include "Simian/TextureCache.h"
#include "Simian/ModelCache.h"
#include "Simian/Keyboard.h"
#include "Simian/EngineComponents.h"
#include "Simian/CTransform.h"
#include "Simian/DebugRenderer.h"
#include "Simian/CJukebox.h"
#include "Simian/Math.h"
#include "Simian/CSprite.h"
#include "Simian/LogManager.h"

namespace Descension
{
    StateEditor::StateEditor()
        : scene_(0)
    {

    }

    //--------------------------------------------------------------------------

    Simian::Scene* StateEditor::Scene() const
    {
        return scene_;
    }

    void StateEditor::Location( const Simian::DataLocation& val )
    {
        location_ = val;
    }

    const Simian::Vector3& StateEditor::LastCameraPosition() const
    {
        return lastCameraPosition_;
    }

    void StateEditor::LastCameraPosition( const Simian::Vector3& val )
    {
        lastCameraPosition_ = val;
    }

    //--------------------------------------------------------------------------

    void StateEditor::OnEnter()
    {

    }

    void StateEditor::OnLoad()
    {
    }

    void StateEditor::OnInit()
    {
        // search for all pool files
        std::vector<std::string> poolFiles;
        Simian::OS::Instance().GetFilesInDirectory("Data", poolFiles);
        // filter by .pol
        for (Simian::u32 i = 0; i < poolFiles.size();)
        {
            if (poolFiles[i].length() < 4 ||
                poolFiles[i].substr(poolFiles[i].length() - 4) != ".pol")
            {
                poolFiles.erase(poolFiles.begin() + i);
                continue;
            }
            else
                poolFiles[i] = "Data/" + poolFiles[i];
            ++i; // its a pool so just continue iterating.
        }

        Simian::GameFactory::Instance().CreateScene(scene_);

        if (location_)
        {
            // override pool files
            scene_->LoadLayout(location_, poolFiles);
            location_ = Simian::DataLocation();
        }
        else
            scene_->LoadLayout("Data/Scenes/DefaultScene.scn", poolFiles);
        scene_->LoadFiles();
        scene_->LoadResources();

        Simian::Game::Instance().TextureCache().LoadFiles();
        Simian::Game::Instance().TextureCache().LoadTextures();

        Simian::Game::Instance().ModelCache().LoadFiles();
        Simian::Game::Instance().ModelCache().LoadMeshes();

        scene_->LoadObjects();

        ReplacePlayer();

        Simian::Entity* editorEntity = scene_->CreateEntity(E_EDITOR);
        scene_->AddEntity(editorEntity);

        static bool firstRun = true;
        if (firstRun)
        {
            firstRun = false;
            NewMap(64, 64);
        }

        DebugRendererShow();
    }

    void StateEditor::OnUpdate( Simian::f32 dt )
    {
        scene_->Update(dt);
    }

    void StateEditor::OnDeinit()
    {
        scene_->Unload();
        Simian::GameFactory::Instance().DestroyScene(scene_);
    }

    void StateEditor::OnUnload()
    {
    }

    void StateEditor::OnExit()
    {

    }

    void StateEditor::NewMap( Simian::u32 width, Simian::u32 height )
    {
        // unload old scene
        OnDeinit();
        
        // load default scene again
        OnInit();

        // force update scene
        scene_->Update(0);
        
        // unload scene and load the default scene
        GCEditor::Instance().NewMap(width, height);
    }

    void StateEditor::LoadMap( const Simian::DataLocation& data )
    {
        location_ = data;

        // unload old scene
        OnDeinit();

        // load default scene again
        OnInit();

        // force update scene
        scene_->Update(0);

        // clear the render queue
        Simian::Game::Instance().RenderQueue().Draw();
    }

    void StateEditor::SaveMap( const std::string& path )
    {
        GCTileMap* tilemap = GCTileMap::TileMap(scene_);
        if (tilemap) GCEditor::Instance().ExportCollision(tilemap->TileMapFile());
        ReplacePlayerMarker();

        SetCursorVisibility(true);

        SetCameraPosition();
        scene_->Serialize(path, true);
        RevertCameraPosition();

        SetCursorVisibility(false);

        ReplacePlayer();
    }

    void StateEditor::ReplacePlayerMarker()
    {
        // replace player marker with the real player
        Simian::Entity* marker = scene_->EntityByTypeRecursive(E_PLAYERMARKER);
        if (marker)
        {
            Simian::Entity* player = scene_->CreateEntity(E_PLAYER);

            // copy transforms
            Simian::CTransform* markerTransform, *playerTransform;
            marker->ComponentByType(Simian::C_TRANSFORM, markerTransform);
            player->ComponentByType(Simian::C_TRANSFORM, playerTransform);
            Simian::CTransform* rotation;
            player->ChildByName("Player")->ComponentByType(Simian::C_TRANSFORM, rotation);

            // set player transform's position
            playerTransform->Position(markerTransform->Position());
            rotation->Rotation(markerTransform->Axis(), markerTransform->Angle());
            playerPosition_ = markerTransform->World().Position();

            // attach player and detach the marker
            if (marker->ParentEntity())
            {
                marker->ParentEntity()->RemoveChild(marker);
                marker->ParentEntity()->AddChild(player);
            }
            else
            {
                scene_->RemoveEntity(marker);
                scene_->AddEntity(player);
            }

            // update scene to remove player marker and add player!
            scene_->Update(0);

            // remove any draws that the player added
            Simian::Game::Instance().RenderQueue().Draw();
        }
    }

    void StateEditor::ReplacePlayer()
    {
        // replace player marker with the real player
        Simian::Entity* player = scene_->EntityByTypeRecursive(E_PLAYER);
        if (player)
        {
            Simian::Entity* marker = scene_->CreateEntity(E_PLAYERMARKER);

            // copy transforms
            Simian::CTransform* markerTransform, *playerTransform, *rotation;
            player->ComponentByType(Simian::C_TRANSFORM, playerTransform);
            player->ChildByName("Player")->ComponentByType(Simian::C_TRANSFORM, rotation);
            marker->ComponentByType(Simian::C_TRANSFORM, markerTransform);

            // set player transform's position
            markerTransform->Position(playerTransform->Position());
            markerTransform->Rotation(rotation->Axis(), rotation->Angle());

            // attach player and detach the marker
            if (player->ParentEntity())
            {
                player->ParentEntity()->RemoveChild(player);
                player->ParentEntity()->AddChild(marker);
            }
            else
            {
                scene_->RemoveEntity(player);
                scene_->AddEntity(marker);
            }

            // update scene to remove player and add player marker!
            scene_->Update(0);

            // remove any draws that the player marker added
            Simian::Game::Instance().RenderQueue().Draw();
        }
    }

    void StateEditor::SetCursorVisibility(bool visible)
    {
        // make cursor visible
        Simian::Entity* cursor = scene_->EntityByTypeRecursive(E_CURSOR);
        Simian::CSprite* sprite = 0;

        if (cursor)
        {
            cursor->ComponentByType(Simian::C_SPRITE, sprite);
            sprite->Visible(visible);
        }
    }

    void StateEditor::SetCameraPosition()
    {
        GCEditor::Instance().CameraBaseTransform()->Position(playerPosition_);
    }

    void StateEditor::RevertCameraPosition()
    {
        GCEditor::Instance().CameraBaseTransform()->Position(lastCameraPosition_);
    }
}
