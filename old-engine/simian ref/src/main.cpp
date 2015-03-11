/*****************************************************************************/
/*!
\file		main.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 20, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifdef _DEBUG
#include <vld.h>
#endif

#include "Console.h"

#include "Simian/SimianPlatform.h"
#include "Simian/Game.h"
#include "Simian/EnginePhases.h"
#include "Simian/GameFactory.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/LogManager.h"
#include "Simian/OS.h"
#include "Simian/FileObjectSubNode.h"
#include "Simian/AudioManager.h"
#include "Simian/FileDataSubNode.h"

#include "EffectCreator.h"
#include "StateGame.h"
#include "StateEditor.h"
#include "StateLoading.h"
#include "ComponentTypes.h"
#include "EntityTypes.h"
#include "AIEffectTypes.h"
#include "ScriptingSystem.h"
#include "../resources/resource.h"

#include <sstream>

#if SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

std::string USER_FILES_PATH = "";

class DescensionGame: public Simian::GameListener
{
private:
    Simian::f32 musicVolume_;
    Simian::f32 soundVolume_;
public:
    DescensionGame()
        : musicVolume_(1.0f), soundVolume_(1.0f) {}

    void OnInitGraphics()
    {
		//Simian::Game::Instance().GraphicsDevice().VSync(true);

        // editor/game specific settings
#ifdef _DESCENDED
        Simian::Game::Instance().GraphicsDevice().Fullscreen(false);
#endif
    }

    void OnPostInit( SubsystemError error ) 
    {
        if (error != SE_NONE)
        {
            std::stringstream ss;
            ss << "An error occurred with ";

            if (error == SE_WINDOW)
                ss << "the window!";
            else if (error == SE_GRAPHICS)
                ss << "the graphics subsystem!";
            else if (error == SE_AUDIO)
                ss << "the audio subsystem!";
            else if (error == SE_INPUT)
                ss << "the input subsystem!";
            else if (error == SE_GSM)
                ss << "the game state manager!";

#if SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32
            MessageBox(NULL, ss.str().c_str(), "Error", 0);
#endif
        }

        // set music and sound volumes
        Simian::AudioManager::Instance().MusicVolume(musicVolume_);
        Simian::AudioManager::Instance().SoundVolume(soundVolume_);

        // load game specific components
        std::vector<Simian::DataLocation> components;
        components.push_back("Data/EngineComponents.h");
        components.push_back("Data/ComponentTypes.h");
        std::vector<Simian::DataLocation> entities;
        entities.push_back("Data/EntityTypes.h");
        Simian::GameFactory::Instance().Init(Simian::P_TOTAL, Descension::GC_TOTAL, Descension::E_TOTAL, components, entities);

        // initialize effects factory
        std::vector<Simian::DataLocation> effects;
        effects.push_back("Data/AIEffectTypes.h");
        Descension::EffectCreator::Instance().Init(Descension::EFF_TOTAL, effects);

        // initialize scripting system
        Descension::ScriptingSystem::Instance().Init();
    }

    void OnPreShutdown()
    {
        Descension::ScriptingSystem::Instance().Deinit();
    }

    void OnBeginUpdate( Simian::f32 ) 
    {
    }

    void OnEndUpdate( Simian::f32 ) 
    {
    }

    void OnSettingsLoad( const Simian::FileObjectSubNode& settings ) 
    {
        settings.Data("MusicVolume", musicVolume_, 1.0f);
        settings.Data("SoundVolume", soundVolume_, 1.0f);
    }

    void OnSettingsSave( Simian::FileObjectSubNode& settings ) 
    {
        settings.AddData("MusicVolume", Simian::AudioManager::Instance().MusicVolume());
        settings.AddData("SoundVolume", Simian::AudioManager::Instance().SoundVolume());
    }
};

// entry point
#if defined(_DESCENDED)
extern "C"
{
Simian::WindowHandle __declspec(dllimport) createWindowHandle_(HWND hwnd);
int __declspec(dllexport) __stdcall StartGame(HWND hwnd)
#elif SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#else
int main(int argc, char* arg[])
#endif
{
    OpenConsoleWindow();

#ifndef _DESCENDED
    DescensionGame descension;
    Simian::Game::Instance().AddListener(&descension);
#endif

    Simian::Game::Instance().GameTitle("Descension: Depths of De'mae");
    Simian::Game::Instance().Icon(IDI_ICON1);

    // make sure the folder for settings exists.
    std::string path = Simian::OS::Instance().GetSpecialFolderPath(Simian::OS::SF_DOCUMENTS);
    path.append("/DigiPen");
    Simian::OS::Instance().DirectoryCreate(path);
    path.append("/Descension");
    Simian::OS::Instance().DirectoryCreate(path);
    path.append("/");
    USER_FILES_PATH = path;

    // check if compiling for the editor
#ifdef _DESCENDED
    Simian::Game::Instance().AddListener(new DescensionGame());
    Simian::Game::Instance().Size(Simian::Vector2(1920, 1080));
    Simian::WindowHandle handle = createWindowHandle_(hwnd);
    Simian::Game::Instance().Init(Descension::StateEditor::InstancePtr(), handle);
    delete handle;
#else
    Simian::OS::Instance().CursorSetVisibility(false);

    Simian::Game::Instance().LoadSettings(path + "Settings.xml");
    Descension::StateLoading::Instance().BackgroundColor(Simian::Color(0.0f, 0.0f, 0.0f, 1.0f));
    Descension::StateLoading::Instance().NextState(Descension::StateGame::InstancePtr());
    Simian::Game::Instance().Start(Descension::StateLoading::InstancePtr());
#endif

    return 0;
}
#if defined(_DESCENDED)
}
#endif
