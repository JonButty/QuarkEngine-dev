/*************************************************************************/
/*!
\file		GestureTutor.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/11/30
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCGestureInterpreter.h"
#include "GestureTutor.h"

#include "Simian/DataFileIO.h"
#include "Simian/FileSystem.h"
#include "Simian/Interpolation.h"
#include "Simian/Game.h"
#include "Simian/LogManager.h"
#include "Simian/Math.h"

#include <string>

namespace Descension {
    Simian::f32 GestureTutor::aniTime_ = 0.0f, GestureTutor::delayTime_ = 0.0f,
                GestureTutor::DELAY_TIME = 2.0f;
    Simian::u32 GestureTutor::drawType_ = GestureTutor::GT_TOTAL,
                GestureTutor::frameID_ = 0;
    std::vector<std::vector<GestureTutor::GesturePath> > GestureTutor::pathVec_;

    Simian::Vector2 GestureTutor::lastPos_;

    std::string GestureTutor::PATH_DIRECTORY = "Gestures/GestureTutorPath.xml";

    void GestureTutor::DrawType(Simian::u32 val)
    {
        //--Once type is initialized, animation time is also reset
        drawType_ = val;
        aniTime_ = 0.0f;
        frameID_ = 0;
        GCGestureInterpreter::Instance()->ExitCasting_();
    }

    Simian::u32 GestureTutor::DrawType(void)
    {
        return drawType_;
    }

    void GestureTutor::DrawGesture(Simian::f32 dt)
    {
        //--Not drawing if unset
        if (drawType_ == GT_TOTAL)
            return;

        //--Cancel drawing if frame has reached the end
        if (frameID_ >= pathVec_[drawType_].size())
        {
            delayTime_ += dt;
            if (delayTime_ >= DELAY_TIME)
                drawType_ = GT_TOTAL;
            return;
        }

        //--Major axis, uhh just gonna HC this
        Simian::f32 screenHt = Simian::Game::Instance().Size().Y();
        Simian::Vector2 pos;
        //--Easier reference
        GesturePath& path = pathVec_[drawType_][frameID_];

        //--Start from FRAME#0 ALWAYS!
        if (frameID_ == 0)
        {
            pos = path.Position * screenHt;
            GCGestureInterpreter::Instance()->AddQuad(pos);
            GCGestureInterpreter::Instance()->SetLastQuad(pos);
            lastPos_ = pos;
            delayTime_ = 0.0f;
            ++frameID_;
            return;
        }

        //--This is safe since FRAME#0 returns above
        GesturePath& prevPath = pathVec_[drawType_][frameID_-1];

        //--For interpolation with parametrized equation
        aniTime_ += dt/pathVec_[drawType_][pathVec_[drawType_].size()-1].Timer;
        //aniTime_ = Simian::Math::Clamp<Simian::f32>(aniTime_, 0.0f, 1.0f);
        Simian::f32 t = (aniTime_-prevPath.Timer)/(path.Timer-prevPath.Timer);
        t = Simian::Math::Clamp<Simian::f32>(t, 0.0f, 1.0f);
        pos.X(prevPath.Position.X()+t*(path.Position.X()-prevPath.Position.X()));
        pos.Y(prevPath.Position.Y()+t*(path.Position.Y()-prevPath.Position.Y()));
        pos *= screenHt; //--Get it to device coordinate

        //--Add a quad when the min_dist is met for spacing
        if ((pos-lastPos_).LengthSquared() >= GCGestureInterpreter::MIN_PT_DIST
            *(screenHt/768.0f))
        {
            lastPos_ = pos;
            GCGestureInterpreter::Instance()->AddQuad(pos);
        }

        GCGestureInterpreter::Instance()->SetLastQuad(pos);

        //--Current FRAME# is the destination in the array index
        if (t >= 1.0f)
            ++frameID_;
    }

    void GestureTutor::LoadPath(void)
    {
        //--Always clear since it's uhh 1 draw a time
        pathVec_.clear();
        
        Simian::DataLocation dataLoc(PATH_DIRECTORY);
        Simian::DataFileIODefault XMLReader;
        XMLReader.Read(dataLoc);
        const Simian::FileObjectSubNode* root = XMLReader.Root(); //--Root

        std::ifstream fstr(PATH_DIRECTORY);
        if (!fstr.is_open())
        {
            SError("GestureTutor: Cannot find file named: " << PATH_DIRECTORY);
            SAssert(false, "GestureTutor: Cannot find needed file.");
        }
        fstr.close();

        const Simian::FileArraySubNode* gestureTutorPath = root->Array("GestureTutorPath");
        root->Data("Delay", DELAY_TIME, DELAY_TIME);

        if (gestureTutorPath->Size() < GT_TOTAL)
        {
            SError("GestureTutor: Expected frame size not met from: " << PATH_DIRECTORY);
            SAssert(false, "GestureTutor: GesturePath size mismatch, unexpected behaviour may occur.");
        }

        std::vector<GesturePath> insertVec;
        GesturePath readPath;

        for (Simian::u32 i=0; i<gestureTutorPath->Size(); ++i)
        {
            const Simian::FileArraySubNode* gestureType = gestureTutorPath->Array(i);

            for (Simian::u32 j=0; j<gestureType->Size(); ++j)
            {
                const Simian::FileObjectSubNode* nodeB = gestureType->Object(j);

                readPath.Timer = 0.0f;
                Simian::f32 f = 0.0f;

                nodeB->Data("Time", readPath.Timer);
                nodeB->Data("X", f);
                readPath.Position.X(f);
                nodeB->Data("Y", f);
                readPath.Position.Y(f);

                insertVec.push_back(readPath);
            }
            //--Yo'dawg, I herd yew liek vectors so I insert a vector into a vector.
            pathVec_.push_back(insertVec);
            insertVec.clear();
        }
    }
}
