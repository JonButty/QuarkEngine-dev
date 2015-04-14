/*************************************************************************/
/*!
\file		GCGestureInterpreter.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCGESTUREINTERPRETER_H_
#define DESCENSION_GCGESTUREINTERPRETER_H_

#include "Gesture.h"
#include "GestureSkills.h"
#include "GCAIBase.h"
#include "GCAttributes.h"

#include "Simian/Camera.h"
#include "Simian/CCamera.h"
#include "Simian/CTransform.h"
#include "Simian/EntityComponent.h"

#include "Simian/MaterialCache.h"
#include "Simian/VertexBuffer.h"
#include "Simian/VertexFormat.h"

#include <list>

namespace Simian
{
    class IndexBuffer;
    class CSoundEmitter;
}

namespace Descension
{
    //just an interfact to interpret the gesture
    class GCGestureInterpreter : public Simian::EntityComponent
    {
    public:
        //const statics
        static const Simian::u32 MAX_VBSIZE, JOINT_COUNT;
        static const Simian::f32 MIN_PT_DIST;
        static const Simian::u32 VB_DIV;//12
        static const Simian::f32 LINE_WIDTH, INK_DECREMENT, SLOW_MULTIPLIER,
                                 JOINT_ANGLE, JOINT_ANGLE2, U_OFFSET, LOWESTY;

        static bool newStart;
        static GCGestureInterpreter* GCGestureInterpreter::pGestureInterpreter;

        enum GI_STATE
        {
            GI_READY = 0,
            GI_EXITING,
            GI_FADING,
            GI_CANCEL,

            GI_STATE_TOTAL
        };

        enum GI_MESSAGEID
        {
            GI_MSGID_FAIL = 0,
            GI_MSGID_NOMP,
            GI_MSGID_STRAIGHT,

            GI_MSGID_NONE
        };

        struct UnwindSet
        {
            Simian::u32 Size;
            bool Unwind;
            UnwindSet(Simian::u32 size = 0, bool unwind = false)
                : Size(size), Unwind(unwind)
            {}
            operator unsigned int() { return Size; }
        };

        struct BlazingTrine
        {
            Simian::Vector3 Points[3];
            Simian::Vector3 Center;
            Simian::f32 Radius;
            Simian::f32 Timer;
        };
    public:
        //set by playerlogic
        bool GestureEnabled(void);
        void GestureEnabled(bool val);
        bool Available(void);
        void LineDepth(Simian::f32 val);
        Simian::f32 LineDepth();
        void BGDepth(Simian::f32 val);
        Simian::f32 BGDepth();
        void PuzzleModeID(Simian::u32 id);
        Simian::u32 PuzzleModeID(void);
        bool PuzzleSolved(void);

        //Getter to myself, in all my glory!!!
        static GCGestureInterpreter* Instance(void);

    private:
        //pointer to object references
        std::vector<GCAIBase*> *allEnemyVec_;
        Simian::CTransform* cameraTransform_;
        Simian::CCamera* camera_;
        Simian::Entity *msgFail_, *msgNoMP_, *msgStraight_;

        //vertex buffers
        std::vector<Simian::Vector2> samplePoints_; //point sampling
        std::vector<Simian::PositionColorUVVertex> vertices_;
        std::list<std::vector<Simian::PositionColorUVVertex> > outgoing_;
        std::list<UnwindSet> outgoingSize_;
        std::list<UnwindSet> outgoingFullSize_;
        std::list<std::vector<Simian::PositionColorUVVertex> > slashShatter_;
        std::list<Simian::u32> slashShatterSize_;
        std::list<std::vector<Simian::f32> > slashShatterRand_;
        std::vector<Simian::PositionColorUVVertex> uiVertices_;

        Simian::VertexBuffer* buffer_;
        Simian::VertexBuffer* uibuffer_;
        Simian::IndexBuffer* ibuffer_;
        Simian::Material* material_;
        Simian::Material* materialSlash_;
        Simian::Material* materialBG_;
        Simian::u32 lastVB_;
        Simian::u32 vbSize_, UIvbSize_;
        Simian::f32 dt_, lineWidth_, prevAngle_, currAngle_, messageIntp_;
        Simian::f32 dmgMultiplier_[GCAttributes::ST_TOTAL];
        Simian::u32 newStrip_, status_, msgStatus_;
        Simian::CSoundEmitter* sounds_;

        Simian::f32 bgDepth_, lineDepth_, mpCost_;
        GestureSkills gestureSkills_;
        bool puzzleSolved_;

        std::vector<Gesture::GestureResult> resultVec_;

        // for the annoying gayvin
        std::vector<BlazingTrine> blazingTrines_;

        //real
        static Gesture gestureClass_;

        static Simian::FactoryPlant<Simian::EntityComponent,
            GCGestureInterpreter>
            factoryPlant_;

    private:
        void update_(Simian::DelegateParameter& param);
        void animateUV_(void);
        void slashEffect_(Simian::f32 dt);
        bool jointCheck_(const Simian::Vector2& mouseVec, Simian::f32 angle);
        void castBlazingTrine_(const BlazingTrine& trine);
        void updateBlazingTrines_(Simian::f32 dt);

        void SwapMessage_(Simian::u32 msgID);
        void UpdateMessage_(void);

    public:
        //constructor that does nothing
        GCGestureInterpreter(Simian::VertexBuffer* buffer = 0,
                             Simian::VertexBuffer* uibuffer = 0,
                             Simian::IndexBuffer* ibuffer = 0,
                             Simian::Material* material = 0,
                             Simian::Material* materialSlash = 0,
                             Simian::Material* materialBG = 0,
                             Simian::u32 lastVB = 0,
                             Simian::u32 vbSize = 0,
                             Simian::u32 UIvbSize = 0,
                             Simian::f32 dt = 0.0f,
                             Simian::f32 lineWidth = 0.0f,
                             Simian::f32 prevAngle = 0.0f,
                             Simian::f32 currAngle = 0.0f,
                             Simian::f32 messageIntp = 0.0f,
                             Simian::u32 newStrip = 0,
                             Simian::u32 status = 0,
                             Simian::f32 bgDepth = 100.0f,
                             Simian::f32 lineDepth = 90.0f,
                             Simian::f32 mpCost = 0.0f)
            : buffer_(buffer), uibuffer_(uibuffer), ibuffer_(ibuffer),
              msgFail_(NULL), msgNoMP_(NULL), msgStraight_(NULL),
              material_(material), materialSlash_(materialSlash), materialBG_(materialBG),
              lastVB_(lastVB), vbSize_(vbSize), UIvbSize_(UIvbSize), dt_(dt),
              lineWidth_(lineWidth), prevAngle_(prevAngle), currAngle_(currAngle),
              messageIntp_(messageIntp), newStrip_(newStrip), status_(status), msgStatus_(GI_MSGID_NONE),
              sounds_(NULL), bgDepth_(bgDepth), lineDepth_(lineDepth), mpCost_(mpCost)
              {
                  for (Simian::u32 i=0; i<GCAttributes::ST_TOTAL; ++i)
                      dmgMultiplier_[i] = 1.0f;
              }

        //update and modify entity status
        void Update(Simian::f32 dt);
        void ExitCasting_(void);

        //process if a command is used
        void ProcessGesture(Gesture::GestureResult& result);
        const GestureSkills& GetSkills(void) const { return gestureSkills_; }
        void SetSkills(const GestureSkills& val);

        //activate command
        void ActivateGesture(void);

        //init gesture
        void InitGesture(bool reload=true);

        //draw current vbuffer
        void AddQuad(const Simian::Vector2& vec);
        void SetLastQuad(const Simian::Vector2& vec);
        void AddToRenderQ(Simian::DelegateParameter&);
        void DrawVertexBuffer(Simian::DelegateParameter& param);
        void DrawVertexBufferSlash(Simian::DelegateParameter& param);
        void DrawVertexBufferUI(Simian::DelegateParameter& param);

        void OnSharedLoad();
        void OnSharedUnload(void);
        void OnAwake();
        void OnInit();
        void OnDeinit();

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
