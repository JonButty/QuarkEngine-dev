/*************************************************************************/
/*!
\file		GCGesturePuzzle.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/09/30
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCGESTUREPUZZLE_H_
#define DESCENSION_GCGESTUREPUZZLE_H_

#include "GCInputListener.h"
#include "Simian/FiniteStateMachine.h"

#include <string>

class Simian::CTransform;

namespace Descension
{
    class GCGesturePuzzle: public GCInputListener
    {
    public:
        static bool newStart; //--sprite init

        //! Puzzle ID enum
        enum GPUZZLE_ID
        {
            GPUZZLE_NONE = 0, //--activated in non-puzzle mode
            GPUZZLE_1, //--LevelInARow
            GPUZZLE_2, //--Drawing
            GPUZZLE_3,
            GPUZZLE_4,
            GPUZZLE_5,

            GPUZZLE_TOTAL,
            GPUZZLE_ENABLED //--Enabled Flag
        };

        enum GPUZZLE_STATES
        {
            GPUZZLE_IDLE = 0,
            GPUZZLE_INTRO,
            GPUZZLE_START,
            GPUZZLE_PASSED,
            GPUZZLE_CLOSED,

            GPUZZLE_TOTALSTATES
        };

    private:
        //--private built-in
        Simian::u32 puzzleID_;
        Simian::f32 cameraNear_, dt_, timer_;

        Simian::Entity *interact_, *backBtn_;
        Simian::CTransform* transform_;

        std::string triggerTarget_, triggerExit_;

        Simian::FiniteStateMachine fsm_;
        
        //--static
        static Simian::u32 puzzleFlag_;
        static GCGesturePuzzle* instance_;
        static bool isActive_;
        static Simian::FactoryPlant<Simian::EntityComponent, GCGesturePuzzle> factoryPlant_;
    public:
        Simian::u32 PuzzleID() const;
        //static GCGesturePuzzle* Instance(void);

        static void UnlockPuzzle(Simian::u32 id);
        static bool IsPuzzleUnlocked(Simian::u32 id);
        static bool IsActive(void);
    public:
        //--FSM CallBacks
        void IntroUpdate_(Simian::DelegateParameter&);
        void StartUpdate_(Simian::DelegateParameter&);
        void PassedUpdate_(Simian::DelegateParameter&);
        void ClosedEnter_(Simian::DelegateParameter&);

        void SolvedPuzzle();
        void FailedPuzzle();
        void ExitPuzzle(const std::string& trigger);

    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCGesturePuzzle();

        void OnSharedLoad();
        void OnAwake();
        void OnInit();
        void OnDeinit();

        void OnTriggered(Simian::DelegateParameter& param);

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
