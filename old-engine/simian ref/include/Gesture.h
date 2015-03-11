/*************************************************************************/
/*!
\file		Gesture.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GESTURE_H_
#define DESCENSION_GESTURE_H_

#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

#include "Simian/Math.h"
#include "Simian/Vector2.h"
#include "Simian/FiniteStateMachine.h"
#include "Simian/Delegate.h"
//#define G_USERTEST_H

#ifdef G_USERTEST_H
#include "InetTest/Inet.h"
#endif

namespace Descension {

    //Gesture class: shape and gesture detection
    class Gesture
    {
    //--------------------------------------------------------------------------
    public:
        //requires higher precision epsilon
        static const Simian::f32 GESTURE_EPSILON;
        static const std::string GESTURE_DIRECTORY;
        static bool LogOutput, GetInput;

        static std::ifstream iFile;

        //temp inet logger
#ifdef G_USERTEST_H
        static Inet::InetLogger inet;
        static std::stringstream tempStream;
        static std::stringstream msgStr;
#endif
        
        //gesture commands
        enum GESTURE_COMMANDS
        {
            GCMD_NONE = 0,
            GCMD_SLASH,
            GCMD_BOLT,
            GCMD_FIRE,
            GCMD_HEAL,
            GCMD_ICE,
            GCMD_WIND,
            GCMD_EARTH,
            GCMD_DARK,

            GCMD_FAIL,
            GCMD_TOTAL
        };

        //gesture states
        enum GESTURE_STATUS
        {
            // init, reading config
            GST_LOADING = 0,

            // in game states
            GST_READY, //waiting
            GST_DRAWING, //user drawing
            GST_SEPARATING, //optimising by splitting
            GST_ASSIGNING, //allocating final results
            GST_COMPARING, //checking with master lists
            GST_COMPLETED, //result is determined
            
            GST_DISABLED, //restarting etc.
            GST_ERROR, //to be set if any problem occurred
            GST_TOTAL
        };

        //MANUAL FLAGS: COMMAND_ID
        enum GESTURE_FLAGS {

            //-----------------
            //--ORDER MATTERS--
            GF_DISABLED = 0,

            GF_XMAJOR, //axis major

            GF_MULTIDIR, //chances multi-directional, used as xMajorMD
            
            GF_CLOSED, //closed shapes
            GF_LINEAR, //linear path
            GF_ANGULAR, //sharp angles
            GF_SPIRAL, //min 2 revolution

            GF_ANGULAR2, //2nd degree
            GF_EXCESSANGLE, //too many angles
            
            //--ORDER MATTERED--
            //-------------------
            GF_PUZZLEMODE,

            GF_TOTAL
        };

    //--------------------------------------------------------------------------
    public:
        //point and weights struct
        struct WeightPoint {
            //width*y+x from 0
            Simian::u32 ID;
            //position coordinates
            union {
                struct {
                    Simian::s32 X, Y;
                };
                Simian::s32 pt[2];
            };
            //0.0-1.0f
            Simian::f32 Weight;
            operator Simian::u32()
            {
                return ID;
            }
        };

        //bounding box data container generated by user
        struct DrawBoundary {
            Simian::f32 MaxX, MaxY, MinX, MinY, LastX, LastY;
        };

        //result container
        struct GestureResult {
            Simian::f32 MatchScore, MissScore, FinalScore;
            Simian::f32 Angle; //pos/angle data
            Simian::u32 Action;
            bool Pass;
            DrawBoundary BoundBox;
            Simian::Vector2 Triangle[3]; //TOP LEFT RIGHT
            operator bool ()
            {
                return Pass;
            }
        };

        // A B
        // C D
        struct Mtx2x2 {
            Simian::f32 A, B, C, D;
        };

    //--------------------------------------------------------------------------
    private:
        std::vector<GestureResult> commandVec_;

        std::vector<Simian::Vector2> pointList_; //list of plotted points
        std::vector<Simian::Vector2> pointListMD_; //for multi-dir if applicable
        std::vector<WeightPoint> pointWeightList_; //list of point with weights
        std::vector<WeightPoint> pointWeightListMD_; //for multi-dir if applicable

        std::vector<std::string> masterCheckList_; //list of master files

        //the following shares the corresponding data in order
        std::vector<std::vector<WeightPoint> > masterPointWeightList_;
        std::vector<Simian::f32> masterMaxScoreList_; //max possible score
        std::vector<Simian::u32> masterFlagList_; //list of bit flags
        std::vector<Simian::u32> masterCmdList_; //list of bit flags

        std::vector<Simian::u32> masterPuzzleIDList_; //list of bit for puzzles

        Simian::Vector2 lastPos_; //last plotted point
        DrawBoundary userBoundingBox_, userBoundingBoxMD_; //useful bounding box data
        GestureResult scoreResult_; //result of the latest sketch
        Simian::f32 totalScore_, totalScoreMD_; //total score accumulated

        Simian::f32 sampleFreq_; //number of point per dist
        Simian::f32 brushSize_; //coverage
        Simian::f32 decayMultiplier_; //outer decay, DO NOT USE 0, use EPSILON
        Simian::f32 brushHardness_; //inner decay
        Simian::u32 samplePadding_; //bounding box padding
        Simian::f32 boxSize_, mapPadding_; //"pixel" size, pixel-map-border
        Simian::f32 resultTolerance_; //0-1.0 of score

        Simian::u32 mapLength_; //# of pixel per sides

        Simian::u32 maxSamplePoints_; //should be soft capped by gameplay
        Simian::u32 totalPointCount_; //total recorded point count

        Simian::u32 gsFlags_;

        Simian::s32 firstAngleIndex_, lastAngleIndex_, lastClosedIndex_;

        Simian::FiniteStateMachine fsm_; //pointer to function determined by SM

    public:
        //default constructor
        Gesture( Simian::f32 totalScore = 0.0f,
            Simian::f32 totalScoreMD = 0.0f,
            Simian::f32 sampleFreq = 0.0f,
            Simian::f32 brushSize = 0.0f,
            Simian::f32 decayMultiplier = 0.0f,
            Simian::f32 brushHardness = 0.0f,
            Simian::u32 samplePadding = 0,
            Simian::f32 boxSize = 0.0f,
            Simian::f32 mapPadding = 0.0f,
            Simian::f32 resultTolerance = 0.0f,
            Simian::u32 mapLength = 0,
            Simian::u32 maxSamplePoints = 0,
            Simian::u32 totalPointCount = 0,
            Simian::u32 gsFlags = 1); //disabled until init

        //destructor
        ~Gesture();

        //function to facilitate re-init
        void InitGesture(bool reload=true);
        //enable
        void SetEnabled(bool val);
        bool GetEnabled(void);
        //--puzzle mode
        void PuzzleModeID(Simian::u32 puzzleID);
        Simian::u32 PuzzleModeID(void);
        //Update function
        void Update(Simian::f32 dt);
        //Get current state ENUM
        int GetState(void) const;
        //get queued commands
        bool GetCommand(Gesture::GestureResult& result);
        //check if available to draw
        bool CanDraw(void);
        //send user data through inet
        void UploadData(void);
        //rotate vector by rotation matrix
        static void RotateVec2(Simian::Vector2 &vec, const Mtx2x2 &matrix);

    private:
        //StateFunctions
        //load the files
        void LoadGestureFile_(Simian::DelegateParameter&);
        //Draw
        void Ready_(Simian::DelegateParameter&);
        void Drawing_(Simian::DelegateParameter&);
        void Separating_(Simian::DelegateParameter&);
        void Assigning_(Simian::DelegateParameter&);
        void Comparing_(Simian::DelegateParameter&);
        void Complete_(Simian::DelegateParameter&);
        //reset command for next input
        void Reset_();
        void ResetGSFlag_();

        //Misc.
        //function to facilitate exiting
        void ClearAllVectors_(void);

        //Private math functions
        static bool pointWeightComp_(const WeightPoint &lhs, const WeightPoint &rhs);
        //safer normalization with EPSILON check
        bool normalizeVec_(Simian::Vector2 &vec);
    };
}
#endif