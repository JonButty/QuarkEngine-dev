/*************************************************************************/
/*!
\file		GCGameData.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/30
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/CTransform.h"
#include "Simian/Scene.h"
#include "Simian/Entity.h"
#include "Simian/DataFileIO.h"
#include "Simian/FileSystem.h"
#include "Simian/Keyboard.h"
#include "Simian/LogManager.h"
#include "Simian/Game.h"

#include "StateGame.h"
#include "StateLoading.h"
#include "EntityTypes.h"
#include "ComponentTypes.h"
#include "Collision.h"

#include "GCGameData.h"
#include "GCAIBase.h"
#include "GCPlayerLogic.h"
#include "Gesture.h"
#include "GCGestureInterpreter.h"
#include "GCPhysicsController.h"
#include "GCUI.h"
#include "GCEditor.h"

#include <fstream>

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCGameData> GCGameData::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_GAMEDATA);
    
    const std::string GCGameData::GAME_DIRECTORY =
                                "/Digipen/Descension/"; //--append to docPath
    GCGameData* GCGameData::instance_ = NULL;

    GCGameData::GCGameData()
        : gameState_(0), saveID_(SAVE_NORMAL)
    {
    }

    Simian::u32 GCGameData::GameState() const
    {
        return gameState_;
    }

    void GCGameData::GameState(Simian::u32 val)
    {
        gameState_ = val;
    }

    Simian::u32 GCGameData::SaveID() const
    {
        return saveID_;
    }

    void GCGameData::SaveID(Simian::u32 val)
    {
        saveID_ = val;
    }

    GCGameData* GCGameData::Instance(void)
    {
        return instance_;
    }

    //--------------------------------------------------------------------------
    void GCGameData::update_( Simian::DelegateParameter& )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        if (Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_CONTROL))
        {
            if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_G))
            {
                GCUI::Instance()->UnloadSkills();
                GCGestureInterpreter::Instance()->SetSkills(0);
                GCUI::Instance()->LoadSkills();
            }
            if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_F))
            {
                GCUI::Instance()->UnloadSkills();
                GCGestureInterpreter::Instance()->SetSkills(15);
                GCUI::Instance()->LoadSkills();
            }
            if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_D))
            {
                GCPlayerLogic::Instance()->Attributes()->ExperienceGoal
                    (GCPlayerLogic::Instance()->Attributes()->MinExperienceGoal());
                GCPlayerLogic::Instance()->Attributes()->MaxMana
                    (GCPlayerLogic::Instance()->Attributes()->MinMaxMana());
                GCPlayerLogic::Instance()->Attributes()->MaxHealth
                    (GCPlayerLogic::Instance()->Attributes()->MinMaxHealth());
                GCPlayerLogic::Instance()->Attributes()->Mana
                    (GCPlayerLogic::Instance()->Attributes()->MaxMana());
                GCPlayerLogic::Instance()->Attributes()->Health
                    (GCPlayerLogic::Instance()->Attributes()->MaxHealth());
                GCPlayerLogic::Instance()->Attributes()->Experience
                    (GCPlayerLogic::Instance()->Attributes()->ExperienceGoal());
            }

            if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_O))
                LoadGameData(1);
            if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_P))
                SaveGameData(1);
        }
    }

    //--------------------------------------------------------------------------
    bool GCGameData::LoadGameData(Simian::u32 saveID, bool loadAll)
    {
        //--Get docPath and save/read
        std::string docPath(Simian::FileSystem::GetDocumentsFolder());
        docPath += GAME_DIRECTORY + "GameData.xml";
        Simian::DataLocation dataLoc(docPath);
        Simian::DataFileIODefault XMLReader;
        XMLReader.Read(dataLoc);
        const Simian::FileObjectSubNode* root = XMLReader.Root();

        //--check file exists
        std::ifstream fstr(docPath);
        if (!fstr.is_open())
        {
            SaveGameData(saveID_);
            return false;
        }
        fstr.close();

        Simian::u32 skill = 0;
        GCUI::Instance()->UnloadSkills();
        const Simian::FileArraySubNode* saveData = root->Array("SaveSlots");
        if (loadAll)
        {
            //--Some preview mode code
        } else {
            const Simian::FileObjectSubNode* save = saveData->Object(saveID);
            Simian::s32 hp = GCPlayerLogic::Instance()->Attributes()->MaxHealth();
            Simian::f32 mp = GCPlayerLogic::Instance()->Attributes()->MaxMana();
            Simian::u32 exp = GCPlayerLogic::Instance()->Attributes()->Experience();
            Simian::u32 maxExp = GCPlayerLogic::Instance()->Attributes()->ExperienceGoal();
            save->Data("GestureSkills", skill, skill);
            save->Data("PrevState", gameState_, gameState_);
            save->Data("MaxHP", hp, hp);
            save->Data("MaxMP", mp, mp);
            save->Data("Exp", exp, exp);
            save->Data("MaxExp", maxExp, maxExp);
            GCGestureInterpreter::Instance()->SetSkills(skill);
            GCPlayerLogic::Instance()->Attributes()->MaxHealth(hp);
            GCPlayerLogic::Instance()->Attributes()->MaxMana(mp);
            GCPlayerLogic::Instance()->Attributes()->Experience(exp);
            GCPlayerLogic::Instance()->Attributes()->ExperienceGoal(maxExp);
            GCPlayerLogic::Instance()->Attributes()->Health(hp);
            GCPlayerLogic::Instance()->Attributes()->Mana(mp);

            if (gameState_ == GAMEDATA_LOADCHECKPOINT) //--move to position
            {
                Simian::f32 x=0, z=0;
                save->Data("PlayerX", x, x);
                save->Data("PlayerZ", z, z);
                GCPlayerLogic::Instance()->Physics()->Enabled(false);
                GCPlayerLogic::Instance()->Transform().ParentTransform()->
                    Position(Simian::Vector3(x, 0, z));
                GCPlayerLogic::Instance()->Physics()->Enabled(true);
            }

            gameState_ = GAMEDATA_TRANSITING;
            if (saveID == SAVE_CHECKPOINT) //--load checkpoint
            {
                std::string levelFile;
                gameState_ = GAMEDATA_LOADCHECKPOINT;
                save->Data("LevelFile", levelFile, levelFile);
                Simian::Game::Instance().GameStateManager().ChangeState(
                    StateLoading::InstancePtr());
                StateLoading::Instance().NextState(StateGame::InstancePtr());
                StateGame::Instance().LevelFile(levelFile);
            }
        }
        GCUI::Instance()->LoadSkills();
        return true;
    }

    bool GCGameData::SaveGameData(Simian::u32 saveID, bool usePosVec,
        const Simian::Vector3& position)
    {
        //--Get docPath and save/read
        std::string docPath(Simian::FileSystem::GetDocumentsFolder());
        docPath += GAME_DIRECTORY + "GameData.xml";

        //--check file exists
        std::ifstream fstr(docPath);
        if (!fstr.is_open())
            CreateNewGameData(docPath);
        else
            fstr.close();
        Simian::DataLocation dataLoc(docPath);
        Simian::DataFileIODefault XMLReader;
        XMLReader.Read(dataLoc);
        Simian::FileObjectSubNode* root = XMLReader.Root();
        Simian::FileArraySubNode* saveData = root->Array("SaveSlots");
        Simian::FileObjectSubNode* save = saveData->Object(saveID);
        save->AddData("ID", saveID);
        save->AddData("GestureSkills",
            GCGestureInterpreter::Instance()->GetSkills().Skills());
        save->AddData("PrevState", gameState_);
        save->AddData("MaxHP", GCPlayerLogic::Instance()->Attributes()->MaxHealth());
        save->AddData("MaxMP", GCPlayerLogic::Instance()->Attributes()->MaxMana());
        save->AddData("Exp", GCPlayerLogic::Instance()->Attributes()->Experience());
        save->AddData("MaxExp", GCPlayerLogic::Instance()->Attributes()->ExperienceGoal());

        if (saveID == SAVE_CHECKPOINT)
        {
            save->AddData("LevelFile", StateGame::Instance().LevelFile());

            //save->AddData("PlayerX", GCPlayerLogic::Instance()->Physics()->Position().X());
            //save->AddData("PlayerZ", GCPlayerLogic::Instance()->Physics()->Position().Z());

            save = saveData->Object(SAVE_NORMAL);
            if (!usePosVec)
            {
                save->AddData("PlayerX", GCPlayerLogic::Instance()->Physics()->Position().X());
                save->AddData("PlayerZ", GCPlayerLogic::Instance()->Physics()->Position().Z());
            } else {
                save->AddData("PlayerX", position.X());
                save->AddData("PlayerZ", position.Z());
            }
        }

        XMLReader.Write(docPath);
        return true;
    }

    void GCGameData::CreateNewGameData(std::string& docPath)
    {
        Simian::DataFileIODefault XMLWriter;

        Simian::FileObjectSubNode* root = XMLWriter.Root();
        root->Name("GameData");
        Simian::FileArraySubNode* saveData = root->AddArray("SaveSlots");
        Simian::FileObjectSubNode* save = saveData->AddObject("Save");
        save->AddData("ID", SAVE_NORMAL);
        save->AddData("GestureSkills",
            GCGestureInterpreter::Instance()->GetSkills().Skills() );
        save->AddData("PrevState", GAMEDATA_NEW);
        save->AddData("MaxHP", GCPlayerLogic::Instance()->Attributes()->MinMaxHealth());
        save->AddData("MaxMP", GCPlayerLogic::Instance()->Attributes()->MinMaxMana());
        save->AddData("Exp", 0);
        save->AddData("MaxExp", GCPlayerLogic::Instance()->Attributes()->MinExperienceGoal());

        save = saveData->AddObject("Save");
        save->AddData("ID", SAVE_CHECKPOINT);
        save->AddData("GestureSkills",
            GCGestureInterpreter::Instance()->GetSkills().Skills() );
        save->AddData("PrevState", GAMEDATA_NEW);
        save->AddData("MaxHP", GCPlayerLogic::Instance()->Attributes()->MinMaxHealth());
        save->AddData("MaxMP", GCPlayerLogic::Instance()->Attributes()->MinMaxMana());
        save->AddData("Exp", 0);
        save->AddData("MaxExp", GCPlayerLogic::Instance()->Attributes()->MinExperienceGoal());
        save->AddData("PlayerX", GCPlayerLogic::Instance()->Physics()->Position().X());
        save->AddData("PlayerZ", GCPlayerLogic::Instance()->Physics()->Position().Z());
        save->AddData("LevelFile", StateGame::Instance().LevelFile());

        XMLWriter.Write(docPath);
    }

    void GCGameData::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate
            <GCGameData, &GCGameData::update_>(this));
    }

    void GCGameData::OnAwake()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        instance_ = this;
    }

    void GCGameData::OnInit()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        LoadGameData(saveID_);
    }

    void GCGameData::OnDeinit()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        SaveGameData(saveID_); //--Transferred to script
    }

    void GCGameData::Serialize( Simian::FileObjectSubNode& )
    {
        //--nothing
    }

    void GCGameData::Deserialize( const Simian::FileObjectSubNode& )
    {
        //--nothing
    }
}

