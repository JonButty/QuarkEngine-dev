/*************************************************************************/
/*!
\file		Gesture.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*************************************************************************/
//#define G_DEBUG //allows toggleOn/Off
//#define G_DEBUGLOG //allows log to file
//#define G_USERTEST //log but no retrieval, no toggling, allows inet
//#define G_DEBUGCONSOLE //print to console
//#define G_ADDQUAD //shall this class call AddQuad?

#ifdef G_DEBUGLOG
#include "Simian/DateTime.h"
#endif

//NOTE: Search #TEST&DEBUG for manual adjustment. Other codes should not change.

#include "Gesture.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#include "Simian/DataFileIOXML.h"
#include "Simian/DataLocation.h"
#include "Simian/FileObjectSubNode.h"
#include "Simian/FileArraySubNode.h"
#include "Simian/DataFileIO.h"
#include "Simian/Utility.h"
#include "Simian/DebugRenderer.h"
#include "Simian/LogManager.h"

#include "GCGestureInterpreter.h"

#include "Simian/Game.h"
#include "Simian/Mouse.h"
#include "Simian/Keyboard.h"

namespace Descension
{
    const Simian::f32 Gesture::GESTURE_EPSILON = 0.000001f;
    const std::string Gesture::GESTURE_DIRECTORY = "Gestures/";
#ifdef G_DEBUGLOG
    bool Gesture::LogOutput = true, Gesture::GetInput = false;

    //get computername as ID
    char g_cName[MAX_COMPUTERNAME_LENGTH + 1] = {0};
    std::stringstream gLogName;
#else
    bool Gesture::LogOutput = false, Gesture::GetInput = false;
#endif
    std::ifstream Gesture::iFile;
    
    //temp inet logger
#ifdef G_USERTEST
    Inet::InetLogger Gesture::inet;
    std::stringstream Gesture::tempStream(""), Gesture::msgStr;
#endif

    //constructor
    Gesture::Gesture( Simian::f32 totalScore,
        Simian::f32 totalScoreMD,
        Simian::f32 sampleFreq,
        Simian::f32 brushSize,
        Simian::f32 decayMultiplier,
        Simian::f32 brushHardness,
        Simian::u32 samplePadding,
        Simian::f32 boxSize,
        Simian::f32 mapPadding,
        Simian::f32 resultTolerance,
        Simian::u32 mapLength,
        Simian::u32 maxSamplePoints,
        Simian::u32 totalPointCount,
        Simian::u32 gsFlags)
        :
        totalScore_(totalScore), //total score accumulated
        totalScoreMD_(totalScoreMD), //total score accumulated
        sampleFreq_(sampleFreq), //number of point per dist
        brushSize_(brushSize), //coverage
        decayMultiplier_(decayMultiplier), //outer decay, DO NOT USE 0, use EPSILON
        brushHardness_(brushHardness), //inner decay
        samplePadding_(samplePadding), //bounding box padding
        boxSize_(boxSize), //"pixel" size
        mapPadding_(mapPadding), //pixel-map-border
        resultTolerance_(resultTolerance), //number of pixel per side (SQUARE MAP)
        mapLength_(mapLength), //number of pixel per side (SQUARE MAP)
        maxSamplePoints_(maxSamplePoints), //should be soft capped by gameplay
        totalPointCount_(totalPointCount), //total recorded point count
        gsFlags_(gsFlags),
        fsm_(GST_LOADING, GST_TOTAL)
  //pointer to function determined by SM
    {
        //member init list
        //InitGesture_(); //use by public caller
    }

    //destructor
    Gesture::~Gesture()
    {
        void ClearAllVectors(void);
    }

    void Gesture::SetEnabled(bool val)
    {
        if (val)
            SFlagUnset(gsFlags_, GF_DISABLED);
        else
        {
            SFlagSet(gsFlags_, GF_DISABLED);
#ifdef G_USERTEST
            msgStr << "Hold CTRL and Left-click to begin drawing!" << std::endl;
#endif
        }
    }

    bool Gesture::GetEnabled(void)
    {
        return !SIsFlagSet(gsFlags_, GF_DISABLED);
    }
    void Gesture::PuzzleModeID(Simian::u32 puzzleID) //--ONLY 1 PUZZLE A TIME
    {
        Simian::u32 id = gsFlags_ >> GF_PUZZLEMODE;
        for (Simian::u32 i=0; id; ++i)
        {
            SFlagUnset(gsFlags_, GF_PUZZLEMODE + i);
            id >>= 1;
        }
        if (puzzleID > 0)
            SFlagSet(gsFlags_, GF_PUZZLEMODE + puzzleID - 1);
    }
    Simian::u32 Gesture::PuzzleModeID(void)
    {
        return (gsFlags_ >> GF_PUZZLEMODE);
    }

    void Gesture::Update(Simian::f32 dt)
    {
#ifdef G_USERTEST
        DebugRendererDrawText(msgStr.str(), Simian::Vector2(-360.0f, -150.0f), Simian::Color(1,1,1,1));
#endif
#ifdef G_DEBUG
        if (Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_CONTROL) &&
            Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_X))
        {
            LogOutput = !LogOutput;
            LogOutput ? std::cout << "Logging Gesture Output..." << std::endl :
                        std::cout << "Disabled Gesture Logging..." << std::endl;
        }
#endif
#ifdef G_DEBUG
        if (Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_CONTROL) &&
            Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_C))
        {
            GetInput = !GetInput;
            GetInput ? std::cout <<
                "Sampling Mode: Press F7 during drawing mode to load data..."
                << std::endl : std::cout << "Disabled Sampling Mode..." << std::endl;

            if (GetInput)
            {
                std::string filePath(GESTURE_DIRECTORY);
                filePath += "Gesture_MinLog.gs";
                iFile.open(filePath.c_str(), std::ios_base::in);

                if (!iFile.is_open())
                {
                    std::cout << "Gesture>> Gesture_MinLog.gs not found." << std::endl;
                    //SAssert(false, "Gesture: No file for sample data found.");
                }
            }
            else
            {
                iFile.close();
            }
        }
        if (GetInput &&
            !Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_CONTROL) &&
            Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_C))
        {
            Simian::Vector2 pos;
            Simian::f32 x=0.0f, y=0.0f;

            iFile >> totalPointCount_;

            if (totalPointCount_ > 2)
            {
                for (Simian::u32 i=0; i<totalPointCount_; ++i)
                {
                    if (!GCGestureInterpreter::Instance()->Available())
                    {
                        std::cout << "Vertex buffer might overflow: Closing"
                            "current file stream." << std::endl;
                        iFile.close();
                        break;
                    }

                    iFile >> x >> y;

                    pos = Simian::Vector2(x, y);

                    pointList_[i] = pos;

                    if (i>0)
                    {
#ifdef G_ADDQUAD
                        GCGestureInterpreter::Instance()->SetLastQuad(
                            Simian::Vector2(pos.X(), -pos.Y()));
#endif
                        userBoundingBox_.MinX = Simian::Math::Min(pos.X(),
                            userBoundingBox_.MinX);
                        userBoundingBox_.MinY = Simian::Math::Min(pos.Y(),
                            userBoundingBox_.MinY);
                        userBoundingBox_.MaxX = Simian::Math::Max(pos.X(),
                            userBoundingBox_.MaxX);
                        userBoundingBox_.MaxY = Simian::Math::Max(pos.Y(),
                            userBoundingBox_.MaxY);
                    }
                    else
                    {
                        userBoundingBox_.MinX = pos.X();
                        userBoundingBox_.MinY = pos.Y();
                        userBoundingBox_.MaxX = pos.X();
                        userBoundingBox_.MaxY = pos.Y();
                    }
#ifdef G_ADDQUAD
                    GCGestureInterpreter::Instance()->AddQuad(
                        Simian::Vector2(pos.X(), -pos.Y()));//, true);
#endif
                }

                userBoundingBox_.LastX = (userBoundingBox_.MaxX+userBoundingBox_.MinX)
                    /2.0f;
                userBoundingBox_.LastY = (userBoundingBox_.MaxY+userBoundingBox_.MinY)
                    /2.0f;
                fsm_.ChangeState(GST_SEPARATING);
            }
            else
            {
                iFile.close();
            }
        }
