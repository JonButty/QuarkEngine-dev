/*************************************************************************/
/*!
\file		editor.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifdef _DESCENDED

#include "Simian/SimianPlatform.h"
#include "Simian/Game.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/Viewport.h"
#include "Simian/Scene.h"

#include "GCEditor.h"
#include "GCPhysicsController.h"
#include "StateEditor.h"
#include "StateGame.h"
#include "Simian/ModelCache.h"

#define DESCENDED_EXPORT __declspec(dllexport) __stdcall

extern "C"
{
    void DESCENDED_EXPORT UpdateGame()
    {
        Simian::Game::Instance().Update();
    }

    void DESCENDED_EXPORT ShutdownGame()
    {
        Simian::Game::Instance().Shutdown();
    }

    void DESCENDED_EXPORT ResizeWindow(Simian::f32 width, Simian::f32 height)
    {
        Simian::Game::Instance().Size(Simian::Vector2(width, height));
        if (&Simian::Game::Instance().Viewport())
            Simian::Game::Instance().Viewport().Size(Simian::Vector2(width, height));
        if (&Simian::Game::Instance().GraphicsDevice())
            Simian::Game::Instance().GraphicsDevice().Size(Simian::Vector2(width, height));
    }

    void DESCENDED_EXPORT NewMap(Simian::u32 width, Simian::u32 height)
    {
        // reset the editor state
        Descension::StateEditor::Instance().NewMap(width, height);
    }

    void DESCENDED_EXPORT PlayLevel()
    {
        Simian::Game::Instance().GameStateManager().ChangeState(Descension::StateGame::InstancePtr());
        Descension::GCEditor::Instance().SelectObject(NULL, false);
        Descension::GCPhysicsController::EnableAllPhysics(true);

        Descension::GCTileMap* tileMap = 
            Descension::GCTileMap::TileMap(Descension::StateEditor::Instance().Scene());
        if (tileMap)
            Descension::GCEditor::Instance().ExportCollision(tileMap->TileMapFile());
        Descension::StateEditor::Instance().ReplacePlayerMarker();
        Descension::GCEditor::EditorMode(false);

        Descension::StateEditor::Instance().SetCursorVisibility(true);
        Descension::StateEditor::Instance().SetCameraPosition();
        Descension::StateEditor::Instance().Scene()->Serialize("dump.scn", true);
        Descension::StateEditor::Instance().RevertCameraPosition();
        Descension::StateEditor::Instance().SetCursorVisibility(false);

        Descension::StateGame::Instance().LevelFile("dump.scn");
    }

    void DESCENDED_EXPORT StopLevel()
    {
        Simian::Game::Instance().GameStateManager().ChangeState(Descension::StateEditor::InstancePtr());
        Descension::StateEditor::Instance().Location("dump.scn");
        Descension::GCEditor::EditorMode(true);

        // force an update
        UpdateGame();
    }

    void DESCENDED_EXPORT SetCollisionMode()
    {
        Descension::GCEditor::Instance().ModeSM().ChangeState(Descension::GCEditor::EM_COLLISION);
    }

    void DESCENDED_EXPORT SetSceneMode()
    {
        Descension::GCEditor::Instance().ModeSM().ChangeState(Descension::GCEditor::EM_SCENE);
    }

    void DESCENDED_EXPORT SetTerraformMode()
    {
        Descension::GCEditor::Instance().ModeSM().ChangeState(Descension::GCEditor::EM_TERRAFORM);
    }

    void DESCENDED_EXPORT ExportCollision(const char* path)
    {
        Descension::GCEditor::Instance().ExportCollision(path);
    }

    void DESCENDED_EXPORT LoadMap(const char* path)
    {
        Descension::StateEditor::Instance().LastCameraPosition(Simian::Vector3::Zero);
        Descension::StateEditor::Instance().LoadMap(path);
    }

    void DESCENDED_EXPORT SaveMap(const char* path)
    {
        Descension::StateEditor::Instance().SaveMap(path);
    }

    void DESCENDED_EXPORT SetXYZSnap(Simian::f32 x, Simian::f32 y, Simian::f32 z)
    {
        Descension::GCEditor::Instance().XyzSnap(Simian::Vector3(x, y, z));
    }

    void DESCENDED_EXPORT SetRotationSnap(Simian::f32 rot)
    {
        Descension::GCEditor::Instance().RotationSnap(rot);
    }

    void DESCENDED_EXPORT SelectGameObject(Simian::Entity* entity, bool addToSelection)
    {
        Descension::GCEditor::Instance().SelectObject(entity, addToSelection);
    }

    void DESCENDED_EXPORT Focus(bool focus)
    {
        if (Descension::GCEditor::EditorMode())
            Descension::GCEditor::Instance().Disabled(!focus);
    }

    void DESCENDED_EXPORT ZoomBy(int zoom)
    {
        Descension::GCEditor::Instance().ZoomBy(zoom);
    }

    void DESCENDED_EXPORT SetGridVisibility(bool visible)
    {
        Descension::GCEditor::Instance().GridVisibility(visible);
    }

    void DESCENDED_EXPORT SetCollisionVisibility(bool visible)
    {
        Descension::GCEditor::Instance().CollisionVisibility(visible);
    }

    void DESCENDED_EXPORT SetObjectVisibility(bool visible)
    {
        Descension::GCEditor::Instance().ObjectVisibility(visible);
    }

    void DESCENDED_EXPORT ReloadResources(bool materials, bool models)
    {
        if (materials)
            Simian::Game::Instance().MaterialCache().Reload();
        if (models)
            Simian::Game::Instance().ModelCache().Reload();
    }

    typedef struct
    {
        Simian::f32 X;
        Simian::f32 Y;
        Simian::f32 Z;
        Simian::f32 Yaw;
        Simian::f32 Pitch;
        Simian::f32 Zoom;
    } cameraInfo_;

    // Camera information
    __declspec(dllexport) void __stdcall CameraGetInfo(cameraInfo_* cam)
    {
        Simian::Vector3 pos, rot;
        Descension::GCEditor::Instance().CameraGetInfo(pos, rot);
        cam->X = pos.X();
        cam->Y = pos.Y();
        cam->Z = pos.Z();
        cam->Yaw = rot.X();
        cam->Pitch = rot.Y();
        cam->Zoom = rot.Z();
    }
};

#endif
