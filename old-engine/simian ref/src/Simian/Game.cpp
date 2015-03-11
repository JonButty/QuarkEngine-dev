/************************************************************************/
/*!
\file		Game.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Game.h"
#include "Simian/Window.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/AudioManager.h"
#include "Simian/InputManager.h"
#include "Simian/Keyboard.h"
#include "Simian/Mouse.h"
#include "Simian/GameStateManager.h"
#include "Simian/TextureCache.h"
#include "Simian/MaterialCache.h"
#include "Simian/ModelCache.h"
#include "Simian/RenderQueue.h"
#include "Simian/GameFactory.h"
#include "Simian/OS.h"
#include "Simian/Viewport.h"
#include "Simian/LogManager.h"
#include "Simian/FileLogger.h"
#include "Simian/FrameRateController.h"
#include "Simian/DebugRenderer.h"
#include "Simian/Camera.h"
#include "Simian/RenderTexture.h"
#include "Simian/InstrumentedProfiler.h"
#include "Simian/EngineWatches.h"
#include "Simian/DataFileIO.h"
#include "Simian/VertexBuffer.h"
#include "Simian/ShaderCache.h"

#include <sstream>
#include "Simian/CompositorChain.h"

namespace Simian
{
    static const f32 FRAME_CAP = 0.1f;

    Game::Game()
        : stdOutLogger_(0),
          fileLogger_(0),
          windowHandle_(0),
          window_(0),
          graphicsDevice_(0),
          renderQueue_(0),
          textureCache_(0),
          materialCache_(0),
          modelCache_(0),
          viewport_(0),
          fullscreenQuad_(0),
          keyboard_(0),
          mouse_(0),
          gameStateManager_(0),
          initialState_(0),
          gameTitle_("Simian Engine Game"),
          fullscreen_(false),
          vsync_(false),
          lastTime_(0),
          timeMultiplier_(1.0f),
          frameTime_(0.0f),
          exiting_(false),
          windowVisible_(true),
          icon_(0)
    {
    }

    Game::~Game()
    {
    }

    //--------------------------------------------------------------------------

    Simian::Window& Game::Window()
    {
        return *window_;
    }

    Simian::GraphicsDevice& Game::GraphicsDevice() const
    {
        return *graphicsDevice_;
    }

    Simian::RenderQueue& Game::RenderQueue() const
    {
        return *renderQueue_;
    }

    Simian::TextureCache& Game::TextureCache() const
    {
        return *textureCache_;
    }

    Simian::ShaderCache& Game::ShaderCache() const
    {
        return *shaderCache_;
    }

    Simian::MaterialCache& Game::MaterialCache() const
    {
        return *materialCache_;
    }

    Simian::ModelCache& Game::ModelCache() const
    {
        return *modelCache_;
    }

    Simian::GameStateManager& Game::GameStateManager()
    {
        return *gameStateManager_;
    }

    const std::string& Game::GameTitle() const
    {
        return gameTitle_;
    }

    void Game::GameTitle( const std::string& val )
    {
        gameTitle_ = val;
    }

    Simian::u32 Game::Icon() const
    {
        return icon_;
    }

    void Game::Icon( Simian::u32 val )
    {
        icon_ = val;
    }

    const Simian::Vector2& Game::Size() const
    {
        return size_;
    }

    void Game::Size( const Simian::Vector2& val )
    {
        size_ = val;

        if (graphicsDevice_)
            graphicsDevice_->Size(size_);

        if (viewport_)
            viewport_->Size(size_);
    }

    void Game::Fullscreen( bool val )
    {
        fullscreen_ = val;

        if (graphicsDevice_)
            graphicsDevice_->Fullscreen(fullscreen_);
    }

    bool Game::Fullscreen() const
    {
        return fullscreen_;
    }

    bool Game::Vsync() const
    {
        return vsync_;
    }

    void Game::Vsync( bool val )
    {
        vsync_ = val;

        if (graphicsDevice_)
            graphicsDevice_->VSync(vsync_);
    }

    Simian::Viewport& Game::Viewport() const
    {
        return *viewport_;
    }

    Simian::f32 Game::TimeMultiplier() const
    {
        return timeMultiplier_;
    }

    void Game::TimeMultiplier( Simian::f32 val )
    {
        timeMultiplier_ = val;
    }

    Simian::f32 Game::FrameTime() const
    {
        return frameTime_;
    }

    void Game::FrameTime( Simian::f32 val )
    {
        frameTime_ = val;
    }

    VertexBuffer* Game::FullscreenQuad() const
    {
        return fullscreenQuad_;
    }

    //--------------------------------------------------------------------------

    GameListener::SubsystemError Game::initSubsystems_()
    {
#if SIMIAN_LOGGING == 1
        LogManager::Instance().CreateInstance(LogManager::LT_STDOUT, stdOutLogger_);
        LogManager::Instance().CreateInstance(LogManager::LT_FILE, fileLogger_);
        fileLogger_->Load("Simian.log");
#endif
        // next level ascii art
        std::stringstream ss;
        ss << "Welcome to " SIMIAN_ENGINE_NAME " \"" SIMIAN_ENGINE_REVISION_NAME "\" " SIMIAN_ENGINE_VERSION << std::endl;
        ss << "                         $                                                     \n"
              "                      MI~+O                                                    \n"
              "                     M7:~=+$O                                                  \n" 
              "                 ~M~:~==+?IZO                                                  \n" 
              "              ,M+:~~=+?II7Z8O                                      +DM  MO7    \n" 
              "           ?M::~==+?II$O8MM                                       M++I8M=+?M   \n" 
              "        ?M,:~==+?II$Z8MM                                          M=IZ8M?7OM   \n" 
              "      M=~===+?II$O8MD       MI                      O?M            7OM  MZD    \n" 
              "       M=+I???II87        ,D+$  MIM   M=M   M=$8   M+?M   M+7M:    M~M  M~M    \n" 
              "        7O=?I??+?$M      IZ??$ M~~=MMI~~==MM~~+7M MI?7M=M:~+=::~M7O~~~=M~~?ZM  \n" 
              "          M~+I??+=IZM   =++=~I++=+++?+++==??++?+~MI++?M+=++=I?++M?=~++=++++?+M \n" 
              "            7=?I?+=?ZM  =,:=?$M==?I$M?=+?7NM?+??+M,~??M,:+~:~~?IMM:~?I77????$M \n" 
              "             M~+I??+=IOM=::=?$M,~??7M+:=?7MM::+?+M,:??M,:+~:,:?IMM,~??$M~~+?$M \n" 
              "               M~=???I7IM~:=?$M,~??7M+:=?IMM,:+?+M,:??M,:+~:,:?IMM,~??7M,:+?$M \n" 
              "             M=+=+?I7OMMM~:=?$M,~??7M+:=?IMM,:+?+M,:??M,:+~:,:?IMM,~??7M,:+?$M \n" 
              "           ~+===?I7Z8M  =,:=?$M,~??7M+:=?IMM,:+?+M,:??M,:++~::?IMM,~??7M,:+?$M \n" 
              "          M~==+?I$OM~   =,:=?$M,~??7M+:=?IMM,:+?+M,:??M,:+?=~~?+MM,~??7M,:+?$M \n" 
              "        87===?I7ODM     =,:+$ZM,~?I$M+:+IZMM,:++~M:~I$M,:?I?+?I?+M,~?I$M,:+?$M \n" 
              "       M~==+I7$8M       =,~78MM,=7ZO8+:IO8+M,:?I+M=+OMO=?Z8DM7$OMM:~IZOM,:?$8M \n" 
              "    M+:~==?I$OM~        =~78Z M:78M  ?78M  M=+I$ZM?ZM O7MN   ~DM M~78M  :?OM+  \n" 
              "    M~~=+I7$8M          =ID=  M7D8   $N+    MIO8MM7M             M78M   IOM    \n" 
              "    M,:?I$OM             M    MM             +M  ZM              MM     M      \n" 
              "    M,~7ODM                                                                    \n" 
              "    M~78M                                                                      \n" 
              "    M7M~                                                                       \n" 
              "    MZ                                                                         \n" 
              "     .                                                                           ";

        SVerbose(ss.str());
        SVerbose("====== Beginning subsystem initialization =======");

        if (window_)
        {
            SVerbose("Starting window...");
            if (!window_->Startup())
                return GameListener::SE_WINDOW;
            windowHandle_ = window_->Handle();
            SVerbose("Window started successfully!");
        }

        SVerbose("Starting graphics device...");
        if (window_)
        {
            if (!graphicsDevice_->Startup(*window_))
            {
                window_->Shutdown();
                return GameListener::SE_GRAPHICS;
            }
        }
        else if (!graphicsDevice_->Startup(windowHandle_))
            return GameListener::SE_GRAPHICS;
        SVerbose("Graphics device started successfully!");

        // create the fullscreen quad
        graphicsDevice_->CreateVertexBuffer(fullscreenQuad_);
        fullscreenQuad_->Load(PositionColorUVVertex::Format, 4);
        PositionColorUVVertex vertices[4] = {
            PositionColorUVVertex(Simian::Vector3(-1, -1, 0), Simian::Color(), Simian::Vector2(0, 1)),
            PositionColorUVVertex(Simian::Vector3(1, -1, 0), Simian::Color(), Simian::Vector2(1, 1)),
            PositionColorUVVertex(Simian::Vector3(-1, 1, 0), Simian::Color(), Simian::Vector2(0, 0)),
            PositionColorUVVertex(Simian::Vector3(1, 1, 0), Simian::Color(), Simian::Vector2(1, 0)),
        };
        fullscreenQuad_->Data(vertices);
            
        SVerbose("Creating viewport...");
        graphicsDevice_->CreateViewport(viewport_);
        viewport_->Load();
        viewport_->Size(size_);
        viewport_->Set();
        SVerbose("Viewport created and set!");

        SVerbose("Starting audio manager...");
        if (!AudioManager::Instance().Startup(windowHandle_))
        {
            graphicsDevice_->Shutdown();
            window_->Shutdown();
            return GameListener::SE_AUDIO;
        }
        SVerbose("Audio manager started successfully!");

        SVerbose("Creating input devices...");
        InputManager::Instance().CreateInstance(InputManager::DID_KEYBOARD, keyboard_);
        InputManager::Instance().CreateInstance(InputManager::DID_MOUSE, mouse_);
        mouse_->WinHand(windowHandle_);

        if (!keyboard_ || !mouse_)
        {
            if (!mouse_)
                InputManager::Instance().DestroyInstance(mouse_);

            AudioManager::Instance().Shutdown();
            graphicsDevice_->Shutdown();
            window_->Shutdown();

            return GameListener::SE_INPUT;
        }
        SVerbose("Input devices created successfully!");

        SVerbose("Creating game state manager...");
        gameStateManager_ = new Simian::GameStateManager(initialState_);

        if (!gameStateManager_)
        {
            InputManager::Instance().DestroyInstance(mouse_);
            InputManager::Instance().DestroyInstance(keyboard_);

            AudioManager::Instance().Shutdown();
            graphicsDevice_->Shutdown();
            window_->Shutdown();

            return GameListener::SE_GSM;
        }
        SVerbose("Game state manager created successfully!");

#if SIMIAN_FRAMERATECONTROLLER == 1
        SVerbose("Creating framerate controller...");
        frameRateController_ = new FrameRateController();
        frameRateController_->MaterialCache(debugMaterialCache_);
        frameRateController_->GraphicsDevice(graphicsDevice_);
        frameRateController_->Load();
        SVerbose("Framerate controller created successfully!");
#endif

#if SIMIAN_DRAWDEBUGSHAPES == 1
        SVerbose("Creating debug renderer...");
        DebugRenderer::Instance().Device(graphicsDevice_);
        DebugRenderer::Instance().RenderQueue(renderQueue_);
        DebugRenderer::Instance().MaterialCache(debugMaterialCache_);
        DebugRendererLoad();
        SVerbose("Framerate controller created successfully!");
#endif
        
        // load the debug textures
        debugTextureCache_->LoadFiles();
        debugTextureCache_->LoadTextures();

#if SIMIAN_ATTACHPROFILER == 1
        SVerbose("Creating profiler...");
        SProfilerInit(WATCH_TOTAL);
        SProfilerLoad(graphicsDevice_, debugMaterialCache_);
        SVerbose("Profiler created successfully!");
#endif

        SVerbose("===== Subsystems initialized successfully! ======");

        return GameListener::SE_NONE;
    }

    void Game::update_()
    {
        while (*window_ && !exiting_)
        {
            if (windowVisible_ && !window_->Visible())
            {
                windowVisible_ = false;
                Simian::AudioManager::Instance().Mute();
            }
            else if (!windowVisible_ && window_->Visible())
            {
                windowVisible_ = true;
                Simian::AudioManager::Instance().Unmute();
            }

            if (window_->Update() && window_->Visible())
                Update();
        }
    }

    void Game::unload_()
    {
        SVerbose("=========== Shutting subsystems down ============");

#if SIMIAN_ATTACHPROFILER == 1
        SVerbose("Destroying profiler...");
        SProfilerUnload();
#endif

#if SIMIAN_DRAWDEBUGSHAPES == 1
        SVerbose("Destroying debug renderer...");
        DebugRendererUnload();
#endif

#if SIMIAN_FRAMERATECONTROLLER == 1
        SVerbose("Destroying framerate controller...");
        frameRateController_->Unload();
        delete frameRateController_;
        frameRateController_ = 0;
#endif

        SVerbose("Destroying game state manager...");
        delete gameStateManager_;
        gameStateManager_ = 0;

        SVerbose("Destroying input devices...");
        InputManager::Instance().DestroyInstance(mouse_);
        mouse_ = 0;
        InputManager::Instance().DestroyInstance(keyboard_);
        keyboard_ = 0;

        SVerbose("Shutting audio manager down...");
        AudioManager::Instance().UnloadAll();
        AudioManager::Instance().Shutdown();

        SVerbose("Destroying render queue...");
        delete renderQueue_;
        renderQueue_ = 0;

        SVerbose("Destroying model cache...");
        modelCache_->UnloadAll();
        delete modelCache_;
        modelCache_ = 0;

        SVerbose("Destroying material cache...");
        materialCache_->UnloadAll();
        delete materialCache_;
        materialCache_ = 0;
        debugMaterialCache_->UnloadAll();
        delete debugMaterialCache_;
        debugMaterialCache_ = 0;

        SVerbose("Destroying texture cache...");
        textureCache_->UnloadAll();
        delete textureCache_;
        textureCache_ = 0;
        debugTextureCache_->UnloadAll();
        delete debugTextureCache_;
        debugTextureCache_ = 0;

        SVerbose("Destroying shader cache...");
        shaderCache_->UnloadAll();
        delete shaderCache_;
        shaderCache_ = 0;
        debugShaderCache_->UnloadAll();
        delete debugShaderCache_;
        debugShaderCache_ = 0;

        SVerbose("Unsetting and removing viewport...");
        viewport_->Unset();
        viewport_->Unload();
        graphicsDevice_->DestroyViewport(viewport_);

        fullscreenQuad_->Unload();
        graphicsDevice_->DestroyVertexBuffer(fullscreenQuad_);

        SVerbose("Shutting graphics device down...");
        graphicsDevice_->Shutdown();
        delete graphicsDevice_;
        graphicsDevice_ = 0;

        if (window_)
        {
            SVerbose("Shutting window down...");
            window_->Shutdown();
            delete window_;
            window_ = 0;
        }

        SVerbose("===== Subsystems deinitialized successfully! ====");
        SVerbose("Good bye! ;)");

#if SIMIAN_LOGGING == 1
        fileLogger_->Unload();
        LogManager::Instance().DestroyInstance(fileLogger_);
        LogManager::Instance().DestroyInstance(stdOutLogger_);
#endif
    }

    //--------------------------------------------------------------------------

    void Game::LoadCaches()
    {
        textureCache_->LoadFiles();
        textureCache_->LoadTextures();
        
        shaderCache_->LoadShaders();

        modelCache_->LoadFiles();
        modelCache_->LoadMeshes();
    }

    void Game::UnloadCaches()
    {
        modelCache_->UnloadAll();
        materialCache_->UnloadAll();
        textureCache_->UnloadAll();
        shaderCache_->UnloadAll();
    }

    void Game::LoadSettings( const DataLocation& data )
    {
        DataFileIODefault settings;
        settings.Read(data);

        Simian::FileObjectSubNode* root = settings.Root();
        float width, height;

        root->Data("Width", width, 0);
        root->Data("Height", height, 0);
        root->Data("Fullscreen", fullscreen_, true);
        root->Data("Vsync", vsync_, false);

        SCallListenersPtr(this, GameListener, OnSettingsLoad, *root);

        // if width or height does not exist then we have to guess the resolution
        if (!width || !height)
            Simian::OS::Instance().DesktopResolution(width, height);

        Size(Simian::Vector2(width, height));

        settings.Close();

        SaveSettings(data.Identifier());
    }

    void Game::SaveSettings( const std::string& filePath )
    {
        FileObjectSubNode root;
        root.Name("Settings");

        root.AddData("Width", size_.X());
        root.AddData("Height", size_.Y());
        root.AddData("Fullscreen", fullscreen_);
        root.AddData("VSync", vsync_);

        SCallListenersPtr(this, GameListener, OnSettingsSave, root);

        DataFileIODefault file;
        file.Root(&root);
        file.Write(filePath);
        file.Close();
    }

    void Game::Start(GameState* initialState)
    {
        initialState_ = initialState;

        window_ = new Simian::Window(gameTitle_, Vector2::Zero, size_, OS::SWS_BORDERS, icon_);
        graphicsDevice_ = new Simian::GraphicsDevice();
        textureCache_ = new Simian::TextureCache(graphicsDevice_);
        shaderCache_ = new Simian::ShaderCache(graphicsDevice_);
        materialCache_ = new Simian::MaterialCache(graphicsDevice_, textureCache_, shaderCache_);
        modelCache_ = new Simian::ModelCache(graphicsDevice_);
        renderQueue_ = new Simian::RenderQueue(graphicsDevice_);

        debugShaderCache_ = new Simian::ShaderCache(graphicsDevice_);
        debugTextureCache_ = new Simian::TextureCache(graphicsDevice_);
        debugMaterialCache_ = new Simian::MaterialCache(graphicsDevice_, debugTextureCache_, debugShaderCache_);

        // default properties
        graphicsDevice_->Size(size_);
        graphicsDevice_->VSync(vsync_);
        graphicsDevice_->Fullscreen(fullscreen_);

        // set up the graphics device
        SCallListenersPtr(this, GameListener, OnInitGraphics);

        // initialize the subsystems
        GameListener::SubsystemError error = initSubsystems_();

        // call post init
        SCallListenersPtr(this, GameListener, OnPostInit, error);

        if (error != GameListener::SE_NONE)
            return;

        // prepare engine components
        SVerbose("Preparing engine components...");
        GameFactory::Instance().PrepareEngineComponents(
            graphicsDevice_, renderQueue_, materialCache_, modelCache_);

        // do initial cache loads
        textureCache_->LoadFiles();
        textureCache_->LoadTextures();

        modelCache_->LoadFiles();
        modelCache_->LoadMeshes();

        SVerbose("Starting game loop...");
        // start looping
        update_();

        Shutdown();
    }

    void Game::Exit()
    {
        exiting_ = true;
    }

    void Game::Init( GameState* initialState, WindowHandle handle )
    {
        initialState_ = initialState;

        window_ = !handle ? new Simian::Window(gameTitle_, Vector2::Zero, size_, OS::SWS_BORDERS) : NULL;
        windowHandle_ = handle;
        graphicsDevice_ = new Simian::GraphicsDevice();
        shaderCache_ = new Simian::ShaderCache(graphicsDevice_);
        textureCache_ = new Simian::TextureCache(graphicsDevice_);
        materialCache_ = new Simian::MaterialCache(graphicsDevice_, textureCache_, shaderCache_);
        modelCache_ = new Simian::ModelCache(graphicsDevice_);
        renderQueue_ = new Simian::RenderQueue(graphicsDevice_);

        debugShaderCache_ = new Simian::ShaderCache(graphicsDevice_);
        debugTextureCache_ = new Simian::TextureCache(graphicsDevice_);
        debugMaterialCache_ = new Simian::MaterialCache(graphicsDevice_, debugTextureCache_, debugShaderCache_);

        // default properties
        graphicsDevice_->Size(size_);
        graphicsDevice_->VSync(false);
        graphicsDevice_->Fullscreen(false);

        // set up the graphics device
        SCallListenersPtr(this, GameListener, OnInitGraphics);

        // initialize the subsystems
        GameListener::SubsystemError error = initSubsystems_();

        // call post init
        SCallListenersPtr(this, GameListener, OnPostInit, error);

        if (error != GameListener::SE_NONE)
            return;

        // prepare engine components
        SVerbose("Preparing engine components...");
        GameFactory::Instance().PrepareEngineComponents(
            graphicsDevice_, renderQueue_, materialCache_, modelCache_);

        // do initial cache loads
        textureCache_->LoadFiles();
        textureCache_->LoadTextures();

        modelCache_->LoadFiles();
        modelCache_->LoadMeshes();
    }

    void Game::Update()
    {
        u64 currentTime = OS::Instance().GetHighPerformanceCounter();
        frameTime_ = lastTime_ ? static_cast<float>(currentTime - lastTime_) : 0.0f;
        frameTime_ /= static_cast<float>(OS::Instance().GetHighPerformanceFrequency());
        frameTime_ = frameTime_ > FRAME_CAP ? FRAME_CAP : frameTime_;
        lastTime_ = currentTime;

        f32 dt = frameTime_ * timeMultiplier_;

        // clear debugrenderer
        DebugRendererClear();

        // update audio
        {
            SProfilerWatch(WATCH_AUDIO, Simian::Color(0.0f, 0.0f, 1.0f));
            AudioManager::Instance().Update(dt);
        }

        // update input
        {
            SProfilerWatch(WATCH_INPUT, Simian::Color(1.0f, 1.0f, 0.0f));
            InputManager::Instance().Update(frameTime_);
        }

        // update the state
        {
            SProfilerWatch(WATCH_GAMESTATE, Simian::Color(1.0f, 0.0f, 0.0f));
            gameStateManager_->Update(dt);
        }

        DrawFrame();
    }

    void Game::DrawFrame()
    {
        // draw anything on the render queue
        {
            SProfilerWatch(WATCH_GRAPHICS, Simian::Color(0.0f, 0.5f, 0.0f));
            renderQueue_->Draw();
        }

        // HACK: combining render textures.. needs work for post effects
        graphicsDevice_->Begin();
        graphicsDevice_->Projection(Simian::Matrix::Identity);
        graphicsDevice_->View(Simian::Matrix::Identity);
        graphicsDevice_->World(Simian::Matrix::Identity);

        u32 depthEnabled = graphicsDevice_->RenderState(GraphicsDevice::RS_DEPTHENABLED);
        u32 lightingEnabled = graphicsDevice_->RenderState(GraphicsDevice::RS_LIGHTING);
        u32 cullingType = graphicsDevice_->RenderState(GraphicsDevice::RS_CULLING);
        u32 blending = graphicsDevice_->RenderState(GraphicsDevice::RS_BLENDING);
        
        // set graphics states for compositing
        graphicsDevice_->RenderState(GraphicsDevice::RS_DEPTHENABLED, false);
        graphicsDevice_->RenderState(GraphicsDevice::RS_LIGHTING, false);
        graphicsDevice_->RenderState(GraphicsDevice::RS_CULLING, GraphicsDevice::SCM_NONE);
        graphicsDevice_->RenderState(GraphicsDevice::RS_BLENDING, true);
        graphicsDevice_->RenderState(GraphicsDevice::RS_SRCBLEND, GraphicsDevice::BM_ONE);
        graphicsDevice_->RenderState(GraphicsDevice::RS_DSTBLEND, GraphicsDevice::BM_INVSRCALPHA);

        // draw the render textures for camera
        for (std::vector<Camera*>::const_iterator i = renderQueue_->Cameras().begin(); 
            i != renderQueue_->Cameras().end(); ++i)
        {
            if (!(*i)->Composite()) continue;

            RenderTexture* renderTexture = (*i)->RenderTexture();

            // if there is a compositor chain, render it
            if ((*i)->CompositorChain())
            {
                (*i)->CompositorChain()->Render();
                renderTexture = (*i)->CompositorChain()->Output();
            }

            // set the render texture
            renderTexture->Set();
            renderTexture->SamplerState(Texture::SS_MINFILTER, Texture::FF_LINEAR);
            renderTexture->SamplerState(Texture::SS_MAGFILTER, Texture::FF_LINEAR);
            graphicsDevice_->Draw(fullscreenQuad_, GraphicsDevice::PT_TRIANGLESTRIP, 2);
            renderTexture->Unset(); // we probably don't need this
        }
        
        // set up orthographics projection for framerate controller and profiler
        graphicsDevice_->Projection(
            Simian::Matrix::Ortho(size_.X(), size_.Y(), 0, 1));

        // draw the framerate controller info here
#if SIMIAN_FRAMERATECONTROLLER == 1
        frameRateController_->Update(frameTime_);
        frameRateController_->Render();
#endif

        SProfilerUpdate(frameTime_);
        SProfilerDraw();

        // restore graphics states
        graphicsDevice_->RenderState(GraphicsDevice::RS_DEPTHENABLED, depthEnabled);
        graphicsDevice_->RenderState(GraphicsDevice::RS_LIGHTING, lightingEnabled);
        graphicsDevice_->RenderState(GraphicsDevice::RS_CULLING, cullingType);
        graphicsDevice_->RenderState(GraphicsDevice::RS_BLENDING, blending);

        // end the drawing
        graphicsDevice_->End();

        // swap buffers
        graphicsDevice_->Swap();
    }

    void Game::Shutdown()
    {
        // unload all caches (in case they were not freed)
        modelCache_->UnloadAll();
        materialCache_->UnloadAll();
        textureCache_->UnloadAll();
        shaderCache_->UnloadAll();

        SCallListenersPtr(this, GameListener, OnPreShutdown);

#ifdef _DESCENDED
        for (u32 i = 0; i < listeners_.size(); ++i)
            delete listeners_[i];
        listeners_.clear();
#endif

        // unload all subsystems
        unload_();
    }
}
