/*************************************************************************/
/*!
\file		StateGame.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_STATEGAME_H_
#define DESCENSION_STATEGAME_H_

#include "Simian/GameStateManager.h"
#include "Simian/Singleton.h"

#include <string>

namespace Simian
{
    class Scene;
    class Music;
}

namespace Descension
{
    class StateGame: public Simian::GameState, public Simian::Singleton<StateGame>
    {
    private:
        Simian::Scene* scene_;
        std::string levelFile_;
    public:
        const std::string& LevelFile() const;
        void LevelFile(const std::string& val);

        Simian::Scene* Scene() const;
        void Scene(Simian::Scene* val);
    public:
        StateGame();

        static Simian::u32 GetFileIndex(const std::string& fileName);
        static std::string GetFileFromIndex(Simian::u32 id);

        void OnEnter();
        void OnLoad();
        void OnInit();
        void OnUpdate(Simian::f32 dt);
        void OnDeinit();
        void OnUnload();
        void OnExit();
    };
}

#endif