#endif
        fsm_.Update(dt);
    }

    Simian::s32 Gesture::GetState(void) const
    {
        return fsm_.CurrentState();
    }

    bool Gesture::GetCommand(Gesture::GestureResult& result)
    {
        if (commandVec_.size() == 0)
        {
            return false;
        }
        else
        {
            result = commandVec_[0];
            commandVec_.erase(commandVec_.begin());
            return true;
        }
    }

    //check if available to draw
    bool Gesture::CanDraw(void)
    {
        return totalPointCount_ < maxSamplePoints_;
    }

    void Gesture::UploadData(void)
    {
#ifdef G_USERTEST
        inet.AddToDataStream(tempStream.str());
        if (!inet.SendData())
            msgStr << "Data transfer to server failed!" << std::endl;
        tempStream.str("");
#endif
    }

    //load the files
    void Gesture::LoadGestureFile_(Simian::DelegateParameter&)
    {
        //file I/O
        //std::cout << "loading" << std::endl;

        //input extractors
        Simian::u32 numArray = 0;
        Simian::f32 totalScore = 0;
        Simian::u32 bits = 0, cmdID = 0, puzzleID = 0;

        std::ifstream inputFile;
        
        masterFlagList_.resize(masterCheckList_.size());
        masterMaxScoreList_.resize(masterCheckList_.size());
        masterPointWeightList_.resize(masterCheckList_.size());
        masterCmdList_.resize(masterCheckList_.size());

        masterPuzzleIDList_.resize(masterCheckList_.size());

        for (Simian::u32 i=0; i<masterCheckList_.size(); ++i)
        {
            std::string filePath(GESTURE_DIRECTORY);
            filePath += masterCheckList_[i].c_str();
            inputFile.open(filePath.c_str(), std::ios_base::in);

            if (!inputFile.is_open())
            {
                SAssert(false, "Gesture: Input file not found.");
                continue;
            }

            inputFile >> numArray >> totalScore >> bits >> cmdID >> puzzleID;
            masterMaxScoreList_[i] = totalScore;
            masterFlagList_[i] = bits;
            masterCmdList_[i] = cmdID;

            masterPuzzleIDList_[i] = puzzleID;

            //temp struct for box data
            WeightPoint tempBox;
            for (Simian::u32 j=0; j<numArray; ++j)
            {
                //read the stuffs
                inputFile >> tempBox.ID;
                inputFile >> tempBox.X;
                inputFile >> tempBox.Y;
                inputFile >> tempBox.Weight;

                masterPointWeightList_[i].push_back(tempBox);
            }

            inputFile.close();
        }

        fsm_.ChangeState(GST_READY);
    }

    //get the cue to start drawing
    void Gesture::Ready_(Simian::DelegateParameter& param)
    {
        if (!SIsFlagSet(gsFlags_, GF_DISABLED))
        {
            Drawing_(param);
            //fsm_.ChangeState(GST_DRAWING);
            //std::cout << "ready" << std::endl;
        }
    }

    void Gesture::Drawing_(Simian::DelegateParameter&)
    {
        if (!GCGestureInterpreter::Instance()->Available())
        {
#ifdef G_USERTEST
            msgStr.str("");
            msgStr << "You have ran out of stamina for a single cast..." ;
#endif
            return;
        }

        //get mouse position WHERE (0,0) is at TOPLEFT
        Simian::Vector2 mouseVec = Simian::Mouse::GetMouseClientPosition();
        Simian::Vector2 winSize(Simian::Game::Instance().Size());
        mouseVec.X(mouseVec.X()-winSize.X()/2.0f);
        mouseVec.Y(mouseVec.Y()-winSize.Y()/2.0f);

        //factor for decision on points plotting with LAZY PYTHAGORAS
        Simian::f32 posDiff = fabs(mouseVec.X()-lastPos_.X()) +
                              fabs(mouseVec.Y()-lastPos_.Y());
        const Simian::f32 LASTPLOT = 4.0f;
        const Simian::u32 MINPOINT = 2;

        //-------------------------------DRAW-----------------------------------
        //smooth follow for drawing
#ifdef G_ADDQUAD
        if (fsm_.CurrentState() == GST_DRAWING &&
            totalPointCount_<maxSamplePoints_)
            GCGestureInterpreter::Instance()->SetLastQuad(
            Simian::Vector2(mouseVec.X(), -mouseVec.Y()));
#endif
        if (Simian::Mouse::IsReleased(Simian::Mouse::KEY_RBUTTON) ||
            SIsFlagSet(gsFlags_, GF_DISABLED) ||
            !GCGestureInterpreter::Instance()->Available())
        {
            //onRelease point: if many, last plot point must have min dist
            if (((maxSamplePoints_-totalPointCount_>0 && totalPointCount_>MINPOINT)
                && posDiff>=sampleFreq_/LASTPLOT))
            {
                //std::cout << "release\n";
                pointList_[totalPointCount_++] = mouseVec; //total point added to list array
#ifdef G_ADDQUAD
                GCGestureInterpreter::Instance()->AddQuad(
                    Simian::Vector2(mouseVec.X(), -mouseVec.Y()));//, true);
#endif
                //@addsprite

                userBoundingBox_.MinX = Simian::Math::Min(mouseVec.X(),
                    userBoundingBox_.MinX);
                userBoundingBox_.MinY = Simian::Math::Min(mouseVec.Y(),
                    userBoundingBox_.MinY);
                userBoundingBox_.MaxX = Simian::Math::Max(mouseVec.X(),
                    userBoundingBox_.MaxX);
                userBoundingBox_.MaxY = Simian::Math::Max(mouseVec.Y(),
                    userBoundingBox_.MaxY);
            }

            userBoundingBox_.LastX = (userBoundingBox_.MaxX+userBoundingBox_.MinX)
                                    /2.0f;
            userBoundingBox_.LastY = (userBoundingBox_.MaxY+userBoundingBox_.MinY)
                                    /2.0f;

            if (totalPointCount_ <= MINPOINT)
            {
                fsm_.ChangeState(GST_COMPLETED); //point only = fail + reset
#ifdef G_DEBUGCONSOLE
                std::cout << "FAIL: Gesture did not meet min. req of 2++ points!"
                          << std::endl;
#endif
#ifdef G_USERTEST
                msgStr << "FAIL: Gesture did not meet min. req of 2++ points!"
                    << std::endl;
#endif
            }
            else
            {
                fsm_.ChangeState(GST_SEPARATING);
            }
        }

        //only if mouse pressed
        if (!Simian::Mouse::IsPressed(Simian::Mouse::KEY_RBUTTON))
        {
            return;
        }

        //lag guard: smooth interpolation
        Simian::s32 numIteration = static_cast<Simian::s32>(posDiff/sampleFreq_);

        //check to assist in last point plotting (onRelease)
        Simian::s32 availablePoints = static_cast<Simian::s32>
                                      (maxSamplePoints_-totalPointCount_);
        if (availablePoints < numIteration)
        {
            numIteration = availablePoints;
        }

        if (posDiff >= sampleFreq_)
        {
            for (Simian::s32 i=1; i<=numIteration &&
                totalPointCount_ < maxSamplePoints_; ++i)
            {
                if (!GCGestureInterpreter::Instance()->Available())
                    break;

                Simian::Vector2 interPoint((mouseVec.X()-lastPos_.X())/numIteration*i,
                    (mouseVec.Y()-lastPos_.Y())/numIteration*i);
                if (fsm_.CurrentState() == GST_READY)
                {
                    //makes sure it doesn't start from 0,0
                    fsm_.ChangeState(GST_DRAWING);
                    //std::cout << "first" << std::endl;
                    pointList_[totalPointCount_++] = mouseVec; //total point added to list array
#ifdef G_ADDQUAD
                    GCGestureInterpreter::Instance()->AddQuad(
                        Simian::Vector2(mouseVec.X(), -mouseVec.Y()));//, true);
#endif
                    //@addsprite

                    userBoundingBox_.MinX = mouseVec.X();
                    userBoundingBox_.MinY = mouseVec.Y();
                    userBoundingBox_.MaxX = mouseVec.X();
                    userBoundingBox_.MaxY = mouseVec.Y();
                    break;
                }
                else
                {
                    //total point added to list array
                    //std::cout << "plot" << std::endl;
                    Simian::Vector2 interVec(lastPos_+interPoint);
                    //--------------------------DRAW----------------------------
                    //smooth follow for drawing
                    if (fsm_.CurrentState() == GST_DRAWING &&
                        totalPointCount_<maxSamplePoints_)
#ifdef G_ADDQUAD
                        GCGestureInterpreter::Instance()->SetLastQuad(
                        Simian::Vector2(interVec.X(), -interVec.Y()));
#endif

                    pointList_[totalPointCount_++] = interVec;
#ifdef G_ADDQUAD
                    GCGestureInterpreter::Instance()->AddQuad(
                        Simian::Vector2(interVec.X(), -interVec.Y()));
#endif

                    //@addsprite

                    userBoundingBox_.MinX = Simian::Math::Min(mouseVec.X(), userBoundingBox_.MinX);
                    userBoundingBox_.MinY = Simian::Math::Min(mouseVec.Y(), userBoundingBox_.MinY);
                    userBoundingBox_.MaxX = Simian::Math::Max(mouseVec.X(), userBoundingBox_.MaxX);
                    userBoundingBox_.MaxY = Simian::Math::Max(mouseVec.Y(), userBoundingBox_.MaxY);
                }
            }
            lastPos_ = mouseVec; //update last position as current
        }
    }

    void Gesture::Separating_(Simian::DelegateParameter&)
    {
        //std::cout << "separating" << std::endl;

        if (LogOutput)
        {
            std::ofstream outputFile;
            std::string filePath(GESTURE_DIRECTORY);
            filePath += "Gesture_MinLog.gs";

#if defined(G_DEBUGLOG) | defined(G_USERTEST)
            outputFile.open(filePath.c_str(), std::ios_base::out | std::ios_base::app);

            if (!outputFile.is_open())
            {
                SAssert(false, "Gesture: File Output failed. Check folder permission.");
            }
            outputFile << totalPointCount_ << std::endl;
#endif
#ifdef G_USERTEST
            tempStream << totalPointCount_ << std::endl;
#endif
            for (Simian::u32 i=0; i<totalPointCount_; ++i)
            {
#if defined(G_DEBUGLOG) | defined(G_USERTEST)
                outputFile << std::setprecision(10) << pointList_[i].X() <<
                    " " << pointList_[i].Y() << std::endl;
#endif
#ifdef G_USERTEST
                tempStream << pointList_[i].X() << " " << pointList_[i].Y() <<
                    std::endl;
#endif
            }
#if defined(G_DEBUGLOG) | defined(G_USERTEST)
            outputFile.close();
#endif
        }

        //bounding analysis
        Simian::f32 dx = userBoundingBox_.MaxX - userBoundingBox_.MinX,
                    dy = userBoundingBox_.MaxY - userBoundingBox_.MinY;
        //const settings
        Simian::f32 closedShapeTolerance = boxSize_*4.0f *
            Simian::Math::Max(dx, dy)/250.0f;
        const Simian::f32 maxLinearDist = boxSize_*2.0f; //linear stray lines tolerance
        const Simian::f32 minAngle = 0.5f, angle2 = 1.1f; //angle2=(1-cosA)
#ifndef G_DEBUG
        const Simian::u32 maxAngleCount = 11; //times*2
#endif
        const Simian::u32 angleCount2Min = 6, boltExploit = 5; //times*2 +1
        const Simian::f32 minRatio = 0.3f;

        //loop diff variables
        //Simian::u32 minSpiralCount = 3, spiralCountX = 0, spiralCountY = 0;
        Simian::Vector2 lineNormal(-(pointList_[totalPointCount_-1] - pointList_[0]).Y()
            ,(pointList_[totalPointCount_-1] - pointList_[0]).X());
        normalizeVec_(lineNormal);

        //step1: closed shape?
        Simian::Vector2 firstSegment(pointList_[1] - pointList_[0]);
        Simian::Vector2 firstNormal(-firstSegment.Y(), firstSegment.X());
        firstNormal.Normalize();
        firstSegment = pointList_[0]-pointList_[totalPointCount_-1];
        if ( firstSegment.Length() < closedShapeTolerance
        && std::abs(firstSegment.Dot(firstNormal)) < closedShapeTolerance/2.0f)
        {
            SFlagSet(gsFlags_, GF_CLOSED);
        }

        //step2 loop: linear? angular?
        SFlagSet(gsFlags_, GF_LINEAR); //anti-check this
        Simian::u32 loopEnd = totalPointCount_-1;
        Simian::f32 boltLinearity = 0.0f;

        //--EXTRA!!!
        firstAngleIndex_ = lastAngleIndex_ = lastClosedIndex_ = 0;
        const Simian::u32 MIN_LAST_DIFF = 5;

        //step3: check angular end points
        Simian::u32 angleCount = 0;
        Simian::Vector2 startVec(pointList_[1] - pointList_[0]);
        Simian::Vector2 endVec(pointList_[totalPointCount_-1] - 
            pointList_[totalPointCount_-2]);
        if ( startVec.Dot(endVec)/(startVec.Length()*endVec.Length())
            < minAngle)
        {
            angleCount+=2; //imagine this proceeding normally
        }

        for (Simian::u32 i=1; i<loopEnd; ++i)
        {
            if (i > 1)
            {
                if (scoreResult_.Triangle[0].Y() > pointList_[i].Y())
                    scoreResult_.Triangle[0] = pointList_[i];
                if (scoreResult_.Triangle[1].X() > pointList_[i].X())
                    scoreResult_.Triangle[1] = pointList_[i];
                if (scoreResult_.Triangle[2].X() < pointList_[i].X())
                    scoreResult_.Triangle[2] = pointList_[i];
            }else{
                scoreResult_.Triangle[0] = scoreResult_.Triangle[1]
                    = scoreResult_.Triangle[2] = pointList_[i];
            }

            //check linear
            Simian::f32 linearCheck =
                std::fabs(lineNormal.Dot(pointList_[i]-pointList_[0]));
            boltLinearity = Simian::Math::Max(linearCheck, boltLinearity);
            if (SIsFlagSet(gsFlags_, GF_LINEAR) && linearCheck > maxLinearDist)
                    SFlagUnset(gsFlags_, GF_LINEAR);

            if ( (pointList_[i]-pointList_[0]).LengthSquared() <
                closedShapeTolerance*closedShapeTolerance/4.0f)
            {
                lastClosedIndex_ = i>MIN_LAST_DIFF ? i : 0;
            }

            //check spiral
            //if (!SIsFlagSet(gsFlags_, GF_SPIRAL))
            //{
            //    if (pointList_[i].X()>userBoundingBox_.LastX !=
            //        pointList_[i-1].X()>userBoundingBox_.LastX)
            //        ++spiralCountX;
            //    if (pointList_[i].Y()>userBoundingBox_.LastY !=
            //        pointList_[i-1].Y()>userBoundingBox_.LastY)
            //        ++spiralCountY;
            //    if (spiralCountX>minSpiralCount  && spiralCountY>minSpiralCount)
            //        SFlagSet(gsFlags_, GF_SPIRAL);
            //}

            //check angular
            //a.b = |a| |b| cosX
            Simian::Vector2 vecA = pointList_[i] - pointList_[i-1];
            Simian::Vector2 vecB = pointList_[i+1] - pointList_[i];
            if ( i > 1 )
            {   //in order: C.A A.B, (1-result1) + (1-result2) < angle2
                Simian::Vector2 vecC = pointList_[i-1] - pointList_[i-2];
                Simian::f32 result1 = 1.0f - 
                    vecC.Dot(vecA) / (vecC.Length()*vecA.Length());
                Simian::f32 result2 = 1.0f - 
                    vecA.Dot(vecB) / (vecA.Length()*vecB.Length());
                if ( (result1+result2) >= minAngle)
                {
                    SFlagSet(gsFlags_, GF_ANGULAR);
                    if (firstAngleIndex_ == 0)
                        firstAngleIndex_ = i;
                    lastAngleIndex_ = i>MIN_LAST_DIFF ? i : 0;
                    ++angleCount;
                }
            }
            if ( (vecA.Dot(vecB) / (vecA.Length()*vecB.Length()) < minAngle) )
            {
                SFlagSet(gsFlags_, GF_ANGULAR);
                if (firstAngleIndex_ == 0)
                    firstAngleIndex_ = i;
                lastAngleIndex_ = i>MIN_LAST_DIFF ? i : 0;
                ++angleCount;
            }

            if ( !SIsFlagSet(gsFlags_, GF_ANGULAR2) && i>1 
                 || angleCount >= angleCount2Min)
            {   //in order: C.A A.B, (1-result1) + (1-result2) < angle2
                Simian::Vector2 vecC = pointList_[i-1] - pointList_[i-2];
                Simian::f32 result1 = 1.0f - 
                    vecC.Dot(vecA) / (vecC.Length()*vecA.Length());
                Simian::f32 result2 = 1.0f - 
                    vecA.Dot(vecB) / (vecA.Length()*vecB.Length());
                if ((result1+result2) >= angle2 || angleCount >= angleCount2Min)
                    SFlagSet(gsFlags_, GF_ANGULAR2);
            }
        }
        //step3: xMajor?
        if (userBoundingBox_.MaxX - userBoundingBox_.MinX >=
            userBoundingBox_.MaxY - userBoundingBox_.MinY)
        {
            SFlagSet(gsFlags_, GF_XMAJOR);
        }

        //step4: too many angular trolls
#ifndef G_DEBUG
        if (!PuzzleModeID() && angleCount >= maxAngleCount)
            ResetGSFlag_();
        else if (PuzzleModeID() && angleCount >= maxAngleCount*2)
            ResetGSFlag_();
#endif

        //step5: lightning trolls
        if ((angleCount >= boltExploit-2 && angleCount <= boltExploit+2)
        && SIsFlagSet(gsFlags_, GF_ANGULAR2) && SIsFlagSet(gsFlags_, GF_LINEAR)
        && boltLinearity <= boxSize_)
        {
            SFlagUnset(gsFlags_, GF_ANGULAR);
            SFlagUnset(gsFlags_, GF_ANGULAR2);
        }
        if ((angleCount >= boltExploit-2 && angleCount <= boltExploit+2)
            && SIsFlagSet(gsFlags_, GF_ANGULAR2) && SIsFlagSet(gsFlags_, GF_LINEAR)
            && boltLinearity >= boxSize_)
        {
            SFlagUnset(gsFlags_, GF_LINEAR);
        }

        //step6: min ratio for pancake exploit
        if (SIsFlagSet(gsFlags_, GF_CLOSED) && SIsFlagSet(gsFlags_, GF_ANGULAR2)
            && (dx/dy < minRatio || dy/dx < minRatio))
            ResetGSFlag_();

        fsm_.ChangeState(GST_ASSIGNING);

        //--EXTRA!!!
        if (totalPointCount_ > MIN_LAST_DIFF)
        {
            if (static_cast<Simian::u32>(lastAngleIndex_) >= totalPointCount_ -
                MIN_LAST_DIFF &&
                static_cast<Simian::u32>(lastAngleIndex_ - firstAngleIndex_)
                < MIN_LAST_DIFF)
            {
                SFlagUnset(gsFlags_, GF_ANGULAR);
                SFlagUnset(gsFlags_, GF_ANGULAR2);
                SFlagSet(gsFlags_, GF_LINEAR);
            }
            if (totalPointCount_ > MIN_LAST_DIFF*4 &&
                !SIsFlagSet(gsFlags_, GF_CLOSED) &&
                static_cast<Simian::u32>(totalPointCount_ - lastClosedIndex_)
                < MIN_LAST_DIFF*4)
                SFlagSet(gsFlags_, GF_CLOSED);
        }
    }
    
    void Gesture::Assigning_(Simian::DelegateParameter&)
    {
        //std::cout << "assigning" << std::endl;
        
        //given: center of HUD = (0, 0)
        Simian::s32 minLimit = static_cast<Simian::s32>(mapLength_/2*-boxSize_);
        Simian::f32 sampleLength = mapLength_ * boxSize_;
        Simian::f32 sampleLengthP = sampleLength - mapPadding_*2; //padded by 1 box

        //-------------------------rotated list---------------------------------
        //get anti-rotation
        if (!SIsFlagSet(gsFlags_, GF_CLOSED))
        {
            Simian::f32 angle = std::atan(
                (pointList_[totalPointCount_-1].Y()-pointList_[0].Y())/
                (pointList_[totalPointCount_-1].X()-pointList_[0].X()) );
            angle = -angle;

            //set rotation matrix
            Mtx2x2 rotationMatrix;
            rotationMatrix.A = std::cosf(angle);
            rotationMatrix.B = -std::sinf(angle);
            rotationMatrix.C = std::sinf(angle);
            rotationMatrix.D = std::cosf(angle);

            for (Simian::u32 i=0; i<totalPointCount_; ++i)
            {
                //update bounding box & anti-rotation
                pointListMD_[i] = pointList_[i];
                RotateVec2(pointListMD_[i], rotationMatrix);

                if (i==0)
                {
                    userBoundingBoxMD_.MinX = pointListMD_[i].X();
                    userBoundingBoxMD_.MinY = pointListMD_[i].Y();
                    userBoundingBoxMD_.MaxX = pointListMD_[i].X();
                    userBoundingBoxMD_.MaxY = pointListMD_[i].Y();
                }else{
                    userBoundingBoxMD_.MinX = Simian::Math::Min(pointListMD_[i].X(),
                        userBoundingBoxMD_.MinX);
                    userBoundingBoxMD_.MinY = Simian::Math::Min(pointListMD_[i].Y(),
                        userBoundingBoxMD_.MinY);
                    userBoundingBoxMD_.MaxX = Simian::Math::Max(pointListMD_[i].X(),
                        userBoundingBoxMD_.MaxX);
                    userBoundingBoxMD_.MaxY = Simian::Math::Max(pointListMD_[i].Y(),
                        userBoundingBoxMD_.MaxY);
                }
            }

            //for normalizing, MULTIDIR == TRUE if xMajorMD
            if (userBoundingBoxMD_.MaxX - userBoundingBox_.MinX >=
                userBoundingBox_.MaxY - userBoundingBox_.MinY)
                SFlagSet(gsFlags_, GF_MULTIDIR);
        }

        //------------------------------------------------------------------
        // Log MD points
        //------------------------------------------------------------------
        if (LogOutput)
        {
            std::ofstream outputFile;
            std::string filePath(GESTURE_DIRECTORY);
            filePath += "Gesture_MinLogMD.gs";

#if defined(G_DEBUGLOG) | defined(G_USERTEST)
            outputFile.open(filePath.c_str(), std::ios_base::out | std::ios_base::app);

            if (!outputFile.is_open())
            {
                SAssert(false, "Gesture: File Output failed. Check folder permission.");
            }
            outputFile << totalPointCount_ << std::endl;
#endif
#ifdef G_USERTEST
            tempStream << totalPointCount_ << std::endl;
#endif
            for (Simian::u32 i=0; i<totalPointCount_; ++i)
            {
#if defined(G_DEBUGLOG) | defined(G_USERTEST)
                outputFile << std::setprecision(10) << pointListMD_[i].X() <<
                    " " << pointListMD_[i].Y() << std::endl;
#endif
#ifdef G_USERTEST
                tempStream << pointList_[i].X() << " " << pointList_[i].Y() <<
                    std::endl;
#endif
            }
#if defined(G_DEBUGLOG) | defined(G_USERTEST)
            outputFile.close();
#endif
        }

        //----------------normalize MD points into sample_length----------------
        //rotate pointListMD_ to prepare
        if (!SIsFlagSet(gsFlags_, GF_CLOSED))
        {
            for (Simian::u32 i=0; i<totalPointCount_-1; ++i)
            {
                WeightPoint tempLine;
                Simian::Vector2 firstPoint(pointListMD_[i]);
                Simian::Vector2 secondPoint(pointListMD_[i+1]);
                Simian::f32 minX = mapPadding_, minY = 0.0f;

                if (SIsFlagSet(gsFlags_, GF_MULTIDIR)) //xMajor
                {
                    Simian::f32 ratio = (userBoundingBoxMD_.MaxY-userBoundingBoxMD_.MinY)
                        /(userBoundingBoxMD_.MaxX-userBoundingBoxMD_.MinX);
                    //shift bigger axis from 0-1 FULL
                    firstPoint.X(minX+minLimit+(firstPoint.X() - userBoundingBoxMD_.MinX)/
                        (userBoundingBoxMD_.MaxX-userBoundingBoxMD_.MinX)*sampleLengthP);
                    secondPoint.X(minX+minLimit+(secondPoint.X() - userBoundingBoxMD_.MinX)/
                        (userBoundingBoxMD_.MaxX-userBoundingBoxMD_.MinX)*sampleLengthP);
                    //shift the smaller axis by same scaling done to bigger axis
                    firstPoint.Y(minY+(firstPoint.Y() - (userBoundingBoxMD_.MaxY+userBoundingBoxMD_.MinY)/2)/
                        (userBoundingBoxMD_.MaxY-userBoundingBoxMD_.MinY)*sampleLengthP*ratio);
                    secondPoint.Y(minY+(secondPoint.Y() - (userBoundingBoxMD_.MaxY+userBoundingBoxMD_.MinY)/2)/
                        (userBoundingBoxMD_.MaxY-userBoundingBoxMD_.MinY)*sampleLengthP*ratio);
                }
                else
                {
                    Simian::f32 ratio = (userBoundingBoxMD_.MaxX-userBoundingBoxMD_.MinX)
                        /(userBoundingBoxMD_.MaxY-userBoundingBoxMD_.MinY);
                    //shift bigger axis from 0-1 FULL
                    firstPoint.Y(minY+(firstPoint.Y() - (userBoundingBoxMD_.MaxY+userBoundingBoxMD_.MinY)/2)/
                        (userBoundingBoxMD_.MaxY-userBoundingBoxMD_.MinY)*sampleLengthP*(1/ratio));
                    secondPoint.Y(minY+(secondPoint.Y() - (userBoundingBoxMD_.MaxY+userBoundingBoxMD_.MinY)/2)/
                        (userBoundingBoxMD_.MaxY-userBoundingBoxMD_.MinY)*sampleLengthP*(1/ratio));
                    firstPoint.X(minX+minLimit+(firstPoint.X() - userBoundingBoxMD_.MinX)/
                        (userBoundingBoxMD_.MaxX-userBoundingBoxMD_.MinX)*sampleLengthP);
                    secondPoint.X(minX+minLimit+(secondPoint.X() - userBoundingBoxMD_.MinX)/
                        (userBoundingBoxMD_.MaxX-userBoundingBoxMD_.MinX)*sampleLengthP);
                }

                Simian::Vector2 originalVecA(firstPoint);
                Simian::Vector2 originalVecB(secondPoint);

                //create box with padding
                if (firstPoint.X() <= secondPoint.X())
                {
                    firstPoint.X(minX = firstPoint.X()-boxSize_*samplePadding_);
                    secondPoint.X(secondPoint.X()+boxSize_);
                }else{
                    firstPoint.X(firstPoint.X()+boxSize_);
                    secondPoint.X(minX = secondPoint.X()-boxSize_*samplePadding_);
                }
                if (firstPoint.Y() <= secondPoint.Y())
                {
                    firstPoint.Y(minY = firstPoint.Y()-boxSize_*samplePadding_);
                    secondPoint.Y(secondPoint.Y()+boxSize_);
                }else{
                    firstPoint.Y(firstPoint.Y() + boxSize_);
                    secondPoint.Y(minY = secondPoint.Y()-boxSize_*samplePadding_);
                }

                //loop and assign
                Simian::u32 xLoop = static_cast<Simian::u32>
                    (std::fabs((firstPoint.X()-secondPoint.X())/boxSize_)+1);
                Simian::u32 yLoop = static_cast<Simian::u32>
                    (std::fabs((firstPoint.Y()-secondPoint.Y())/boxSize_)+1);
                for (Simian::u32 j=0; j<xLoop; ++j)
                {
                    //within Xspace first
                    if (minX+j*boxSize_+boxSize_/2 < minLimit ||
                        minX+j*boxSize_+boxSize_/2 > minLimit+sampleLength)
                    {
                        continue;
                    }
                    for (Simian::u32 k=0; k<yLoop; ++k)
                    {
                        //within Yspace first
                        if (minY+k*boxSize_+boxSize_/2 < minLimit ||
                            minY+k*boxSize_+boxSize_/2 > minLimit+sampleLength)
                        {
                            continue;
                        }
                        tempLine.ID = static_cast<Simian::u32>(
                            (minY+k*boxSize_+boxSize_/2-minLimit)/boxSize_) *
                            static_cast<Simian::u32>(sampleLength/boxSize_)
                            + static_cast<Simian::u32>((minX+j*boxSize_+boxSize_/2
                            - minLimit)/boxSize_);
                        tempLine.X = static_cast<Simian::s32>(
                            (minX+j*boxSize_+boxSize_/2-minLimit)/boxSize_);
                        tempLine.Y = static_cast<Simian::s32>(
                            (minY+k*boxSize_+boxSize_/2-minLimit)/boxSize_);
                        //normal |_
                        Simian::Vector2 normal(-(originalVecB.Y()-originalVecA.Y()),
                            (originalVecB.X()-originalVecA.X()));
                        if (!normalizeVec_(normal))
                        {
                            continue; //bad magnitude
                        }
                        //vec(box-P) proj_onto_normal
                        Simian::Vector2 tempVec(
                            minX+j*boxSize_+boxSize_/2-originalVecA.X(),
                            minY+k*boxSize_+boxSize_/2-originalVecA.Y() );
                        //with brush hardness in the core section, 0000...0.0f-1.0f
                        tempLine.Weight = Simian::Math::Max(std::fabs(tempVec.Dot(normal))-
                            brushSize_*brushHardness_, 0.0f)/(brushSize_*decayMultiplier_);
                        //prevents duplicate: check before pushpak
                        std::vector<WeightPoint>::iterator ite_WP;
                        ite_WP = std::find(pointWeightListMD_.begin(),
                            pointWeightListMD_.end(), tempLine.ID);
                        if (ite_WP==pointWeightListMD_.end())
                        {
                            pointWeightListMD_.push_back(tempLine); //add new ID
                            if (tempLine.Weight < 1.0f)
                                totalScoreMD_ += 1.0f-tempLine.Weight;
                        }else{
                            //assign lesser where 0 = FULL (stroke over)
                            if (ite_WP->Weight >= tempLine.Weight)
                            {
                                if (tempLine.Weight < 1.0f)
                                    if (ite_WP->Weight >= 1.0f)
                                        totalScoreMD_ += 1.0f-tempLine.Weight;
                                    else
                                        totalScoreMD_ += ite_WP->Weight - tempLine.Weight;
                                ite_WP->Weight = tempLine.Weight;
                            }
                        }
                    }
                }
            }

            //sort by ASC ID_
            std::sort(pointWeightListMD_.begin(), pointWeightListMD_.end(),
                pointWeightComp_);
        }

        //--------------------------original list-------------------------------
        for (Simian::u32 i=0; i<totalPointCount_-1; ++i)
        {
            WeightPoint tempLine;
            Simian::Vector2 firstPoint(pointList_[i]);
            Simian::Vector2 secondPoint(pointList_[i+1]);
            Simian::f32 minX = mapPadding_, minY = 0.0f;

            if (SIsFlagSet(gsFlags_, GF_XMAJOR)) //xMajor
            {
                Simian::f32 ratio = (userBoundingBox_.MaxY-userBoundingBox_.MinY)
                    /(userBoundingBox_.MaxX-userBoundingBox_.MinX);
                //shift bigger axis from 0-1 FULL
                firstPoint.X(minX+minLimit+(firstPoint.X() - userBoundingBox_.MinX)/
                    (userBoundingBox_.MaxX-userBoundingBox_.MinX)*sampleLengthP);
                secondPoint.X(minX+minLimit+(secondPoint.X() - userBoundingBox_.MinX)/
                    (userBoundingBox_.MaxX-userBoundingBox_.MinX)*sampleLengthP);
                //shift the smaller axis by same scaling done to bigger axis
                firstPoint.Y(minY+(firstPoint.Y() - (userBoundingBox_.MaxY+userBoundingBox_.MinY)/2)/
                    (userBoundingBox_.MaxY-userBoundingBox_.MinY)*sampleLengthP*ratio);
                secondPoint.Y(minY+(secondPoint.Y() - (userBoundingBox_.MaxY+userBoundingBox_.MinY)/2)/
                    (userBoundingBox_.MaxY-userBoundingBox_.MinY)*sampleLengthP*ratio);
            }
            else
            {
                Simian::f32 ratio = (userBoundingBox_.MaxX-userBoundingBox_.MinX)
                    /(userBoundingBox_.MaxY-userBoundingBox_.MinY);
                //shift bigger axis from 0-1 FULL
                firstPoint.Y(minY+minLimit+(firstPoint.Y() - userBoundingBox_.MinY)/
                    (userBoundingBox_.MaxY-userBoundingBox_.MinY)*sampleLengthP);
                secondPoint.Y(minY+minLimit+(secondPoint.Y() - userBoundingBox_.MinY)/
                    (userBoundingBox_.MaxY-userBoundingBox_.MinY)*sampleLengthP);
                //removed minX
                firstPoint.X((firstPoint.X() - (userBoundingBox_.MaxX+userBoundingBox_.MinX)/2)/
                    (userBoundingBox_.MaxX-userBoundingBox_.MinX)*sampleLengthP*ratio);
                secondPoint.X((secondPoint.X() - (userBoundingBox_.MaxX+userBoundingBox_.MinX)/2)/
                    (userBoundingBox_.MaxX-userBoundingBox_.MinX)*sampleLengthP*ratio);
            }

            Simian::Vector2 originalVecA(firstPoint);
            Simian::Vector2 originalVecB(secondPoint);

            //create box with padding
            if (firstPoint.X() <= secondPoint.X())
            {
                firstPoint.X(minX = firstPoint.X()-boxSize_*samplePadding_);
                secondPoint.X(secondPoint.X()+boxSize_);
            }else{
                firstPoint.X(firstPoint.X()+boxSize_);
                secondPoint.X(minX = secondPoint.X()-boxSize_*samplePadding_);
            }
            if (firstPoint.Y() <= secondPoint.Y())
            {
                firstPoint.Y(minY = firstPoint.Y()-boxSize_*samplePadding_);
                secondPoint.Y(secondPoint.Y()+boxSize_);
            }else{
                firstPoint.Y(firstPoint.Y()+boxSize_);
                secondPoint.Y(minY = secondPoint.Y()-boxSize_*samplePadding_);
            }

            //loop and assign
            Simian::u32 xLoop = static_cast<Simian::u32>
                (std::fabs((firstPoint.X()-secondPoint.X())/boxSize_)+1);
            Simian::u32 yLoop = static_cast<Simian::u32>
                (std::fabs((firstPoint.Y()-secondPoint.Y())/boxSize_)+1);

            for (Simian::u32 j=0; j<xLoop; ++j)
            {
                //within Xspace first
                if (minX+j*boxSize_+boxSize_/2 < minLimit ||
                    minX+j*boxSize_+boxSize_/2 > minLimit+sampleLength)
                {
                    continue;
                }
                for (Simian::u32 k=0; k<yLoop; ++k)
                {
                    //within Yspace first
                    if (minY+k*boxSize_+boxSize_/2 < minLimit ||
                        minY+k*boxSize_+boxSize_/2 > minLimit+sampleLength)
                    {
                        continue;
                    }
                    tempLine.ID = static_cast<Simian::u32>(
                        (minY+k*boxSize_+boxSize_/2-minLimit)/boxSize_) *
                        static_cast<Simian::u32>(sampleLength/boxSize_)
                        + static_cast<Simian::u32>((minX+j*boxSize_+boxSize_/2
                        - minLimit)/boxSize_);
                    tempLine.X = static_cast<Simian::s32>(
                        (minX+j*boxSize_+boxSize_/2-minLimit)/boxSize_);
                    tempLine.Y = static_cast<Simian::s32>(
                        (minY+k*boxSize_+boxSize_/2-minLimit)/boxSize_);
                    //normal |_
                    Simian::Vector2 normal(-(originalVecB.Y()-originalVecA.Y()),
                        (originalVecB.X()-originalVecA.X()));
                    if (!normalizeVec_(normal))
                    {
                        continue; //bad magnitude
                    }
                    //vec(box-P) proj_onto_normal
                    Simian::Vector2 tempVec(
                        minX+j*boxSize_+boxSize_/2-originalVecA.X(),
                        minY+k*boxSize_+boxSize_/2-originalVecA.Y() );
                    //with brush hardness in the core section, 0000...0.0f-1.0f
                    tempLine.Weight = Simian::Math::Max(std::fabs(tempVec.Dot(normal))-
                        brushSize_*brushHardness_, 0.0f)/(brushSize_*decayMultiplier_);
                    //prevents duplicate: check before pushback
                    std::vector<WeightPoint>::iterator ite_WP;
                    ite_WP = std::find(pointWeightList_.begin(), pointWeightList_.end(),
                                       tempLine.ID);
                    //
                    //--------------------------VERIFY--------------------------
                    //
                    if (ite_WP==pointWeightList_.end())
                    {
                        pointWeightList_.push_back(tempLine); //add new ID
                        if (tempLine.Weight < 1.0f)
                            totalScore_ += 1.0f-tempLine.Weight;
                    }else{
                        //assign lesser where 0 = FULL (stroke over)
                        if (ite_WP->Weight >= tempLine.Weight)
                        {
                            if (tempLine.Weight < 1.0f)
                                if (ite_WP->Weight >= 1.0f)
                                    totalScore_ += 1.0f-tempLine.Weight;
                                else
                                    totalScore_ += ite_WP->Weight - tempLine.Weight;
                            ite_WP->Weight = tempLine.Weight;
                        }
                    }
                }
            }
        }
        //sort by ASC ID_
        std::sort(pointWeightList_.begin(), pointWeightList_.end(),
                  pointWeightComp_);
        fsm_.ChangeState(GST_COMPARING);
    }

    void Gesture::Comparing_(Simian::DelegateParameter&)
    {
        //std::cout << "comparing" << std::endl;
        const Simian::f32 skipScore = 2.0f;

        std::vector<WeightPoint>::iterator iteCurrent, iteSearch;
        Simian::f32 scoreMiss = 0.0f, scoreHit = 0.0f, prevScore = 0.0f;

        for (Simian::u32 i=0; i<masterPointWeightList_.size(); ++i)
        {
            //--PUZZLEMODE
            if (((gsFlags_ >> GF_PUZZLEMODE) && 
                !(masterPuzzleIDList_[i] & gsFlags_ >> GF_PUZZLEMODE)) ||
                (!(gsFlags_ >> GF_PUZZLEMODE) && masterCmdList_[i] == 0) ||
                !((GCGestureInterpreter::Instance()->GetSkills().Skills() >> masterCmdList_[i]) & 1))
                continue;

            //OPTIMIZATION: Don't check against same type if passed
            if (masterCmdList_[i] == scoreResult_.Action && scoreResult_.Pass
                && scoreResult_.FinalScore > skipScore)
                continue;

            //------------------------------------------------------------------
            // #TEST&DEBUG: PASSING CRITERIA
            //------------------------------------------------------------------
            //line VS. bolt & others separation
            if ( ((SIsFlagSet(gsFlags_, GF_LINEAR) !=
                SIsFlagSet(masterFlagList_[i], GF_LINEAR))
                && SIsFlagSet(gsFlags_, GF_ANGULAR)) || 
                (!SIsFlagSet(gsFlags_, GF_LINEAR) && //curve vs slash
                SIsFlagSet(masterFlagList_[i], GF_LINEAR)) )
            {
                continue;
            }
            //if not angular, don't check with angular
            //use extra sample for circle/triangle ambuiguity
            if (SIsFlagSet(gsFlags_, GF_ANGULAR) !=
                SIsFlagSet(masterFlagList_[i], GF_ANGULAR) &&
                masterCmdList_[i] != GCMD_HEAL)
                continue;

            //I'm closed, you're not
            if (SIsFlagSet(gsFlags_, GF_CLOSED) !=
                SIsFlagSet(masterFlagList_[i], GF_CLOSED))
                continue;

            //I'm super sharp, but you're not (lightning)
            if ((SIsFlagSet(gsFlags_, GF_ANGULAR2) !=
                SIsFlagSet(masterFlagList_[i], GF_ANGULAR)) &&
                masterCmdList_[i] != GCMD_BOLT)
                continue;

            //----------------------RESET SCORE VARIABLES!!!--------------------
            scoreMiss = 0.0f, scoreHit = 0.0f;

            iteCurrent = masterPointWeightList_[i].begin();
            if (!SIsFlagSet(masterFlagList_[i], GF_MULTIDIR))
            {
                //----------------------SINGLE-DIR INPUT------------------------
                for (Simian::u32 j=0; j<pointWeightList_.size(); ++j)
                {
                    if (pointWeightList_[j].Weight<1.0f)
                    {
                        iteSearch = std::find(iteCurrent,
                            masterPointWeightList_[i].end(), pointWeightList_[j].ID);

                        if (iteSearch == masterPointWeightList_[i].end())
                        {	//CANNOT BE FOUND
                            scoreMiss += 1.0f-pointWeightList_[j].Weight;
                        }else{
                            //found but check if valid non black box
                            if (iteSearch->Weight < 1.0f)
                            {
                                iteCurrent = iteSearch;
                                scoreHit += Simian::Math::Max(
                                    1.0f-pointWeightList_[j].Weight,
                                    1.0f-iteSearch->Weight);
                            }else{
                                scoreMiss +=  1.0f-pointWeightList_[j].Weight;
                            }
                        }
                    }
                }
            }
            else
            {
                //-----------------------MULTI-DIR INPUT------------------------
                for (Simian::u32 j=0; j<pointWeightListMD_.size(); ++j)
                { 
                    if (pointWeightListMD_[j].Weight<1.0f)
                    {
                        iteSearch = std::find(iteCurrent,
                            masterPointWeightList_[i].end(), pointWeightListMD_[j].ID);

                        if (iteSearch == masterPointWeightList_[i].end())
                        {	//CANNOT BE FOUND
                            scoreMiss += 1.0f-pointWeightListMD_[j].Weight;
                        }else{
                            //found but check if valid non black box
                            if (iteSearch->Weight < 1.0f)
                            {
                                iteCurrent = iteSearch;
                                scoreHit += Simian::Math::Max(
                                    1.0f-pointWeightListMD_[j].Weight,
                                    1.0f-iteSearch->Weight);
                            }else{
                                scoreMiss +=  1.0f-pointWeightListMD_[j].Weight;
                            }
                        }
                    }
                }
            }
            //take bigger result
            Simian::f32 result = scoreMiss < GESTURE_EPSILON
                ? scoreHit : scoreHit/scoreMiss;
            result *= scoreHit / masterMaxScoreList_[i];
            if (prevScore < result)
            {
#ifdef G_DEBUGCONSOLE
                std::cout << "LastMatch: "<< i << " Hit/Miss/Max=" << scoreHit <<
                    " " << scoreMiss << " " << masterMaxScoreList_[i] <<
                    " Result= " << result << " PrevScore=" << prevScore << std::endl;
#endif
                //if (scoreResult_.Action == GCMD_SLASH ||
                //    scoreResult_.Action == GCMD_WIND)
                //{
                //    Simian::f32 offsetAngle = std::atan(
                //        (pointList_[0].Y()-pointList_[totalPointCount_-1].Y())/
                //        (pointList_[totalPointCount_-1].X()-pointList_[0].X()) )
                //        / Simian::Math::PI * 180.0f;
                //    if (pointList_[totalPointCount_-1].X()-pointList_[0].X() >= 0
                //        && pointList_[0].Y()-pointList_[totalPointCount_-1].Y() < 0)
                //        offsetAngle += 360.0f;
                //    else
                //        if (pointList_[totalPointCount_-1].X()-pointList_[0].X() <= 0)
                //            offsetAngle += 180.0f;
                //    scoreResult_.Angle = offsetAngle;
                //}

                //get last point, midpoint is useless
                userBoundingBox_.LastX = pointList_[totalPointCount_-1].X();
                userBoundingBox_.LastY = pointList_[totalPointCount_-1].Y();
                scoreResult_.BoundBox = userBoundingBox_;

                bool prevPass = scoreResult_.Pass;
                scoreResult_.Pass = result > resultTolerance_ ? true : false;

                //--------------------------------------------------------------
                // #TEST&DEBUG: PASSING CRITERIA
                //--------------------------------------------------------------
                //SPECIAL CASE: INVERSE BOLT + EXPLOIT OF UTURN
                const Simian::f32 boltExploit = boxSize_*1.5f;
                if (masterCmdList_[i] == GCMD_BOLT)
                {
                    if (pointList_[0].Y() > pointList_[totalPointCount_-1].Y()
                    || (pointList_[totalPointCount_-1].Y() - pointList_[0].Y()
                    < boltExploit*2))
                        scoreResult_.Pass = false;

                    if (pointList_[0].Y() < pointList_[totalPointCount_-1].Y() &&
                    (  (std::abs(pointList_[0].Y()-userBoundingBox_.MinY) > boltExploit
                    || std::abs(pointList_[totalPointCount_-1].Y()-userBoundingBox_.MaxY)
                    > boltExploit) ) )
                        scoreResult_.Pass = false;
                    
                    if (pointList_[0].Y() > pointList_[totalPointCount_-1].Y() &&
                    (  (std::abs(pointList_[0].Y()-userBoundingBox_.MaxY) > boltExploit
                    || std::abs(pointList_[totalPointCount_-1].Y()-userBoundingBox_.MinY)
                    > boltExploit) ) )
                        scoreResult_.Pass = false;

                    //FIX: if previously passed is not bolt, revert
                    if (prevPass && scoreResult_.Action != GCMD_BOLT &&
                        masterCmdList_[i] == GCMD_BOLT && !scoreResult_.Pass)
                        scoreResult_.Pass = true;
                }
                //assign to result
                prevScore = result;
                scoreResult_.Action = masterCmdList_[i];
                scoreResult_.MatchScore = scoreHit;
                scoreResult_.MissScore = scoreMiss;
                scoreResult_.FinalScore = result;

                //--PUZZLE: If 0 and PuzzleMode, use slash effect
                if (masterCmdList_[i] == GCMD_NONE && PuzzleModeID())
                    scoreResult_.Action = GCMD_SLASH;
            }
        }

        fsm_.ChangeState(GST_COMPLETED);
    }

    void Gesture::Complete_(Simian::DelegateParameter&)
    {
        //std::cout << "completing" << std::endl;
#ifdef G_DEBUGCONSOLE
        std::cout << "\n==================================================\n";
        std::cout << "CLOSED=" << SIsFlagSet(gsFlags_, GF_CLOSED) << std::endl;
        std::cout << "LINEAR=" << SIsFlagSet(gsFlags_, GF_LINEAR) << std::endl;
        std::cout << "XMAJOR=" << SIsFlagSet(gsFlags_, GF_XMAJOR) << std::endl;
        std::cout << "ANGULAR=" << SIsFlagSet(gsFlags_, GF_ANGULAR) << std::endl;
        std::cout << "SPIRAL=" << SIsFlagSet(gsFlags_, GF_SPIRAL) << std::endl;
        std::cout << "ANGULAR2=" << SIsFlagSet(gsFlags_, GF_ANGULAR2) << std::endl;
        std::cout << "BestMatch= " << scoreResult_.Action << std::endl;
        std::cout << "FinalOverallScore= " << scoreResult_.FinalScore << std::endl;
        std::cout << "Hit/Miss= " << scoreResult_.MatchScore << "/"
                  << scoreResult_.MissScore << std::endl;
        std::cout << "Pass= " << scoreResult_.Pass << std::endl;
        std::cout << "==================================================\n";
#endif
#ifdef G_DEBUG
        if (Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_CONTROL))
        {
            std::cout << "CTRL(HOLD): OUTPUTING WEIGHT POINTS..." << std::endl;

            SFlagUnset(gsFlags_, GF_MULTIDIR);

            if (Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_Z))
            {
                SFlagSet(gsFlags_, GF_MULTIDIR);
                std::cout << "Gesture>> SAVING AS MULTI-DIRECTIONAL!" << std::endl;
            }

            std::ofstream outputFile;
            std::string filePath(GESTURE_DIRECTORY);
            filePath += "GestureOutput.gs";
            outputFile.open(filePath.c_str(), std::ios_base::out | std::ios_base::trunc);

            if (!outputFile.is_open())
            {
                SAssert(false, "Gesture: Save failed. Check folder permission.");
            }

            if (Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_Z))
            {   //MLUTI-DIR TYPE
                outputFile << std::setprecision(10) << pointWeightListMD_.size() << " "
                << totalScoreMD_ << " " << gsFlags_ << " " << 0 << std::endl;
                for (Simian::u32 i=0; i<pointWeightListMD_.size(); ++i)
                {
                    outputFile << pointWeightListMD_[i].ID << " " <<
                        pointWeightListMD_[i].X << " " << pointWeightListMD_[i].Y <<
                        " " << pointWeightListMD_[i].Weight << std::endl;
                }
            }
            else
            {   //NORMAL TYPE
                outputFile << std::setprecision(10) << pointWeightList_.size() << " "
                << totalScore_ << " " << gsFlags_ << " " << 0 << std::endl;
                for (Simian::u32 i=0; i<pointWeightList_.size(); ++i)
                {
                    outputFile << pointWeightList_[i].ID << " " <<
                        pointWeightList_[i].X << " " << pointWeightList_[i].Y <<
                        " " << pointWeightList_[i].Weight << std::endl;
                }
            }

            //-------------------------SAVE VISUAL DATA-------------------------
            outputFile << "\n==================================================\n";
            outputFile << "CLOSED=" << SIsFlagSet(gsFlags_, GF_CLOSED) << std::endl;
            outputFile << "LINEAR=" << SIsFlagSet(gsFlags_, GF_LINEAR) << std::endl;
            outputFile << "XMAJOR=" << SIsFlagSet(gsFlags_, GF_XMAJOR) << std::endl;
            outputFile << "ANGULAR=" << SIsFlagSet(gsFlags_, GF_ANGULAR) << std::endl;
            outputFile << "SPIRAL=" << SIsFlagSet(gsFlags_, GF_SPIRAL) << std::endl;
            outputFile << "ANGULAR2=" << SIsFlagSet(gsFlags_, GF_ANGULAR2) << std::endl;
            outputFile << "==================================================\n\n";

            std::vector<WeightPoint>::iterator iteSearch, iteEnd;

            if (Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_Z))
                iteEnd = pointWeightListMD_.end();
            else
                iteEnd = pointWeightList_.end();

            for (Simian::u32 i=0; i<mapLength_-1; ++i) //row
            {
                for (Simian::s32 j=static_cast<Simian::s32>(mapLength_-1);
                    j>=0; --j) //column
                {
                    if (Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_Z))
                    {
                        iteSearch = std::find(pointWeightListMD_.begin(),
                        pointWeightListMD_.end(), i*mapLength_ + mapLength_-1-j);
                    }
                    else
                    {
                        iteSearch = std::find(pointWeightList_.begin(),
                        pointWeightList_.end(), i*mapLength_ + mapLength_-1-j);
                    }

                    if (iteSearch == iteEnd)
                    {	//CANNOT BE FOUND
                        outputFile << " ";
                    }else{
                        //found but check if valid non black box
                        if (iteSearch->Weight < 1.0f)
                        {
                            if (iteSearch->Weight < GESTURE_EPSILON)
                            {
                                outputFile << "@";
                                continue;
                            }
                            if (iteSearch->Weight < 0.333f)
                            {
                                outputFile << "O";
                                continue;
                            }
                            if (iteSearch->Weight < 0.666f)
                            {
                                outputFile << "o";
                                continue;
                            }
                            outputFile << "*";
                        }
                        else
                        {
                            outputFile << "-";
                        }
                    }
                }
                outputFile << std::endl;
            }

            outputFile.close();
        }
