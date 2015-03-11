/*************************************************************************/
/*!
\file		StateEditor.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_STATEEDITOR_H_
#define DESCENSION_STATEEDITOR_H_

#include "Simian/Singleton.h"
#include "Simian/GameStateManager.h"
#include "Simian/DataLocation.h"
#include "Simian/Vector3.h"

namespace Simian
{
    class Scene;
}

namespace Descension
{
    class StateEditor: public Simian::GameState, public Simian::Singleton<StateEditor>
    {
    private:
        Simian::Scene* scene_;
        Simian::DataLocation location_;
        Simian::Vector3 lastCameraPosition_;
        Simian::Vector3 playerPosition_;
    public:
        Simian::Scene* Scene() const;
        void Location(const Simian::DataLocation& val);

        const Simian::Vector3& LastCameraPosition() const;
        void LastCameraPosition(const Simian::Vector3& val);
    public:
        StateEditor();

        void OnEnter();
        void OnLoad();
        void OnInit();
        void OnUpdate(Simian::f32 dt);
        void OnDeinit();
        void OnUnload();
        void OnExit();

        void NewMap(Simian::u32 width, Simian::u32 height);
        void LoadMap(const Simian::DataLocation& data);
        void SaveMap(const std::string& path);
        void ReplacePlayerMarker();
        void ReplacePlayer();

        void SetCursorVisibility(bool visible);

        // sets camera to player's position
        void SetCameraPosition();
        // resets it back to its original position
        void RevertCameraPosition();
    };
}

#endif
