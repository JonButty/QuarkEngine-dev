/*************************************************************************/
/*!
\file		GCUI.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCUI_H_
#define DESCENSION_GCUI_H_

#include "Sprite.h"

#include "Simian/EntityComponent.h"
#include "Simian/Camera.h"
#include "Simian/CCamera.h"
#include "Simian/CTransform.h"
#include "Simian/VertexBuffer.h"
#include "Simian/FiniteStateMachine.h"

namespace Simian //FWD
{
    class IndexBuffer;
    class Material;
    class CJukebox;
}

namespace Descension
{
    class GCAIBase;
    class Entity;
}

namespace Descension
{
    class GCUI : public Simian::EntityComponent
    {
    public: //const statics
        static std::vector<std::vector<Simian::Vector2> > uiFrames;
        static GCUI* pGCUI;

        static const Simian::u32 MAX_SPRITE_COUNT, POLY_VERTEX_COUNT, MAX_VBSIZE,
                                 ALPHABIT;
        static const Simian::f32 UV_EPSILON, ICON_START_SCALE, BAR_RELSCALE_Y,
                                 BAR_RELSCALE_X;

        enum UI_SPRITE_FRAME
        {
            //--BASICs
            UI_TLBG = 0,
            UI_TLBG_BG,
            UI_HP,
            UI_MP,
            UI_EXP,
            UI_EXPGLOW,
            UI_BAR,
            UI_NAIL,
            UI_FLASH,
            //--BTN
            UI_WIN_BTN,
            UI_PAUSE,
            UI_OPTION,
            UI_HOWTOPLAY,
            UI_QUIT, 
            UI_CREDIT,
            UI_RESTART,
            UI_YES,
            UI_NO,
            //--BTN_DOWN
            UI_OPTION_DOWN,
            UI_HOWTOPLAY_DOWN,
            UI_QUIT_DOWN, 
            UI_CREDIT_DOWN,
            UI_RESTART_DOWN,
            UI_WIN_QUIT,
            UI_WIN_RESTART,
            UI_YES_DOWN,
            UI_NO_DOWN,
            //--SKILLS_ICONS
            UI_ICON_SLASH,
            UI_ICON_LIGHTNING,
            UI_ICON_FIRE,
            UI_ICON_HEAL,
            //--BOSSES
            UI_BOSS0,

            UI_CRIMSON_REVENANT,
            UI_VULCAN,
            UI_CRYSTAL_GUARDIAN,

            UI_A0,
            UI_A1,
            UI_A2,
            UI_A3,
            UI_A4,
            UI_A5,
            UI_A6,
            UI_A7,
            UI_A8,
            UI_A9,
            UI_A10,

            UI_EXP_MSG0,
            UI_EXP_MSG1,

            UI_TOTAL_FRAMES
        };
        enum GAME_STATUS
        {
            GAME_LOADING = 0,
            GAME_RUNNING,
            GAME_PAUSED,

            GAME_CINEMATIC,

            GAME_STATUS_TOTAL
        };
        enum MENU_INNERSTATE
        {
            MENU_READY,
            MENU_OPTION,
            MENU_HOWTOPLAY,
            MENU_CREDIT,
            MENU_RESTART,
            MENU_QUIT,

            MENU_CONFIRM, //requires answer

            MENU_STATE_TOTAL
        };
        union Hexa32 //--B G R A
        {
            struct {
                Simian::u32 Full;
            };
            Simian::u8 Byte[4];
            Hexa32 (Simian::u32 val) {
                Full = val;
            }
        };

        struct BossNameStruct
        {
            enum TWEEN_STATE
            {
                BOSSNAME_IN = 0,
                BOSSNAME_HOLD,
                BOSSNAME_OUT,
                BOSSNAME_IDLE
            };

            Simian::u32 State;
            Simian::f32 InTime;
            Simian::f32 HoldTime;
            Simian::f32 OutTime;
            Simian::u32 BossENUM;

            BossNameStruct() : State(BOSSNAME_IDLE)
            {}
        };

        enum EXP_MSG_STATE
        {
            EXP_MSG_IDLE = 0,
            EXP_MSG_IN,
            EXP_MSG_HOLD,
            EXP_MSG_SWAP,
            EXP_MSG_UPDATESWAP,

            EXP_MSG_TOTAL
        };

    public: //accessors/setters
        static GCUI* Instance(void) { return pGCUI; }
        static Simian::u32 GameStatus(void) { return gameStatus_; }
        void Enabled(bool flag);
        void UIDepth(Simian::f32 val);
        Simian::f32 UIDepth(void);
        void StartBossBattle(GCAIBase* boss, Simian::u32 bossID);
        void EndBossBattle(void);

    private:
        Simian::f32 dt_, uiDepth_;
        Simian::u32 spriteCount_, menuState_, expMsgState_;
        bool enabled_, cinematics_, uiLoaded_, skillShow_, checkBars_;
        Simian::Entity* entity_;
        Simian::CJukebox* jukebox_;

        //sprite
        std::vector<Sprite> sprites_; //point sampling
        std::vector<Simian::PositionColorUVVertex> vertices_;

        //vertex-material
        Simian::Material* material_;
        Simian::VertexBuffer* vbuffer_;
        Simian::IndexBuffer* ibuffer_;
        Simian::Vector2 screenSize_;

        Simian::FiniteStateMachine fsm_;

        BossNameStruct bossName_;

        static Simian::FactoryPlant<Simian::EntityComponent,
            GCUI> factoryPlant_;
        static Simian::u32 gameStatus_, bossID_;
        static GCAIBase* bossPtr_;

    private:
        void Update_(Simian::DelegateParameter& param);
        void UpdateUI_(void);
        void UpdateMenu_(void);
        void UpdateBoss_(void);
        void ResizeAllSprites_(void);

        void MenuRestartEnter_(Simian::DelegateParameter&);
        void MenuRestartUpdate_(Simian::DelegateParameter&);
        void MenuRestartExit_(Simian::DelegateParameter&);
        void MenuQuitEnter_(Simian::DelegateParameter&);
        void MenuQuitUpdate_(Simian::DelegateParameter&);
        void MenuQuitExit_(Simian::DelegateParameter&);

        void MenuCloseCallBack_(Simian::DelegateParameter&);
        
        void UpdateVB_(void);
        void GenerateUIFrames_(void);
        void LoadMenu_(Simian::Vector2& winSize, Simian::f32 opacLow);
        void UnloadMenu_(void);
        void LoadUI_(void);
        void UnloadUI_(void);

        void MenuEnterAnimation_(Sprite* sprite,
                                Simian::f32 startX,
                                Simian::f32 startY,
                                Simian::f32 intp,
                                Simian::f32 intpMultiplier,
                                bool horizontal = false,
                                Simian::f32 opacMultiplier = 1.0f);
    public:
        GCUI(Simian::f32 dt = 0.0f,
            Simian::f32 uiDepth = 80.0f,
            Simian::u32 spriteCount = 0,
            Simian::u32 menuState = 0,
            bool enabled = true,
            bool cinematics = false,
            Simian::Entity* entity = NULL,
            Simian::CJukebox* jukebox = NULL,
            Simian::Material* material = NULL,
            Simian::VertexBuffer* vbuffer = NULL,
            Simian::IndexBuffer* ibuffer = NULL) :
                dt_(dt), uiDepth_(uiDepth), spriteCount_(spriteCount),
                menuState_(menuState), enabled_(enabled), uiLoaded_(false), skillShow_(true),
                cinematics_(cinematics), entity_(entity), jukebox_(jukebox),
                material_(material), vbuffer_(vbuffer), ibuffer_(ibuffer),
                fsm_(MENU_READY, MENU_STATE_TOTAL)
        {
        }
        
        //--vertex buffers
        void AddToRenderQ(Simian::DelegateParameter&);
        void DrawVertexBuffer(Simian::DelegateParameter&);

        void LoadSkills(bool animate = false, Simian::u32 id = 0);
        void UpdateSkills(void);
        void UnloadSkills(bool remove = false);

        void ShowBossName(Simian::f32 inTime, Simian::f32 holdTime,
                          Simian::f32 outTime, Simian::u32 bossENUM);
        void UpdateBossName(void);
        void UpdateExpMessage(void);

        //--Sprites creation
        bool CreateSprite(const std::string& name = "Sprite",
                          Simian::f32 posX = 0.0f,
                          Simian::f32 posY = 0.0f,
                          Simian::f32 width = 0.0f,
                          Simian::f32 height = 0.0f,
                          Simian::f32 scaleX = 1.0f,
                          Simian::f32 scaleY = 1.0f,
                          Simian::f32 rotation = 0.0f,
                          Simian::f32 opacity = 1.0f,
                          Simian::f32 pivotX = 0.0f,
                          Simian::f32 pivotY = 0.0f);
        bool DeleteSprite(Simian::u32 id);
        Sprite& GetSprite(const std::string& name);
        Sprite* GetSpritePtr(const std::string& name);
        bool CheckAvailability(void);

        //--components
        void OnSharedLoad(void);
        void OnSharedUnload(void);
        void OnAwake();
        void OnInit();
        void OnDeinit();

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
