/*****************************************************************************/
/*!
\file		StateGame.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "StateGame.h"
#include "CombatCommandSystem.h"
#include "StateLoading.h"
#include "SaveManager.h"

#include "Simian/GameFactory.h"
#include "Simian/Game.h"
#include "Simian/ModelCache.h"
#include "Simian/TextureCache.h"
#include "Simian/Scene.h"
#include "Simian/AudioManager.h"
#include "Simian/Music.h"
#include "Simian/DebugRenderer.h"
#include "Simian/Keyboard.h"
#include "Simian/LogManager.h"

#include "GCPlayerLogic.h"
#include "MetricLogManager.h"

namespace Descension
{
    StateGame::StateGame()
        : levelFile_("Data/Scenes/Splash.scn"),
          scene_(0)
    {
    }

    //--------------------------------------------------------------------------

    const std::string& StateGame::LevelFile() const
    {
        return levelFile_;
    }

    void StateGame::LevelFile( const std::string& val )
    {
        levelFile_ = val;
    }

    Simian::Scene* StateGame::Scene() const
    {
        return scene_;
    }

    void StateGame::Scene( Simian::Scene* val )
    {
        scene_ = val;
    }

    //--------------------------------------------------------------------------

    Simian::u32 StateGame::GetFileIndex(const std::string& fileName)
    {
        //--Crazy hard-coded index for scene files
        if (fileName == "Data/Scenes/Level_0_1.scn") {
            return 0;
        } else if (fileName == "Data/Scenes/Level_0_2.scn") {
            return 1;
        } else if (fileName == "Data/Scenes/Level_1.scn") {
            return 2;
        } else if (fileName == "Data/Scenes/Level_2.scn") {
            return 3;
        } else if (fileName == "Data/Scenes/Level_3_0.scn") {
            return 4;
        } else if (fileName == "Data/Scenes/Level_3.scn") {
            return 5;
        } else if (fileName == "Data/Scenes/Level_4.scn") {
            return 6;
        } else if (fileName == "Data/Scenes/Level_5.scn") {
            return 7;
        } else if (fileName == "Data/Scenes/Level_6.scn") {
            return 8;
        } else if (fileName == "Data/Scenes/Level_7.scn") {
            return 9;
        } else if (fileName == "Data/Scenes/Level_8.scn") {
            return 10;
        }

        return 15; //--last dummy slot
    }

    std::string StateGame::GetFileFromIndex(Simian::u32 id)
    {
        SVerbose(id);
        //--Crazy hard-coded index for scene files
        if (id == 0) {
            return std::string("Data/Scenes/Level_0_1.scn");
        } else if (id == 1) {
            return std::string("Data/Scenes/Level_0_2.scn");
        } else if (id == 2) {
            return std::string("Data/Scenes/Level_1.scn");
        } else if (id == 3) {
            return std::string("Data/Scenes/Level_2.scn");
        } else if (id == 4) {
            return std::string("Data/Scenes/Level_3_0.scn");
        } else if (id == 5) {
            return std::string("Data/Scenes/Level_3.scn");
        } else if (id == 6) {
            return std::string("Data/Scenes/Level_4.scn");
        } else if (id == 7) {
            return std::string("Data/Scenes/Level_5.scn");
        } else if (id == 8) {
            return std::string("Data/Scenes/Level_6.scn");
        } else if (id == 9) {
            return std::string("Data/Scenes/Level_7.scn");
        } else if (id == 10) {
            return std::string("Data/Scenes/Level_8.scn");
        }

        return std::string(); //--last dummy slot
    }

    void StateGame::OnEnter()
    {
    }

    void StateGame::OnLoad()
    {
        if (!scene_)
        {
            Simian::GameFactory::Instance().CreateScene(scene_);

            scene_->LoadLayout(levelFile_);
            scene_->LoadFiles();
            scene_->LoadResources();

            Simian::Game::Instance().TextureCache().LoadFiles();
            Simian::Game::Instance().TextureCache().LoadTextures();

            Simian::Game::Instance().ModelCache().LoadFiles();
            Simian::Game::Instance().ModelCache().LoadMeshes();
        }
        
        //--Unlock the level to current data set
        SaveManager::Instance().CurrentData().LevelsUnlocked[GetFileIndex(levelFile_)] = 1;

        scene_->LoadObjects();
    }

    void StateGame::OnInit()
    {
    }

    void StateGame::OnUpdate(Simian::f32 dt)
    {
        DLogManagerUpdate(dt);
        if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_1))
        {
            DebugRendererToggleVisibility();
        }

        // PRESENTATION HACKS
#if 1
        if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_F1))
        {
            SaveManager::Instance().CurrentData().SaveID = SaveManager::SAVE_NORMAL;
            Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());
            StateLoading::Instance().NextState(StateGame::InstancePtr());
            StateGame::Instance().LevelFile("Data/Scenes/MainMenu.scn");
        }
        else if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_F2))
        {
            SaveManager::Instance().CurrentData().SaveID = SaveManager::SAVE_NORMAL;
            Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());
            StateLoading::Instance().NextState(StateGame::InstancePtr());
            StateGame::Instance().LevelFile("Data/Scenes/Level_0_1.scn");
        }
        else if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_F3))
        {
            SaveManager::Instance().CurrentData().SaveID = SaveManager::SAVE_NORMAL;
            Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());
            StateLoading::Instance().NextState(StateGame::InstancePtr());
            StateGame::Instance().LevelFile("Data/Scenes/Level_0_2.scn");
        }
        else if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_F4))
        {
            SaveManager::Instance().CurrentData().SaveID = SaveManager::SAVE_NORMAL;
            Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());
            StateLoading::Instance().NextState(StateGame::InstancePtr());
            StateGame::Instance().LevelFile("Data/Scenes/Level_1.scn");
        }
        else if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_F5))
        {
            SaveManager::Instance().CurrentData().SaveID = SaveManager::SAVE_NORMAL;
            Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());
            StateLoading::Instance().NextState(StateGame::InstancePtr());
            StateGame::Instance().LevelFile("Data/Scenes/Level_2.scn");
        }
        else if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_F6))
        {
            SaveManager::Instance().CurrentData().SaveID = SaveManager::SAVE_NORMAL;
            Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());
            StateLoading::Instance().NextState(StateGame::InstancePtr());
            StateGame::Instance().LevelFile("Data/Scenes/Level_3.scn");
        }

        else if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_F7))
        {
            SaveManager::Instance().CurrentData().SaveID = SaveManager::SAVE_NORMAL;
            Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());
            StateLoading::Instance().NextState(StateGame::InstancePtr());
            StateGame::Instance().LevelFile("Data/Scenes/Level_4.scn");
        }
		else if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_F8))
        {
            SaveManager::Instance().CurrentData().SaveID = SaveManager::SAVE_NORMAL;
            Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());
            StateLoading::Instance().NextState(StateGame::InstancePtr());
            StateGame::Instance().LevelFile("Data/Scenes/Level_5.scn");
        }
        else if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_F9))
        {
            SaveManager::Instance().CurrentData().SaveID = SaveManager::SAVE_NORMAL;
            Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());
            StateLoading::Instance().NextState(StateGame::InstancePtr());
            StateGame::Instance().LevelFile("Data/Scenes/Level_3_0.scn");
        }
        else if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_PAGEUP))
        {
            SaveManager::Instance().CurrentData().SaveID = SaveManager::SAVE_NORMAL;
            Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());
            StateLoading::Instance().NextState(StateGame::InstancePtr());
            StateGame::Instance().LevelFile("Data/Scenes/Level_6.scn");
        }
        else if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_BACKSPACE))
        {
            SaveManager::Instance().CurrentData().SaveID = SaveManager::SAVE_NORMAL;
            Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());
            StateLoading::Instance().NextState(StateGame::InstancePtr());
            StateGame::Instance().LevelFile("Data/Scenes/Level_7.scn");
        }
        else if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_END))
        {
            SaveManager::Instance().CurrentData().SaveID = SaveManager::SAVE_NORMAL;
            Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());
            StateLoading::Instance().NextState(StateGame::InstancePtr());
            StateGame::Instance().LevelFile("Data/Scenes/Level_8.scn");
        }

        if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_0))
            Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());

        if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_I))
            GCPlayerLogic::Instance()->Attributes()->Invulnerable(true);

        if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_O))
            GCPlayerLogic::Instance()->Attributes()->Damage(80);

#endif

        // process the commands (before any adding or removing happens)
        CombatCommandSystem::Instance().Process();

        scene_->Update(dt);
    }

    void StateGame::OnDeinit()
    {
        // clear out any left overs
        CombatCommandSystem::Instance().Clear();
    }

    void StateGame::OnUnload()
    {
        scene_->Unload();
        Simian::GameFactory::Instance().DestroyScene(scene_);

        // stop all sounds
        Simian::AudioManager::Instance().StopAll();

        // clear scene pointer
        scene_ = 0;

        DLogManagerSave();
    }

    void StateGame::OnExit()
    {
    }
}
