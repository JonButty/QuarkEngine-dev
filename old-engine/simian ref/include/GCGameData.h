/*************************************************************************/
/*!
\file		GCGameData.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/09/05
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCGAMEDATA_H_
#define DESCENSION_GCGAMEDATA_H_

#include "Simian/EntityComponent.h"
#include <string>

namespace Descension
{
    class GCGameData: public Simian::EntityComponent
    {
    public:
        static const std::string GAME_DIRECTORY;

        enum SAVEID_TYPE
        {
            SAVE_NORMAL = 0,
            SAVE_CHECKPOINT,
            SAVEID_TOTAL
        };

        enum GAMEDATA_STATE
        {
            GAMEDATA_NEW = 0,
            GAMEDATA_TRANSITING,
            GAMEDATA_LOADCHECKPOINT,
            GAMEDATA_EXITING,
        };
    private:
        Simian::u32 gameState_, saveID_;

        static GCGameData* instance_;
        // factory plant
        static Simian::FactoryPlant<Simian::EntityComponent, GCGameData> factoryPlant_;
    public:
        Simian::u32 GameState() const;
        void GameState(Simian::u32 val);
        Simian::u32 SaveID() const;
        void SaveID(Simian::u32 val);

        static GCGameData* Instance(void);
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCGameData();

        bool LoadGameData(Simian::u32 saveID, bool LoadAll = false);
        bool SaveGameData(Simian::u32 saveID, bool usePosVec = false,
                          const Simian::Vector3& position = Simian::Vector3());
        void CreateNewGameData(std::string& docPath);

        void OnSharedLoad();
        void OnAwake();
        void OnInit();
        void OnDeinit();

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