#endif
        //-------------------------LOG CURRENT SESSION--------------------------
#if defined(G_DEBUGLOG)// | defined(G_USERTEST)
        if (LogOutput)
        {
            std::ofstream outputFile;
            std::string filePath(GESTURE_DIRECTORY);
            filePath += gLogName.str();
            outputFile.open(filePath.c_str(), std::ios_base::out | std::ios_base::app);

            if (!outputFile.is_open())
            {
                SAssert(false, "Gesture: Logging failed. Check folder permission.");
            }

            //-------------------------SAVE VISUAL DATA-------------------------
            outputFile << "\n==================================================\n";
            outputFile << "CLOSED=" << SIsFlagSet(gsFlags_, GF_CLOSED) << std::endl;
            outputFile << "LINEAR=" << SIsFlagSet(gsFlags_, GF_LINEAR) << std::endl;
            outputFile << "XMAJOR=" << SIsFlagSet(gsFlags_, GF_XMAJOR) << std::endl;
            outputFile << "ANGULAR=" << SIsFlagSet(gsFlags_, GF_ANGULAR) << std::endl;
            outputFile << "SPIRAL=" << SIsFlagSet(gsFlags_, GF_SPIRAL) << std::endl;
            outputFile << "ANGULAR2=" << SIsFlagSet(gsFlags_, GF_ANGULAR2) << std::endl;
            outputFile << "BestMatch= " << scoreResult_.Action << std::endl;
            outputFile << "FinalScore= " << scoreResult_.FinalScore << std::endl;
            outputFile << "Hit/Miss= " << scoreResult_.MatchScore << "/"
                       << scoreResult_.MissScore << std::endl;
            outputFile << "Pass= " << scoreResult_.Pass << std::endl;
            outputFile << "==================================================\n\n";

            std::vector<WeightPoint>::iterator iteSearch;
            for (Simian::u32 i=0; i<mapLength_-1; ++i) //row
            {
                for (Simian::s32 j=static_cast<Simian::s32>(mapLength_-1);
                     j>=0; --j) //column
                {
                    iteSearch = std::find(pointWeightList_.begin(),
                        pointWeightList_.end(), i*mapLength_ + mapLength_-1-j);

                    if (iteSearch == pointWeightList_.end())
                    {	//CANNOT BE FOUND
                        outputFile << " ";
                        //std::cout << " ";
                    }else{
                        //found but check if valid non black box
                        if (iteSearch->Weight < 1.0f)
                        {
                            if (iteSearch->Weight < GESTURE_EPSILON)
                            {
                                outputFile << "@";
                                //std::cout << "@";
                                continue;
                            }
                            if (iteSearch->Weight < 0.333f)
                            {
                                outputFile << "O";
                                //std::cout << "O";
                                continue;
                            }
                            if (iteSearch->Weight < 0.666f)
                            {
                                outputFile << "o";
                                //std::cout << "o";
                                continue;
                            }
                            outputFile << "*";
                            //std::cout << "*";
                        }
                        else
                        {
                            outputFile << "-";
                            //std::cout << "-";
                        }
                    }
                }
                outputFile << std::endl;
                //std::cout << std::endl;
            }

            outputFile.close();
        }
