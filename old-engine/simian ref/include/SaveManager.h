/*************************************************************************/
/*!
\file		SaveManager.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/11/30
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/

#ifndef DESCENSION_SAVEMANAGER_H_
#define DESCENSION_SAVEMANAGER_H_

#include "Simian/Singleton.h"
#include "Simian/Vector2.h"

#include <string>

namespace Descension
{
    static const Simian::u32 SAVE_NUM_CRYSTALS = 32;
    static const Simian::u32 SAVE_NUM_LEVELSUNLOCKED = 16;
    static const Simian::u32 SAVE_MAX_FILENAME = 44;

    class SaveManager: public Simian::Singleton<SaveManager>
    {
    public:
        static const std::string SAVE_DIRECTORY;

        enum SAVEID_TYPE
        {
            SAVE_INVALID = 0, //--Invalidated file
            SAVE_NORMAL,      //--load from file
            SAVE_CHECKPOINT,  //--Check initialData before loading
        };

        // save all relevant data
        struct GlobalGameData 
        {
            //--Array: marked as 1 if unlocked/destroyed
            Simian::u8 SkillsUnlocked;
            Simian::u8 Checkpoint;
            Simian::u8 Crystals[SAVE_NUM_CRYSTALS];
            Simian::u8 LevelsUnlocked[SAVE_NUM_LEVELSUNLOCKED];
            Simian::s8 CurrentLevel[SAVE_MAX_FILENAME];
            Simian::s32 HP;
            Simian::f32 MP;
            Simian::u32 Exp;
            Simian::u32 SaveID;
            Simian::Vector2 Position;
            Simian::u8 Reserved[2000];
        };
    private:
        GlobalGameData initialData_, currentData_;
    public:
        Descension::SaveManager::GlobalGameData& CurrentData()
                                { return currentData_; }
        Descension::SaveManager::GlobalGameData& InitialData()
                                { return initialData_; }
        //--Extract as string
        std::string GetCurrentLevel(const GlobalGameData& data);
        void SetCurrentLevel(const std::string& levelName,
                             GlobalGameData& data);
    public:
        SaveManager();

        //--
        void UpdateCurrentData(void);
        void CopyInitialToCurrent(void);

        void LoadCheckpoint(bool userCurrentData = false);

        void LoadData(void);
        void SaveData(Simian::u32 saveID);
    };
}

#endif
