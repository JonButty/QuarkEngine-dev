/*************************************************************************/
/*!
\file		SaveManager.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/11/30
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/

#include "SaveManager.h"
#include "StateLoading.h"
#include "StateGame.h"

#include "Simian/LogManager.h"
#include "Simian/FileSystem.h"
#include "Simian/Game.h"

#include "GCGestureInterpreter.h"
#include "GCPlayerLogic.h"
#include "GCEditor.h"

#include <fstream>
#include <memory>
#include <stdio.h>

namespace Descension
{
    const std::string SaveManager::SAVE_DIRECTORY =
        Simian::FileSystem::GetDocumentsFolder() + "/Digipen/Descension/GameData.sav";

    SaveManager::SaveManager()
    {
        std::memset(&currentData_, 0, sizeof(GlobalGameData));
        std::memset(&initialData_, 0, sizeof(GlobalGameData));
    }

    std::string SaveManager::GetCurrentLevel(const GlobalGameData& data)
    {
        return std::string(data.CurrentLevel);
    }

    void SaveManager::SetCurrentLevel(const std::string& levelName,
                                      GlobalGameData& data)
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        char buffer[SAVE_MAX_FILENAME] = {0};
        sprintf(buffer, "%s", &levelName[0]);

        std::memcpy(&data.CurrentLevel, buffer, SAVE_MAX_FILENAME);
    }

    void SaveManager::UpdateCurrentData(void)
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        //--Get skills
        currentData_.SkillsUnlocked = 
            static_cast<Simian::u8>(GCGestureInterpreter::Instance()->GetSkills().Skills());
        currentData_.Checkpoint = SAVE_CHECKPOINT;
        
        //--Get level string
        char buffer[SAVE_MAX_FILENAME] = {0};
        sprintf(buffer, "%s", &(StateGame::Instance().LevelFile()[0]));
        std::memcpy(&currentData_.CurrentLevel, buffer, SAVE_MAX_FILENAME);

        currentData_.HP = GCPlayerLogic::Instance()->Attributes()->MaxHealth();
        currentData_.MP = GCPlayerLogic::Instance()->Attributes()->MaxMana();
        currentData_.Exp = GCPlayerLogic::Instance()->Attributes()->Experience();

        //--***--+NOTE+--***
        //--***DO NOT SET SAVEID, POSITION, CRYSTAL, LEVELSUNLOCKED HERE!!!
    }

    void SaveManager::CopyInitialToCurrent(void)
    {
        std::memcpy(&currentData_, &initialData_, sizeof(GlobalGameData));
    }

    void SaveManager::LoadCheckpoint(bool useCurrentData)
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        if (!useCurrentData)
            std::memcpy(&currentData_, &initialData_, sizeof(GlobalGameData));

        Simian::Game::Instance().GameStateManager().ChangeState(
            StateLoading::InstancePtr());
        StateLoading::Instance().NextState(StateGame::InstancePtr());
        StateGame::Instance().LevelFile(currentData_.CurrentLevel);
    }

    void SaveManager::LoadData(void)
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        std::ifstream file(SAVE_DIRECTORY, std::ios::binary);

        if (!file.is_open()) //--Create new file that is not validated if none
        {
            std::memset(&initialData_, 0, sizeof(GlobalGameData));
            SaveData(SAVE_INVALID); //--This data is not validated, all zeroes
            return;
        }

        file.read(reinterpret_cast<char*>(&initialData_), sizeof(GlobalGameData));

        file.close();
    }

    void SaveManager::SaveData(Simian::u32 saveID)
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        std::ofstream file(SAVE_DIRECTORY, std::ios::binary);

        if (!file.is_open())
        {
            SWarn("SaveManager: Cannot open " << SAVE_DIRECTORY);
            return;
        }
        
        if (saveID == SAVE_INVALID) //--Used to reset a game data slot
        {
            std::memset(&currentData_, 0, sizeof(GlobalGameData));
            std::memset(&initialData_, 0, sizeof(GlobalGameData));
            file.write(reinterpret_cast<char*>(&currentData_), sizeof(GlobalGameData));
        } else {
            //--Set the next level to be unlocked too either way
            currentData_.LevelsUnlocked[StateGame::GetFileIndex(
                currentData_.CurrentLevel)] = 1;
            currentData_.SaveID = saveID;
            std::memcpy(&initialData_, &currentData_, sizeof(GlobalGameData));
            file.write(reinterpret_cast<char*>(&currentData_), sizeof(GlobalGameData));
        }

        file.close();
    }
}