#endif
        //calculate last point & push command
        commandVec_.push_back(scoreResult_);

        Reset_();
        fsm_.ChangeState(GST_READY);
    }

    void Gesture::Reset_(void)
    {
        //std::cout << "resetting" << std::endl;
        InitGesture(false);
        //vector clear and resize
        pointList_.clear();
        pointListMD_.clear();
        pointWeightList_.clear();
        pointWeightListMD_.clear();

        pointList_.resize(maxSamplePoints_);
        pointListMD_.resize(maxSamplePoints_);

        fsm_.ChangeState(GST_READY);
    }

    void Gesture::ResetGSFlag_()
    {
        Simian::u32 prevID = PuzzleModeID();
        gsFlags_ = prevID << GF_PUZZLEMODE;
    }

    //function to facilitate re-init
    void Gesture::InitGesture(bool reload)
    {
        bool disabled = SIsFlagSet(gsFlags_, GF_DISABLED);
        //Simian::u32 puzzleID = (gsFlags_ >> GF_PUZZLEMODE);
        //FSM
        fsm_[GST_LOADING].OnUpdate = Simian::Delegate::CreateDelegate<Gesture,
            &Gesture::LoadGestureFile_>(this);
        fsm_[GST_READY].OnUpdate = Simian::Delegate::CreateDelegate<Gesture,
            &Gesture::Ready_>(this);
        fsm_[GST_DRAWING].OnUpdate = Simian::Delegate::CreateDelegate<Gesture,
            &Gesture::Drawing_>(this);
        fsm_[GST_SEPARATING].OnUpdate = Simian::Delegate::CreateDelegate<Gesture,
            &Gesture::Separating_>(this);
        fsm_[GST_ASSIGNING].OnUpdate = Simian::Delegate::CreateDelegate<Gesture,
            &Gesture::Assigning_>(this);
        fsm_[GST_COMPARING].OnUpdate = Simian::Delegate::CreateDelegate<Gesture,
            &Gesture::Comparing_>(this);
        fsm_[GST_COMPLETED].OnUpdate = Simian::Delegate::CreateDelegate<Gesture,
            &Gesture::Complete_>(this);
        fsm_.ChangeState(GST_LOADING);

        ResetGSFlag_();

        scoreResult_.Action = totalPointCount_ = 0;
        lastPos_.X(0); lastPos_.Y(0);
        totalScore_ = totalScoreMD_ = userBoundingBox_.LastX = userBoundingBox_.LastY
            = userBoundingBox_.MaxX = userBoundingBox_.MaxY
            = userBoundingBox_.MinX = userBoundingBox_.MinY
            = userBoundingBoxMD_.LastX = userBoundingBoxMD_.LastY
            = userBoundingBoxMD_.MaxX = userBoundingBoxMD_.MaxY
            = userBoundingBoxMD_.MinX = userBoundingBoxMD_.MinY
            = scoreResult_.MatchScore = scoreResult_.MissScore
            = scoreResult_.BoundBox.LastX = scoreResult_.BoundBox.LastY
            = scoreResult_.BoundBox.MaxX = scoreResult_.BoundBox.MaxY
            = scoreResult_.BoundBox.MinX = scoreResult_.BoundBox.MinY
            = scoreResult_.Angle = 0.0f;
        scoreResult_.Pass = false;
        scoreResult_.Triangle[0] = scoreResult_.Triangle[1] =
            scoreResult_.Triangle[2] = Simian::Vector2();
        
        //enable on init
        if (disabled)
            SFlagSet(gsFlags_, GF_DISABLED);
        //if (puzzleID > 0)
        //    gsFlags_ |= puzzleID << GF_PUZZLEMODE;
        if (!reload)
            return;

        //config file
        std::string filePath(GESTURE_DIRECTORY);
        filePath += "config.xml";

        //XML-Config
        Simian::DataLocation dataLoc(filePath);
        Simian::DataFileIOXML XMLReader;
        XMLReader.Read(dataLoc);

        Simian::FileObjectSubNode data(*XMLReader.Root());
        Simian::FileArraySubNode checkList(*data.Array("checkList"));

        const Simian::FileDataSubNode* sampleFreq = data.Data("sampleFreq_");
        sampleFreq_ = sampleFreq ? sampleFreq->AsF32() : 0;
        const Simian::FileDataSubNode* brushSize = data.Data("brushSize_");
        brushSize_ = brushSize ? brushSize->AsF32() : 0;
        const Simian::FileDataSubNode* decayMultiplier = data.Data("decayMultiplier_");
        decayMultiplier_ = decayMultiplier ? decayMultiplier->AsF32() : 0;
        const Simian::FileDataSubNode* brushHardness = data.Data("brushHardness_");
        brushHardness_ = brushHardness ? brushHardness->AsF32() : 0;
        const Simian::FileDataSubNode* samplePadding = data.Data("samplePadding_");
        samplePadding_ = samplePadding ? samplePadding->AsU32() : 0;
        const Simian::FileDataSubNode* boxSize = data.Data("boxSize_");
        boxSize_ = boxSize ? boxSize->AsF32() : 0;
        const Simian::FileDataSubNode* mapPadding = data.Data("mapPadding_");
        mapPadding_ = mapPadding ? mapPadding->AsF32() : 0;
        const Simian::FileDataSubNode* resultTolerance = data.Data("resultTolerance_");
        resultTolerance_ = resultTolerance ? resultTolerance->AsF32() : 0;
        const Simian::FileDataSubNode* mapLength = data.Data("mapLength_");
        mapLength_ = mapLength ? mapLength->AsU32() : 0;
        const Simian::FileDataSubNode* maxSamplePoints = data.Data("maxSamplePoints_");
        maxSamplePoints_ = maxSamplePoints ? maxSamplePoints->AsU32() : 0;

        std::string subDir, fileName;
        const Simian::FileDataSubNode* sampleSubDirectory = data.Data("sampleSubDirectory");
        subDir = sampleSubDirectory ? sampleSubDirectory->AsString() : 0;

        for (Simian::u32 i=0; i<checkList.Size(); ++i)
        {
            fileName = checkList.Data(i)->AsString();
            masterCheckList_.push_back(subDir+fileName);
        }

        pointList_.resize(maxSamplePoints_);
        pointListMD_.resize(maxSamplePoints_);
        
#ifdef G_DEBUGLOG
        gLogName.str("");
        Simian::DateTime timeStamp;
        DWORD useless = MAX_COMPUTERNAME_LENGTH + 1;
        //get unique id to session
        GetComputerName(g_cName, &useless);
        gLogName << "SessionID_" << static_cast<Simian::u32>(timeStamp.Year())
            << static_cast<Simian::u32>(timeStamp.Month()) <<
            static_cast<Simian::u32>(timeStamp.Day())
            << static_cast<Simian::u32>(timeStamp.Hour()) << 
            static_cast<Simian::u32>(timeStamp.Second()) << "_" <<
            std::string(g_cName) << ".gs";
#endif
        //inet init
#ifdef G_USERTEST
        msgStr.str("");
        LogOutput = true;
        inet.SetConnectionInfo(); //use default param
        msgStr << "Connnecting to server... ";
        if (!inet.Connect())
            msgStr << "Descension/Glogger[ERROR]>> CANNOT ESTABLISH CONNECTION." << std::endl
            << "Help us by sending the log file 'Gestures/Gesture_MinLog.gs' to jason.ngai@digipen.edu" << std::endl
            << "Thank you for your help!" << std::endl;
        else
            msgStr << "Connection established!" << std::endl << "***Welcome to Descension!***" << std::endl;
#endif
        
    }

    //function to facilitate exiting
    void Gesture::ClearAllVectors_(void)
    {
#ifdef G_USERTEST
        msgStr << "Disconnecting..." << std::endl;
        inet.Disconnect();
#endif

        commandVec_.clear();

        pointList_.clear();
        pointListMD_.clear();
        pointWeightList_.clear();
        pointWeightListMD_.clear();

        masterCheckList_.clear();

        masterPointWeightList_.clear();
        masterMaxScoreList_.clear();
        masterFlagList_.clear();
        masterCmdList_.clear();

        masterPuzzleIDList_.clear();
    }

    bool Gesture::pointWeightComp_(const WeightPoint &lhs, const WeightPoint &rhs)
    {
        return (lhs.ID < rhs.ID);
    }

    //--------------------------------------------------------------------------
    // Math methods
    //--------------------------------------------------------------------------
    //safer normalization with EPSILON check
    bool Gesture::normalizeVec_(Simian::Vector2 &vec)
    {
        Simian::f32 magnitude = vec.Length();

        if (magnitude <= GESTURE_EPSILON)
        {
            return false; //division by 0, ERROR
        }

        vec.X(vec.X()/magnitude);
        vec.Y(vec.Y()/magnitude);

        return true;
    }

    //rotate vector by rotation matrix
    void Gesture::RotateVec2(Simian::Vector2 &vec, const Mtx2x2 &matrix)
    {
        vec = Simian::Vector2(matrix.A*vec.X() + matrix.B*vec.Y(),
                              matrix.C*vec.X() + matrix.D*vec.Y());
    }
}
