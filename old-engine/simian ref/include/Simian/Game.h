/************************************************************************/
/*!
\file		Game.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_GAME_H_
#define SIMIAN_GAME_H_

#include "SimianPlatform.h"
#include "Singleton.h"
#include "Observer.h"
#include "Vector2.h"

namespace Simian
{
    typedef void* WindowHandle;

    class Window;

    class FileLogger;
    class LoggerBase;
    class GraphicsDevice;
    class RenderQueue;
    class TextureCache;
    class ShaderCache;
    class MaterialCache;
    class ModelCache;
    class Viewport;
    class VertexBuffer;
    class DataLocation;

    class Keyboard;
    class Mouse;

    class GameStateManager;
    class GameState;

    class FrameRateController;
    class FileObjectSubNode;

    /** Interface for the Game observer. */
    class SIMIAN_EXPORT GameListener
    {
    public:
        enum SubsystemError
        {
            SE_NONE,
            SE_WINDOW,
            SE_GRAPHICS,
            SE_AUDIO,
            SE_INPUT,
            SE_GSM
        };
    public:
        /** Virtual destructor for inheritance. */
        virtual ~GameListener() {}

        /** Setup the graphics device to your liking here. */
        virtual void OnInitGraphics() = 0;

        /** Setup the this happens after all subsystems are setup. 
            @remarks
                This would be a good place to set the initial game state and 
                game factory's init.
        */
        virtual void OnPostInit(SubsystemError error) = 0;

        /** Called when the game is shutting down.
            @remarks
                Called before the engine subsystems are shutdown. This would be
                a good place to deinit any user systems initialized in 
                GameListener::OnPostInit.
        */
        virtual void OnPreShutdown() = 0;

        /** Called every frame before the scene is rendered. */
        virtual void OnBeginUpdate(f32 dt) = 0;

        /** Called every frame after the scene is rendered. */
        virtual void OnEndUpdate(f32 dt) = 0;

        /** Called when settings are loaded. */
        virtual void OnSettingsLoad(const FileObjectSubNode& settings) = 0;

        /** Called when a settings are saved. */
        virtual void OnSettingsSave(FileObjectSubNode& settings) = 0;
    };

    class SIMIAN_EXPORT Game: public Observer<GameListener>, public Singleton<Game>
    {
    private:
        // logging system
        LoggerBase* stdOutLogger_;
        FileLogger* fileLogger_;

        // the actual window
        WindowHandle windowHandle_;
        Window* window_;

        // graphics systems
        GraphicsDevice* graphicsDevice_;
        RenderQueue* renderQueue_;
        TextureCache* textureCache_;
        ShaderCache* shaderCache_;
        MaterialCache* materialCache_;
        ModelCache* modelCache_;

        TextureCache* debugTextureCache_;
        ShaderCache* debugShaderCache_;
        MaterialCache* debugMaterialCache_;

        Viewport* viewport_;
        VertexBuffer* fullscreenQuad_;

#if SIMIAN_FRAMERATECONTROLLER == 1
        FrameRateController* frameRateController_;
#endif

        // input system
        Keyboard* keyboard_;
        Mouse* mouse_;

        // state system
        GameStateManager* gameStateManager_;
        GameState* initialState_;

        // other data
        std::string gameTitle_;
        Vector2 size_;
        bool fullscreen_;
        bool vsync_;
        u64 lastTime_;
        f32 timeMultiplier_;
        f32 frameTime_;
        bool exiting_;
        bool windowVisible_;

        u32 icon_;
    public:
        Simian::Window& Window();
        
        Simian::GraphicsDevice& GraphicsDevice() const;
        
        Simian::RenderQueue& RenderQueue() const;
        
        Simian::TextureCache& TextureCache() const;

        Simian::ShaderCache& ShaderCache() const;
        
        Simian::MaterialCache& MaterialCache() const;
        
        Simian::ModelCache& ModelCache() const;

        Simian::Viewport& Viewport() const;

        Simian::GameStateManager& GameStateManager();
        
        const std::string& GameTitle() const;
        void GameTitle(const std::string& val);

        Simian::u32 Icon() const;
        void Icon(Simian::u32 val);

        const Simian::Vector2& Size() const;
        void Size(const Simian::Vector2& val);

        bool Fullscreen() const;
        void Fullscreen(bool val);

        bool Vsync() const;
        void Vsync(bool val);

        bool Exiting() const { return exiting_; }

        Simian::f32 TimeMultiplier() const;
        void TimeMultiplier(Simian::f32 val);

        Simian::f32 FrameTime() const;
        void FrameTime(Simian::f32 val);

        VertexBuffer* FullscreenQuad() const;
    private:
        GameListener::SubsystemError initSubsystems_();
        void update_();
        void unload_();
    public:
        Game();
        ~Game();
        
        void LoadCaches();
        void UnloadCaches();

        void LoadSettings(const DataLocation& data);
        void SaveSettings(const std::string& filePath);

        void Start(GameState* initialState);
        void Exit();

        // these functions are for full control of the game loop (not for the 
        // feint of heart, but perfect for the editor programmer ;) )
        void Init(GameState* initialState, WindowHandle handle = 0);
        void Update();
        void DrawFrame();
        void Shutdown();
    };
}

#endif
