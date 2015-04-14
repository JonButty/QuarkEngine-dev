/*************************************************************************/
/*!
\file		GCGesturePuzzleTrigger.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/09/30
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCGESTUREPUZZLETRIGGER_H_
#define DESCENSION_GCGESTUREPUZZLETRIGGER_H_

#include "Sprite.h"

#include "GCInputListener.h"

#include "Simian/VertexBuffer.h"
#include "Simian/FiniteStateMachine.h"
#include "Simian/Vector2.h"

#include <string>

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

    class GCGesturePuzzleTrigger: public GCInputListener
    {
    public:
        static const Simian::u32 MAX_SPRITE_COUNT, POLY_VERTEX_COUNT,
            MAX_VBSIZE, ALPHABIT;
        static const Simian::f32 UV_EPSILON, ICON_START_SCALE, FADE_SPEED;

        enum GPUZZLE_FRAME
        {
            GPUZZLE_FRAME_WINDOW = 0,
            GPUZZLE_FRAME_BUTTON,

            GPUZZLE_FRAME_TOTAL
        };

        enum InnerState
        {
            IDLE = 0,
            NEARBY,

            INNERSTATE_TOTAL
        };

    public:
        Simian::CTransform& Transform();
        void VBDepth(Simian::f32 val);
        Simian::f32 VBDepth(void);

        static bool PlayerInRange(void);

    private:
        Simian::CTransform* transform_;
        GCPhysicsController* physics_;
        Simian::CSoundEmitter* sounds_;

        Simian::f32 vbDepth_, timer_, dt_;
        Simian::u32 puzzleID_, spriteCount_;
        bool soundPlayed_;

        //sprite
        std::string texture_;
        std::vector<Sprite> sprites_; //point sampling
        std::vector<Simian::PositionColorUVVertex> vertices_;
        std::vector<std::vector<Simian::Vector2> > uvFrames_;

        //vertex-material
        Simian::Material* material_;
        Simian::VertexBuffer* vbuffer_;
        Simian::IndexBuffer* ibuffer_;
        Simian::Vector2 screenSize_;

        Simian::FiniteStateMachine fsm_;

        static GCGesturePuzzleTrigger* instance_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCGesturePuzzleTrigger>
            factoryPlant_;
    private:
        void update_(Simian::DelegateParameter& param);
        void ResizeAllSprites_(void);

        void FSMIdleUpdate_(Simian::DelegateParameter& param);
        void FSMNearbyUpdate_(Simian::DelegateParameter& param);
    public:
        GCGesturePuzzleTrigger();

        //--vertex buffers
        void UpdateVB_(void);
        void AddToRenderQ(Simian::DelegateParameter&);
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
