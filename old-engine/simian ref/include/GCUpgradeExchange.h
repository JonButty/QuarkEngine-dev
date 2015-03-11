/*************************************************************************/
/*!
\file		GCUpgradeExchange.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/09/30
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCUPGRADEEXCHANGE_H_
#define DESCENSION_GCUPGRADEEXCHANGE_H_

#include "Sprite.h"

#include "GCInputListener.h"

#include "Simian/VertexBuffer.h"
#include "Simian/FiniteStateMachine.h"
#include "Simian/Vector2.h"

namespace Simian
{
    class CTransform;
    class IndexBuffer;
    class Material;
    class CSoundEmitter;
}

namespace Descension
{
    class GCPhysicsController;

    class GCUpgradeExchange: public GCInputListener
    {
    public:
        //--Useful statics
        static const Simian::u32 MAX_SPRITE_COUNT, POLY_VERTEX_COUNT,
            MAX_VBSIZE, ALPHABIT;
        static const Simian::f32 UV_EPSILON, ICON_START_SCALE,
                                 ENTER_RAD, EXIT_RAD, RESTART_TIME, CAM_HEIGHT;
        static const Simian::u32 KEY_SMASH, KEY_ESC;
        static const Simian::f32 AGGRO_RANGE;

        enum UX_TYPE
        {
            UX_NONE = 0,
            UX_HP,
            UX_MP,
            UX_EXP,

            UX_EXCHANGE_TOTAL
        };
        enum UX_FRAME
        {
            UX_START,
            UX_ESCAPE,
            UX_NOEXP,
            UX_INTERRUPTED,
            UX_COMPLETED,
            UX_FULL,
            UX_ESCAPE2,

            UX_MSG_HP,
            UX_MSG_MP,
            UX_MSG_EXP,

            UX_TOTAL_FRAMES
        };
        enum UX_INNERSTATE
        {
            UX_STATE_IDLE = 0,
            UX_STATE_INTRO,
            UX_STATE_START,
            UX_STATE_END,

            UX_STATE_TOTAL
        };

    public:
        Simian::CTransform& Transform();
        void VBDepth(Simian::f32 val);
        Simian::f32 VBDepth(void);
        static bool Exchanging(void);

    private:
        Simian::CTransform* transform_;
        GCPhysicsController* physics_;
        Simian::CSoundEmitter* sounds_;

        Simian::f32 angle_, rotationSpeed_, vbDepth_;
        Simian::u32 typeID_, spriteCount_, smashCount_, gemID_;
        Simian::s32 prevHP_, initHP_, initExp_;
        Simian::f32 timer_, dt_, floatInc_, floatDec_, ratio_, pTimer_;
        Simian::f32 winTime_, baseScore_, maxScore_, easeFactor_, initCost_,
                    winCost_;

        bool shattered_, exchanged_, spinning_, destroyed_;

        //sprite
        std::vector<Sprite> sprites_; //point sampling
        std::vector<Simian::PositionColorUVVertex> vertices_;
        std::vector<std::vector<Simian::Vector2> > uvFrames_;

        //vertex-material
        Simian::Material* material_;
        Simian::VertexBuffer* vbuffer_;
        Simian::IndexBuffer* ibuffer_;
        Simian::Vector2 screenSize_;
        Simian::Vector3 yawPitchZoom_;

        Simian::FiniteStateMachine fsm_;

        static bool exchanging_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCUpgradeExchange>
            factoryPlant_;
    private:
        void update_(Simian::DelegateParameter& param);
        void ResizeAllSprites_(void);
        void AddParticle_(bool wideArea = false);

        void checkDestroyed_(void);
        void updateDestroyed(void);

        void FSMIdleUpdate_(Simian::DelegateParameter& param);
        void FSMIntroUpdate_(Simian::DelegateParameter& param);
        void FSMStartEnter_(Simian::DelegateParameter& param);
        void FSMStartUpdate_(Simian::DelegateParameter& param);
        void FSMEndEnter_(Simian::DelegateParameter& param);
        void FSMEndUpdate_(Simian::DelegateParameter& param);
    public:
        GCUpgradeExchange();

        //--vertex buffers
        void AddToRenderQ(Simian::DelegateParameter&);
        void UpdateVB_(void);
        void DrawVertexBuffer(Simian::DelegateParameter&);
        //Sprites wrapper
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

        void OnSharedLoad();
        void OnSharedUnload(void);
        void OnAwake();
        void OnInit();
        void OnDeinit();

        void OnTriggered(Simian::DelegateParameter& param);

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
