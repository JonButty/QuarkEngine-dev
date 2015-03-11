/*************************************************************************/
/*!
\file		GCGestureInterpreter.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
//#define G_DEBUGHELPER //assist gesture in message printing
//#define G_DEBUG_VISUAL //hold shift and press C to read in minLog

#include "GCGestureInterpreter.h"
#include "GCGesturePuzzle.h"
#include "GCAttributes.h"
#include "CombatCommandDealDamage.h"
#include "CombatCommandSystem.h"
#include "GCPlayerLogic.h"
#include "GCPhysicsController.h"
#include "ComponentTypes.h"
#include "EntityTypes.h"
#include "GCFlameEffect.h"
#include "GCEditor.h"
#include "GCUI.h"
#include "GCUpgradeExchange.h"
#include "GCFadeSpriteEffect.h"
#include "GestureTutor.h"

#include "SaveManager.h"

#include "Simian/Ray.h"
#include "Simian/Plane.h"
#include "Simian/DebugRenderer.h"
#include "Simian/Game.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/RenderQueue.h"
#include "Simian/IndexBuffer.h"
#include "Simian/Interpolation.h"
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/Mouse.h"
#include "Simian/Keyboard.h"
#include "Simian/Scene.h"
#include "Simian/CSoundEmitter.h"
#include "Simian/Math.h"
#include "Simian/FileSystem.h"
#include "Simian/LogManager.h"

#include <fstream>
#include "GCParticleEmitterTriangle.h"

namespace Descension
{
    //const statics
    const Simian::u32 GCGestureInterpreter::MAX_VBSIZE = 1024,
        GCGestureInterpreter::VB_DIV = 16, //4*4
        GCGestureInterpreter::JOINT_COUNT = 5; //1024: DIV by 12
    const Simian::f32 GCGestureInterpreter::LINE_WIDTH = 16.0f,
        GCGestureInterpreter::MIN_PT_DIST = 18*18, //use square
        GCGestureInterpreter::INK_DECREMENT = 30.0f, //per diagonal
        GCGestureInterpreter::SLOW_MULTIPLIER = 0.1f, //half of width
        GCGestureInterpreter::JOINT_ANGLE = 0.75f,
        GCGestureInterpreter::JOINT_ANGLE2 = 0.25f, //cosAngle
        GCGestureInterpreter::U_OFFSET = 0.05f,
        GCGestureInterpreter::LOWESTY = -0.5f;
    GCGestureInterpreter* GCGestureInterpreter::pGestureInterpreter;

    static const Simian::f32 TRINE_DELAY = 2.0f;

    bool GCGestureInterpreter::newStart = false;

    Gesture GCGestureInterpreter::gestureClass_;

    Simian::FactoryPlant<Simian::EntityComponent, GCGestureInterpreter>
        GCGestureInterpreter::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_GESTUREINTERPRETER);

    void GCGestureInterpreter::Update(Simian::f32 dt)
    {
        GCAttributes* pAttribs = GCPlayerLogic::Instance()->Attributes();

        if (PuzzleModeID() != GCGesturePuzzle::GPUZZLE_NONE)
            pAttribs->Mana(pAttribs->MaxMana());

        if (status_ == GI_READY)
            GestureEnabled(Simian::Mouse::IsPressed(Simian::Mouse::KEY_RBUTTON)
            && pAttribs->Mana() > 1.0f && pAttribs->Health() > 0);

        //get command and check result
        Gesture::GestureResult result;

        while (!Simian::Mouse::IsPressed(Simian::Mouse::KEY_RBUTTON) &&
            gestureClass_.GetCommand(result))
            ProcessGesture(result);

        gestureClass_.Update(dt); //update gestureClass

        // update active blazing trines
        updateBlazingTrines_(dt_);

        if (!gestureClass_.GetEnabled() && resultVec_.size() > 0)
        {
            ActivateGesture(); //unleash the force
        } else {
            if (gestureClass_.GetState()==Gesture::GST_DRAWING
                && Simian::Mouse::OnMouseMove() && gestureClass_.CanDraw()
                && Available())
            {
                Simian::Vector2 mouseVec = Simian::Mouse::GetMouseClientPosition();
                Simian::Vector2 winSize(Simian::Game::Instance().Size());
                mouseVec.X(mouseVec.X()-winSize.X()/2.0f);
                mouseVec.Y(-(mouseVec.Y()-winSize.Y()/2.0f));
                static Simian::Vector2 lastMouse; //--last mouse position
                if (Simian::Mouse::IsTriggered(Simian::Mouse::KEY_RBUTTON))
                    lastMouse = mouseVec;
                Simian::f32 sqAngle = 1-currAngle_;
                if (samplePoints_.size() == 0 || //new entry mode
                    (samplePoints_[samplePoints_.size()-1]-mouseVec).LengthSquared()
                    >= MIN_PT_DIST*(1+sqAngle*sqAngle)
                    || newStrip_ == samplePoints_.size()+1)
                {
                    if (samplePoints_.size() != 0 && newStrip_ != samplePoints_.size()+1)
                        SetLastQuad(mouseVec);
                    AddQuad(mouseVec);
                }

                SetLastQuad(mouseVec); //draw quads
                Simian::f32 decrement = (lastMouse- mouseVec).Length()*INK_DECREMENT/
                    winSize.Length();
                pAttribs->Mana(pAttribs->Mana() - decrement);
                mpCost_ += decrement;
                lastMouse = mouseVec; //--update last position for next loop
            }

            if (Simian::Mouse::IsPressed(Simian::Mouse::KEY_RBUTTON) &&
                pAttribs->Mana() <= 1.0f)
            {
                if (msgStatus_ != GI_MSGID_NOMP)
                    SwapMessage_(GI_MSGID_NOMP);
            }

            if (!Simian::Mouse::IsPressed(Simian::Mouse::KEY_RBUTTON))
                newStrip_ = samplePoints_.size()+1;

            if (PuzzleModeID() != GCGesturePuzzle::GPUZZLE_NONE)
                pAttribs->Mana(pAttribs->MaxMana());

            animateUV_(); //aura energy
            slashEffect_(dt);
        }
    }

    //process a valid command
    void GCGestureInterpreter::ProcessGesture(Gesture::GestureResult& result)
    {
        GCAttributes* pAttribs = GCPlayerLogic::Instance()->Attributes();
        Simian::u32 gCol = 0xFF777777;

        if (result) //PASS
        {
            if (gestureClass_.PuzzleModeID())
            {
                if (!GCGesturePuzzle::IsPuzzleUnlocked(gestureClass_.PuzzleModeID())
                    && GCGesturePuzzle::IsActive())
                {
                    SwapMessage_(GI_MSGID_FAIL);
                    ExitCasting_();
                    return;
                }
                puzzleSolved_ = true;
                if (result.Action >= Gesture::GCMD_TOTAL)
                    return;
            }

            if (!gestureSkills_.CheckSkill(result.Action)) { //--but not avail
                ExitCasting_();
                lastVB_ = vbSize_;
                mpCost_ = 0.0f;
                return;
            }
            switch (result.Action)
            {
            case Gesture::GCMD_SLASH: gCol = 0xFFFFFFFF;
                dmgMultiplier_[pAttribs->ST_PHYSICAL] *= pAttribs->SpellContMultiplier();
                pAttribs->Mana(pAttribs->Mana() - pAttribs->SpellCost(pAttribs->ST_PHYSICAL)
                    *dmgMultiplier_[pAttribs->ST_PHYSICAL]);
                break;
            case Gesture::GCMD_BOLT: gCol = 0xFFFFFF00;
                dmgMultiplier_[pAttribs->ST_LIGHTNING] *= pAttribs->SpellContMultiplier();
                pAttribs->Mana(pAttribs->Mana() - pAttribs->SpellCost(pAttribs->ST_LIGHTNING)
                    *dmgMultiplier_[pAttribs->ST_LIGHTNING]);
                break;
            case Gesture::GCMD_FIRE: gCol = 0xFFFF3311;
                dmgMultiplier_[pAttribs->ST_FIRE] *= pAttribs->SpellContMultiplier();
                pAttribs->Mana(pAttribs->Mana() - pAttribs->SpellCost(pAttribs->ST_FIRE)
                    *dmgMultiplier_[pAttribs->ST_FIRE]);
                break;
            case Gesture::GCMD_HEAL: gCol = 0xFF55FF55;
                dmgMultiplier_[pAttribs->ST_HEALING] *= pAttribs->SpellContMultiplier();
                pAttribs->Mana(pAttribs->Mana() - pAttribs->SpellCost(pAttribs->ST_HEALING)
                    *dmgMultiplier_[pAttribs->ST_HEALING]);
                break;
            case Gesture::GCMD_ICE: gCol = 0xFF00FFFF;
                dmgMultiplier_[pAttribs->ST_ICE] *= pAttribs->SpellContMultiplier();
                pAttribs->Mana(pAttribs->Mana() - pAttribs->SpellCost(pAttribs->ST_ICE)
                    *dmgMultiplier_[pAttribs->ST_ICE]);
                break;
            case Gesture::GCMD_WIND: gCol = 0xFFccFFcc;
                dmgMultiplier_[pAttribs->ST_WIND] *= pAttribs->SpellContMultiplier();
                pAttribs->Mana(pAttribs->Mana() - pAttribs->SpellCost(pAttribs->ST_WIND)
                    *dmgMultiplier_[pAttribs->ST_WIND]);
                break;
            case Gesture::GCMD_EARTH: gCol = 0xFF662200;
                dmgMultiplier_[pAttribs->ST_EARTH] *= pAttribs->SpellContMultiplier();
                pAttribs->Mana(pAttribs->Mana() - pAttribs->SpellCost(pAttribs->ST_EARTH)
                    *dmgMultiplier_[pAttribs->ST_EARTH]);
                break;
            case Gesture::GCMD_DARK: gCol = 0xFF5500bb;
                dmgMultiplier_[pAttribs->ST_SHADOW] *= pAttribs->SpellContMultiplier();
                pAttribs->Mana(pAttribs->Mana() - pAttribs->SpellCost(pAttribs->ST_SHADOW)
                    *dmgMultiplier_[pAttribs->ST_SHADOW]);
                break;
            default: break;
            }
            //--------------change colour & add to shatter effect---------------
            for (Simian::u32 i=0; i<vbSize_/2; ++i)
                vertices_[i*2+1].Diffuse = vertices_[i*2].Diffuse = gCol;
            //----------------------------SHATTER-------------------------------
            if (result.Action == Gesture::GCMD_SLASH)
            {
                for (Simian::u32 k=0; k<2; ++k) //repeats of shatter density
                {
                    Simian::s32 loopCount = vbSize_/4, maxVel = 75, minVel = 25, div = 25;
                    std::vector<Simian::f32> randVec;
                    for (Simian::s32 i=0; i<loopCount; ++i) { // xVel and yVel
                        Simian::f32 tempRand = Simian::Math::Random(0, 2) == 0 ? //neg or pos
                            static_cast<Simian::f32>(Simian::Math::Random(-maxVel, -minVel)/div) :
                        static_cast<Simian::f32>(Simian::Math::Random(minVel, maxVel)/div);
                        for (Simian::u32 j=0; j<8; ++j) {
                            randVec.push_back(tempRand + (Simian::Math::Random(0, 2) == 0 ? //neg or pos
                                static_cast<Simian::f32>(Simian::Math::Random(-maxVel, -minVel)/div/1.25f) :
                            static_cast<Simian::f32>(Simian::Math::Random(minVel, maxVel)/div/1.25f)) );
                        }
                    }
                    slashShatterRand_.push_back(randVec);
                    slashShatter_.push_back(vertices_);
                    slashShatterSize_.push_back(vbSize_);
                }
            }
            for (Simian::u32 i=0; i<vbSize_/2; ++i)
            {
                if (i%2)
                {
                    vertices_[i*2+1].U = 0.5f-U_OFFSET;
                    vertices_[i*2].U = 0.5f+U_OFFSET;
                }else{
                    vertices_[i*2].U = 0.5f-U_OFFSET;
                    vertices_[i*2+1].U = 0.5f+U_OFFSET;
                }
            }

            //------------------------copy and delete---------------------------
            outgoing_.push_back(vertices_);
            outgoingSize_.push_back(vbSize_);
            outgoingFullSize_.push_back(vbSize_);

#ifdef G_DEBUGHELPER
            Gesture::msgStr.str("");
            Gesture::msgStr << "Great drawing, good job!" << std::endl;
#endif
            //push to resultVec_
            resultVec_.push_back(result);
        }
        else //FAILED
        {
#ifdef G_DEBUGHELPER
            Gesture::msgStr.str("");
            Gesture::msgStr << "The recognition system could not recognize the last drawing...\n"
                << std::endl;
#endif
            pAttribs->Mana(pAttribs->Mana() + mpCost_);
            Simian::u32 bitFlag = GCGestureInterpreter::Instance()->GetSkills().Skills();
            if ((bitFlag & 2) && (bitFlag>>2 == 0))
            {
                SwapMessage_(GI_MSGID_STRAIGHT);
            } else {
                SwapMessage_(GI_MSGID_FAIL);
            }

            //--------------change colour & add to unwind effect---------------
            Simian::u32 countMax = vbSize_/2;
            for (Simian::u32 i=0; i<countMax; ++i)
            {
                vertices_[i*2+1].Diffuse = vertices_[i*2].Diffuse = 0xFFccccFF;
                if (i%2)
                {
                    vertices_[i*2+1].U = 0.5f-U_OFFSET;
                    vertices_[i*2].U = 0.5f+U_OFFSET;
                } else {
                    vertices_[i*2].U = 0.5f-U_OFFSET;
                    vertices_[i*2+1].U = 0.5f+U_OFFSET;
                }
            }
            outgoing_.push_back(vertices_);
            outgoingSize_.push_back(UnwindSet(vbSize_, true));
            outgoingFullSize_.push_back(vbSize_);
        }

        ExitCasting_();
        lastVB_ = vbSize_;
        mpCost_ = 0.0f;
    }

    void GCGestureInterpreter::SetSkills(const GestureSkills& val)
    {
        if ((gestureSkills_.Skills() & 1 << Gesture::GCMD_NONE) && 
            !(val.Skills() & (1 << Gesture::GCMD_NONE) ))
        {
            gestureSkills_.Skills(val.Skills() + (1<<Gesture::GCMD_NONE));
        } else {
            gestureSkills_.Skills(val.Skills());
        }
    }

    //activate command
    void GCGestureInterpreter::ActivateGesture(void)
    {
        //reset array
        for (Simian::u32 i=0; i<GCAttributes::ST_TOTAL; ++i)
            dmgMultiplier_[i] = 1.0f;

        //if no enemies
        if (allEnemyVec_->size() == 0)
            return;

        //player attribs
        GCAttributes* pAttribs = GCPlayerLogic::Instance()->Attributes();

        for (Simian::u32 i=0; i < resultVec_.size(); ++i)
        {
            //generate bounding box vectors
            Simian::Vector2 boundingVec[4] =
            {
                Simian::Vector2(resultVec_[i].BoundBox.MinX, -resultVec_[i].BoundBox.MinY),
                Simian::Vector2(resultVec_[i].BoundBox.MinX, -resultVec_[i].BoundBox.MaxY),
                Simian::Vector2(resultVec_[i].BoundBox.MaxX, -resultVec_[i].BoundBox.MinY),
                Simian::Vector2(resultVec_[i].BoundBox.MaxX, -resultVec_[i].BoundBox.MaxY),
            };

            Simian::Vector3 boundingVec3D[4];

            //get normalized co-ord
            for (Simian::u32 j=0; j<4; ++j)
            {
                boundingVec[j] /= Simian::Game::Instance().Size()*0.5f;

                if (!Simian::Plane::Z.Intersect(
                    Simian::Ray::FromProjection(cameraTransform_->World(),
                    camera_->Camera().Projection(), boundingVec[j]), boundingVec3D[j]))
                {
                    //continue if fail: not intersecting
                    resultVec_.clear();
                    return;
                }
            }

            //---------------------PROCESS GESTURE--------------------------
            Simian::f32 boltRadius = 0.5f, lineHitDist = 0.25f;
            Simian::f32 hitRadius = 0.0f;

            //----------------BOLT: strike single only----------------------
            if (resultVec_[i].Action == Gesture::GCMD_BOLT)
            {
                Simian::Vector2 boltVec(resultVec_[i].BoundBox.LastX,
                    -resultVec_[i].BoundBox.LastY);
                boltVec /= Simian::Game::Instance().Size()*0.5f;

                Simian::Vector3 boltVec3D;
                if (!Simian::Plane::Z.Intersect(
                    Simian::Ray::FromProjection(cameraTransform_->World(),
                    camera_->Camera().Projection(), boltVec), boltVec3D))
                {
                    continue;
                }

                Simian::f32 minDist = 0.0f, newDist = 0.0f;
                GCAIBase* target = NULL;

                //get shortest distance guy
                for (Simian::u32 k=0; k<allEnemyVec_->size(); ++k)
                {
                    Simian::Vector3 ePos((*allEnemyVec_)[k]->Physics()->Position());
                    newDist = (ePos-boltVec3D).LengthSquared();

                    if ( k==0 )
                    {
                        target = (*allEnemyVec_)[k];
                        minDist = newDist;
                        continue;
                    }

                    if ( newDist < minDist &&
                        (*allEnemyVec_)[k]->Attributes()->Health() > 0)
                    {
                        target = (*allEnemyVec_)[k];
                        minDist = newDist;
                    }
                }
                //check pass and damage
                if (minDist < (boltRadius+target->Physics()->Radius())
                    * (boltRadius+target->Physics()->Radius()) &&
                    target->Attributes()->Health() > 0 &&
                    target->Physics()->Position().Y() >= LOWESTY)
                {
                    GCAttributes* eAttribs = target->Attributes();
                    CombatCommandSystem::Instance().AddCommand(pAttribs, eAttribs,
                        CombatCommandDealDamage::Create(pAttribs->SpellDamage(pAttribs->ST_LIGHTNING)));
                }

                //--------------------------------------------------------------
                Simian::Entity* effect = ParentScene()->CreateEntity(E_LIGHTNINGEFFECT);
                Simian::CTransform* transform;
                effect->ComponentByType(Simian::C_TRANSFORM, transform);
                transform->Position(boltVec3D);
                ParentScene()->AddEntity(effect);
                //--------------------------------------------------------------

                continue;
            }

            //-----------------------Closed: Use Centers------------------------
            if (resultVec_[i].Action == Gesture::GCMD_FIRE)
            {
                BlazingTrine trine;

                //--------------------------------------------------------------
                Simian::Vector3 (&triangle)[3] = trine.Points;//--TOP LEFT RIGHT
                bool breakThis = false;
                for (Simian::u32 j=0; j<3; ++j) //--FIX negated Y-axis
                {
                    resultVec_[i].Triangle[j].Y(-resultVec_[i].Triangle[j].Y());
                    resultVec_[i].Triangle[j] /= Simian::Game::Instance().Size()*0.51f;
                    if (!Simian::Plane::Z.Intersect(
                        Simian::Ray::FromProjection(cameraTransform_->World(),
                        camera_->Camera().Projection(), resultVec_[i].Triangle[j]), triangle[j]))
                    {
                        breakThis = true;
                        break;
                    }
                }
                if (breakThis)
                    continue;

                if (sounds_)
                    sounds_->Play(GCPlayerLogic::S_CAST_FIRE_CRACKLE);

                Simian::Entity* effect = ParentScene()->CreateEntity(E_EMBERSEFFECT);
                
                Simian::Entity* child = effect->ChildByName("Smoke");
                GCParticleEmitterTriangle* triangleEmitter;
                child->ComponentByType(GC_PARTICLEEMITTERTRIANGLE, triangleEmitter);
                triangleEmitter->Vertices(triangle);

                child = effect->ChildByName("Floaty");
                child->ComponentByType(GC_PARTICLEEMITTERTRIANGLE, triangleEmitter);
                triangleEmitter->Vertices(triangle);

                ParentScene()->AddEntity(effect);

                //--------------------------------------------------------------

                Simian::Vector2 ctrVec(
                    (resultVec_[i].BoundBox.MinX+resultVec_[i].BoundBox.MaxX)/2.0f,
                    -(resultVec_[i].BoundBox.MinY+resultVec_[i].BoundBox.MaxY)/2.0f);
                ctrVec /= Simian::Game::Instance().Size()*0.5f;

                //get new hit radius from average box length
                hitRadius = ( std::abs(boundingVec3D[3].X()-boundingVec3D[0].X())
                    + std::abs(boundingVec3D[3].Z()-boundingVec3D[0].Z()) / 5.0f);

                Simian::Vector3 ctrVec3D;
                if (!Simian::Plane::Z.Intersect(
                    Simian::Ray::FromProjection(cameraTransform_->World(),
                    camera_->Camera().Projection(), ctrVec), ctrVec3D))
                    continue;

                trine.Center = ctrVec3D;
                trine.Radius = hitRadius;
                trine.Timer = 0.0f;

                blazingTrines_.push_back(trine);
            }

            if (resultVec_[i].Action != Gesture::GCMD_SLASH &&
                resultVec_[i].Action != Gesture::GCMD_WIND &&
                resultVec_[i].Action != Gesture::GCMD_FIRE)
            {
                Simian::Vector2 ctrVec(
                    (resultVec_[i].BoundBox.MinX+resultVec_[i].BoundBox.MaxX)/2.0f,
                    -(resultVec_[i].BoundBox.MinY+resultVec_[i].BoundBox.MaxY)/2.0f);
                ctrVec /= Simian::Game::Instance().Size()*0.5f;

                //get new hit radius from average box length
                hitRadius = ( std::abs(boundingVec3D[3].X()-boundingVec3D[0].X())
                    + std::abs(boundingVec3D[3].Z()-boundingVec3D[0].Z()) / 5.0f);

                Simian::Vector3 ctrVec3D;
                if (!Simian::Plane::Z.Intersect(
                    Simian::Ray::FromProjection(cameraTransform_->World(),
                    camera_->Camera().Projection(), ctrVec), ctrVec3D))
                {
                    continue;
                }

                //hit all the guys inside radius
                for (Simian::u32 k=0; k<allEnemyVec_->size(); ++k)
                {
                    Simian::Vector3 ePos((*allEnemyVec_)[k]->Physics()->Position());
                    Simian::f32 dist = (ePos-ctrVec3D).LengthSquared();

                    //hit if within range
                    if ((*allEnemyVec_)[k]->Attributes()->Health() > 0
                        && dist < (hitRadius+(*allEnemyVec_)[k]->Physics()->Radius())
                        * (hitRadius+(*allEnemyVec_)[k]->Physics()->Radius()) &&
                        (*allEnemyVec_)[k]->Physics()->Position().Y() >= LOWESTY)
                    {
                        GCAttributes* eAttribs = (*allEnemyVec_)[k]->Attributes();

                        CombatCommandSystem::Instance().AddCommand(pAttribs, eAttribs,
                            CombatCommandDealDamage::Create(pAttribs->SpellDamage(pAttribs->ST_EARTH)));
                    }
                }

                continue;
            }

            //----------------------------Line: DDA-----------------------------
            if (resultVec_[i].Action == Gesture::GCMD_SLASH ||
                resultVec_[i].Action == Gesture::GCMD_WIND)
            {
                if (sounds_)
                    sounds_->Play(Simian::Math::Random(
                        GCPlayerLogic::S_CAST_SLASH_1, GCPlayerLogic::S_CAST_SLASH_1 + GCPlayerLogic::TOTAL_SLASH_SOUNDS));

                Simian::Vector3 posOffset(0.4f, 0.0f, 0.4f);
                //get line vector and normal
                Simian::Vector3 lineVec( boundingVec3D[3] - boundingVec3D[0] );
                Simian::Vector3 p0(boundingVec3D[0]), p1(boundingVec3D[3]);

                //other slope: "positive" /
                if (( resultVec_[i].BoundBox.LastY < (resultVec_[i].BoundBox.MaxY
                    +resultVec_[i].BoundBox.MinY)/2.0f &&
                    resultVec_[i].BoundBox.LastX > (resultVec_[i].BoundBox.MaxX
                    +resultVec_[i].BoundBox.MinX)/2.0f ) ||
                    ( resultVec_[i].BoundBox.LastY > (resultVec_[i].BoundBox.MaxY
                    +resultVec_[i].BoundBox.MinY)/2.0f &&
                    resultVec_[i].BoundBox.LastX < (resultVec_[i].BoundBox.MaxX
                    +resultVec_[i].BoundBox.MinX)/2.0f ))
                {
                    lineVec = boundingVec3D[2] - boundingVec3D[1];
                    p0 = boundingVec3D[1];
                    p1 = boundingVec3D[2];
                }

                if (resultVec_[i].Action == Gesture::GCMD_WIND)
                {
                    p0 = (boundingVec3D[1]+boundingVec3D[0])/2;
                    p1 = (boundingVec3D[3]+boundingVec3D[2])/2;
                    lineVec = p1 - p0;
                    lineHitDist += 0.25f;
                }

                Simian::Vector3 lineNormal(-lineVec.Z(), lineVec.Y(), lineVec.X());
                lineNormal.Normalize();

                //hit all the guys inside radius
                for (Simian::u32 k=0; k<allEnemyVec_->size(); ++k)
                {
                    posOffset = Simian::Vector3((*allEnemyVec_)[k]->Physics()->Radius(),
                                0, (*allEnemyVec_)[k]->Physics()->Radius());
                    Simian::Vector3 ePos(posOffset +
                        (*allEnemyVec_)[k]->Physics()->Position());
                    //within finite line
                    if ( !((ePos-p0).Dot(lineVec) >= 0.0f &&
                        (ePos-p1).Dot(-lineVec) >= 0.0f) &&
                         !((ePos-p0-posOffset).Dot(lineVec) >= 0.0f &&
                        (ePos-p1-posOffset).Dot(-lineVec) >= 0.0f) )
                        continue;
                    //hit if within range
                    if ((*allEnemyVec_)[k]->Attributes()->Health() > 0
                        && ( std::abs((ePos-p0).Dot(lineNormal)) <
                        lineHitDist + (*allEnemyVec_)[k]->Physics()->Radius() ||
                        std::abs(((*allEnemyVec_)[k]->Physics()->Position()-p0).Dot(lineNormal))
                        < lineHitDist + (*allEnemyVec_)[k]->Physics()->Radius()) &&
                        (*allEnemyVec_)[k]->Physics()->Position().Y() >= LOWESTY)
                    {
                        GCAttributes* eAttribs = (*allEnemyVec_)[k]->Attributes();
                        CombatCommandSystem::Instance().AddCommand(pAttribs, eAttribs,
                            CombatCommandDealDamage::Create(pAttribs->SpellDamage(pAttribs->ST_PHYSICAL)));
                    }
                }

                continue;
            }
        }
        //dump processing
        resultVec_.clear();
    }

    bool GCGestureInterpreter::GestureEnabled(void)
    {
        return gestureClass_.GetEnabled();
    }

    bool GCGestureInterpreter::Available(void)
    {
        return vbSize_ < MAX_VBSIZE-1-JOINT_COUNT*4-4;
    }

    void GCGestureInterpreter::LineDepth(Simian::f32 val)
    {
        lineDepth_ = val;
    }
    Simian::f32 GCGestureInterpreter::LineDepth(void)
    {
        return lineDepth_;
    }
    void GCGestureInterpreter::BGDepth(Simian::f32 val)
    {
        bgDepth_ = val;
    }
    Simian::f32 GCGestureInterpreter::BGDepth()
    {
        return bgDepth_;
    }
    void GCGestureInterpreter::PuzzleModeID(Simian::u32 id)
    {
        gestureClass_.PuzzleModeID(id);
    }
    Simian::u32 GCGestureInterpreter::PuzzleModeID(void)
    {
        return gestureClass_.PuzzleModeID();
    }
    bool GCGestureInterpreter::PuzzleSolved (void)
    {
        if (puzzleSolved_)
        {
            puzzleSolved_ = false;
            return true;
        }
        return puzzleSolved_;
    }

    //set by player logic
    void GCGestureInterpreter::GestureEnabled(bool val)
    {
        if (!val)
        {
            if (gestureClass_.GetEnabled())
            {
#ifdef G_DEBUGHELPER
                if (vbSize_ == 0)
                    Gesture::msgStr.str("");
#endif
                gestureClass_.SetEnabled(false);
                status_ = GI_EXITING;
            }
        } else {
            if (!gestureClass_.GetEnabled())
                gestureClass_.SetEnabled(true);
        }
    }

    GCGestureInterpreter* GCGestureInterpreter::Instance(void)
    {
        return pGestureInterpreter;
    }

    void GCGestureInterpreter::InitGesture(bool reload)
    {
        //get list of enemies and data
        allEnemyVec_ = &GCAIBase::AllEnemiesVec();

        camera_ = GCPlayerLogic::Instance()->Camera();
        camera_->ComponentByType(Simian::C_TRANSFORM, cameraTransform_);

        //init gesture class
        gestureClass_.InitGesture(reload);
        gestureClass_.SetEnabled(false);
    }
    void GCGestureInterpreter::AddToRenderQ(Simian::DelegateParameter&)
    {
        Simian::Game::Instance().RenderQueue().AddRenderObject(
            Simian::RenderObject(*materialBG_, Simian::RenderQueue::RL_UI,
            Simian::Matrix::Translation(0, 0, bgDepth_),
            Simian::Delegate::CreateDelegate<GCGestureInterpreter,
            &GCGestureInterpreter::DrawVertexBufferUI>(this), 0));
        Simian::Game::Instance().RenderQueue().AddRenderObject(
            Simian::RenderObject(*material_, Simian::RenderQueue::RL_UI,
            Simian::Matrix::Translation(0, 0, lineDepth_),
            Simian::Delegate::CreateDelegate<GCGestureInterpreter,
            &GCGestureInterpreter::DrawVertexBuffer>(this), 0));
        Simian::Game::Instance().RenderQueue().AddRenderObject(
            Simian::RenderObject(*materialSlash_, Simian::RenderQueue::RL_UI,
            Simian::Matrix::Translation(0, 0, lineDepth_),
            Simian::Delegate::CreateDelegate<GCGestureInterpreter,
            &GCGestureInterpreter::DrawVertexBufferSlash>(this), 0));
    }
    // Draw callback
    void GCGestureInterpreter::DrawVertexBuffer(Simian::DelegateParameter&)
    {
        if (GCUI::GameStatus() != GCUI::GAME_PAUSED)
        {
            if (!gestureClass_.GetEnabled())
            {
                Simian::Game::Instance().TimeMultiplier(1.0f);
                if (status_ == GI_EXITING)
                    status_ = GI_READY;
            }else{
                if (PuzzleModeID() == GCGesturePuzzle::GPUZZLE_NONE)
                    Simian::Game::Instance().TimeMultiplier(SLOW_MULTIPLIER);
                //--The problem of enter cinematic slow forever
                if ((GCUI::GameStatus() != GCUI::GAME_PAUSED &&
                    GCPlayerLogic::Instance()->PlayerControlOverride()) ||
                    GCUI::GameStatus() == GCUI::GAME_PAUSED)
                {
                    gestureClass_.SetEnabled(false);
                }
            }
        }

        buffer_->Data(&vertices_[0], vbSize_*
            Simian::PositionColorUVVertex::Format.Size());
        Simian::Game::Instance().GraphicsDevice().DrawIndexed(buffer_, ibuffer_,
            Simian::GraphicsDevice::PT_TRIANGLES, vbSize_/2);

        //----------------------------------------------------------------------
        std::list<std::vector<Simian::PositionColorUVVertex> >::iterator
            ite = outgoing_.begin();
        std::list<UnwindSet>::iterator iteSize = outgoingSize_.begin();
        std::list<UnwindSet>::iterator iteSizeF = outgoingFullSize_.begin();

        while (ite != outgoing_.end())
        {
            buffer_->Data(&((*ite)[0]), *iteSize*
                Simian::PositionColorUVVertex::Format.Size());
            Simian::Game::Instance().GraphicsDevice().DrawIndexed(buffer_, ibuffer_,
                Simian::GraphicsDevice::PT_TRIANGLES, *iteSize/2);
            ++ite;
            ++iteSize;
            ++iteSizeF;
        }
    }

    void GCGestureInterpreter::DrawVertexBufferSlash(Simian::DelegateParameter&)
    {
        //---------------------------SLASH SHATTER------------------------------
        std::list<std::vector<Simian::PositionColorUVVertex> >::iterator 
            ite = slashShatter_.begin();
        std::list<Simian::u32>::iterator iteSize = slashShatterSize_.begin();
        
        while (ite != slashShatter_.end())
        {
            buffer_->Data(&((*ite)[0]), *iteSize*
                Simian::PositionColorUVVertex::Format.Size());
            Simian::Game::Instance().GraphicsDevice().DrawIndexed(buffer_, ibuffer_,
                Simian::GraphicsDevice::PT_TRIANGLES, *iteSize/2);
            ++ite;
            ++iteSize;
        }
    }

    void GCGestureInterpreter::DrawVertexBufferUI(Simian::DelegateParameter&)
    {
        static Simian::f32 opacity=0.0f;//, timer=0.0f, target=0.0f, start=0.0f;
        const Simian::f32 multiplier = 1.0f, maxOpacity = 0.6f;

        if (!gestureClass_.GetEnabled() && GCUI::GameStatus() != GCUI::GAME_PAUSED)
        {
            if (opacity > 0.0f)
                opacity -= Simian::Game::Instance().FrameTime()*multiplier;
            if (opacity <= 0.0f)
            {
                opacity = 0.0f;
                return;
            }
        }
        else
        {
            if (opacity < maxOpacity)
                opacity += Simian::Game::Instance().FrameTime()*multiplier;
            if (opacity > maxOpacity)
                opacity = maxOpacity;
        }

        //--for resizing
        uiVertices_[0].X = -Simian::Game::Instance().Size().X()*0.5f;
        uiVertices_[0].Y = -Simian::Game::Instance().Size().Y()*0.5f;
        uiVertices_[1].X = Simian::Game::Instance().Size().X()*0.5f;
        uiVertices_[1].Y = -Simian::Game::Instance().Size().Y()*0.5f;
        uiVertices_[2].X = Simian::Game::Instance().Size().X()*0.5f;
        uiVertices_[2].Y = Simian::Game::Instance().Size().Y()*0.5f;
        uiVertices_[3].X = -Simian::Game::Instance().Size().X()*0.5f;
        uiVertices_[3].Y = Simian::Game::Instance().Size().Y()*0.5f;

        for (Simian::u32 i=0; i<UIvbSize_; ++i)
        {
            uiVertices_[i] = Simian::PositionColorUVVertex(
                Simian::Vector3(uiVertices_[i].X, uiVertices_[i].Y),
                Simian::Color(0.0f, 0.0f, 0.0f, opacity),
                Simian::Vector2(uiVertices_[i].U, uiVertices_[i].V));
        }
        uibuffer_->Data(&uiVertices_[0], UIvbSize_*
            Simian::PositionColorUVVertex::Format.Size());
        Simian::Game::Instance().GraphicsDevice().DrawIndexed(uibuffer_, ibuffer_,
            Simian::GraphicsDevice::PT_TRIANGLES, UIvbSize_/2);
    }

    void GCGestureInterpreter::OnSharedLoad(void)
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::
            CreateDelegate<GCGestureInterpreter,
            &GCGestureInterpreter::update_>(this));
        RegisterCallback(Simian::P_RENDER, Simian::Delegate::
            CreateDelegate<GCGestureInterpreter,
            &GCGestureInterpreter::AddToRenderQ>(this));

        material_ = Simian::Game::Instance().MaterialCache().
            Load("Materials/gesture.mat");
        materialSlash_ = Simian::Game::Instance().MaterialCache().
            Load("Materials/gestureSlash.mat");
        materialBG_ = Simian::Game::Instance().MaterialCache().
            Load("Materials/gesture_bg.mat");

        //----------------------------------------------------------------------
        //vertex buffers
        //----------------------------------------------------------------------
        const Simian::Color col;

        Simian::Game::Instance().GraphicsDevice().CreateVertexBuffer(buffer_);
        Simian::Game::Instance().GraphicsDevice().CreateVertexBuffer(uibuffer_);
        Simian::Game::Instance().GraphicsDevice().CreateIndexBuffer(ibuffer_);

        vertices_.resize(MAX_VBSIZE, Simian::PositionColorUVVertex(
            Simian::Vector3(), col, Simian::Vector2()));
        uiVertices_.resize(MAX_VBSIZE, Simian::PositionColorUVVertex(
            Simian::Vector3(), col, Simian::Vector2()));
        std::vector<Simian::u16> indices;
        for (Simian::u32 i=0; i<(MAX_VBSIZE*3)/2; ++i)
        {
            //012230 BL BR TR TR TL BL
            indices.push_back(static_cast<Simian::u16>(i*4));
            indices.push_back(static_cast<Simian::u16>(i*4+1));
            indices.push_back(static_cast<Simian::u16>(i*4+2));
            indices.push_back(static_cast<Simian::u16>(i*4+2));
            indices.push_back(static_cast<Simian::u16>(i*4+3));
            indices.push_back(static_cast<Simian::u16>(i*4));
        }

        buffer_->Load(Simian::PositionColorUVVertex::Format, vertices_.size(), true);
        buffer_->Data(&vertices_[0]);
        uibuffer_->Load(Simian::PositionColorUVVertex::Format, uiVertices_.size(), true);
        uibuffer_->Data(&uiVertices_[0]);
        ibuffer_->Load(Simian::IndexBufferBase::IBT_U16, indices.size());
        ibuffer_->Data(&indices[0]);

        uiVertices_[UIvbSize_++] = Simian::PositionColorUVVertex(
            Simian::Vector3(Simian::Game::Instance().Size().X()/-2.0f,
            Simian::Game::Instance().Size().Y()/-2.0f), col,
            Simian::Vector2(0, 1));
        uiVertices_[UIvbSize_++] = Simian::PositionColorUVVertex(
            Simian::Vector3(Simian::Game::Instance().Size().X()/2.0f,
            Simian::Game::Instance().Size().Y()/-2.0f), col,
            Simian::Vector2(1, 1));
        uiVertices_[UIvbSize_++] = Simian::PositionColorUVVertex(
            Simian::Vector3(Simian::Game::Instance().Size().X()/2.0f,
            Simian::Game::Instance().Size().Y()/2.0f), col,
            Simian::Vector2(1, 0));
        uiVertices_[UIvbSize_++] = Simian::PositionColorUVVertex(
            Simian::Vector3(Simian::Game::Instance().Size().X()/-2.0f,
            Simian::Game::Instance().Size().Y()/2.0f), col,
            Simian::Vector2(0,0));
    }

    void GCGestureInterpreter::OnSharedUnload(void)
    {
        buffer_->Unload();
        uibuffer_->Unload();
        ibuffer_->Unload();

        Simian::Game::Instance().GraphicsDevice().DestroyVertexBuffer(buffer_);
        Simian::Game::Instance().GraphicsDevice().DestroyVertexBuffer(uibuffer_);
        Simian::Game::Instance().GraphicsDevice().DestroyIndexBuffer(ibuffer_);
    }

    void GCGestureInterpreter::OnAwake(void)
    {
        //nothing: use init
        pGestureInterpreter = this;
        lineWidth_ = LINE_WIDTH;
        status_ = GI_READY;
        puzzleSolved_ = false;

        newStart = true;
    }

    void GCGestureInterpreter::OnInit(void)
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        InitGesture(true);
        GestureTutor::LoadPath();
        sounds_ = GCPlayerLogic::Instance()->Sounds();
        mpCost_ = messageIntp_ = 0;
        msgStatus_ = GI_MSGID_NONE;
        msgFail_ = ParentScene()->CreateEntity(E_GMESSAGE_FAIL);
        msgNoMP_ = ParentScene()->CreateEntity(E_GMESSAGE_NOMP);
        msgStraight_ = ParentScene()->CreateEntity(E_GMESSAGE_STRAIGHT);
        ParentScene()->AddEntity(msgFail_);
        ParentScene()->AddEntity(msgNoMP_);
        ParentScene()->AddEntity(msgStraight_);

        if (SaveManager::Instance().CurrentData().SaveID != SaveManager::SAVE_INVALID)
        {
            SetSkills(static_cast<Simian::u32>(SaveManager::Instance().
                CurrentData().SkillsUnlocked));
        }
    }

    void GCGestureInterpreter::OnDeinit(void)
    {
        //clear all memory
        vertices_.clear();
        resultVec_.clear();
        gestureClass_.SetEnabled(false);
        gestureClass_.PuzzleModeID(GCGesturePuzzle::GPUZZLE_NONE);
        gestureClass_.UploadData();
    }

    void GCGestureInterpreter::Serialize(Simian::FileObjectSubNode& data)
    {
        //nothing for now
        data.AddData("BGDepth", bgDepth_);
        data.AddData("LineDepth", lineDepth_);
        data.AddData("GestureSkills", gestureSkills_.Skills());
        data.AddData("Enabled", gestureSkills_.CheckSkill(Gesture::GCMD_NONE));
    }

    void GCGestureInterpreter::Deserialize(const Simian::FileObjectSubNode& data)
    {
        const Simian::FileDataSubNode* BGDepthNode = data.Data("BGDepth");
        bgDepth_ = BGDepthNode ? BGDepthNode->AsF32() : bgDepth_;

        const Simian::FileDataSubNode* LineDepthNode = data.Data("LineDepth");
        lineDepth_ = LineDepthNode ? LineDepthNode->AsF32() : lineDepth_;

        const Simian::FileDataSubNode* GestureSkillsNode = data.Data("GestureSkills");
        gestureSkills_ = GestureSkillsNode ? GestureSkills(GestureSkillsNode->AsU32())
            : gestureSkills_;

        const Simian::FileDataSubNode* EnabledNode = data.Data("Enabled");
        if (EnabledNode && EnabledNode->AsBool())
            gestureSkills_.Skills(gestureSkills_.Skills() | 1);
    }

    void GCGestureInterpreter::update_(Simian::DelegateParameter& param)
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        //--STRHAX
        /*
        if (Simian::Keyboard::IsPressed(Simian::Keyboard::KEY_CONTROL))
        {
            if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_G))
            {
                GCUI::Instance()->UnloadSkills();
                GCGestureInterpreter::Instance()->SetSkills(0);
                GCUI::Instance()->LoadSkills();
            }
            if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_F))
            {
                GCUI::Instance()->UnloadSkills();
                GCGestureInterpreter::Instance()->SetSkills(15);
                GCUI::Instance()->LoadSkills();
            }
            if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_D))
            {
                GCPlayerLogic::Instance()->Attributes()->ExperienceGoal
                    (GCPlayerLogic::Instance()->Attributes()->MinExperienceGoal());
                GCPlayerLogic::Instance()->Attributes()->MaxMana
                    (GCPlayerLogic::Instance()->Attributes()->MinMaxMana());
                GCPlayerLogic::Instance()->Attributes()->MaxHealth
                    (GCPlayerLogic::Instance()->Attributes()->MinMaxHealth());
                GCPlayerLogic::Instance()->Attributes()->Mana
                    (GCPlayerLogic::Instance()->Attributes()->MaxMana());
                GCPlayerLogic::Instance()->Attributes()->Health
                    (GCPlayerLogic::Instance()->Attributes()->MaxHealth());
                GCPlayerLogic::Instance()->Attributes()->Experience
                    (GCPlayerLogic::Instance()->Attributes()->ExperienceGoal());
            }

            if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_P))
            {
                SaveManager::Instance().CurrentData().Position = Simian::Vector2(
                    GCPlayerLogic::Instance()->Physics()->Position().X(),
                    GCPlayerLogic::Instance()->Physics()->Position().Z());
                SaveManager::Instance().UpdateCurrentData();
                SaveManager::Instance().SaveData(SaveManager::SAVE_CHECKPOINT);
            }

            if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_O))
                SaveManager::Instance().LoadCheckpoint();
        }
        */
        //--ENDHAX
        Simian::f32 winHeight = Simian::Game::Instance().Size().Y(),
            scaleHeight = winHeight/1280.0f;
        Simian::CTransform* fadeSprite = NULL;
        msgFail_->ComponentByType(Simian::C_TRANSFORM, fadeSprite);
        //fadeSprite->Position(Simian::Vector3(0, winHeight*0.32f));
        fadeSprite->Scale(Simian::Vector3(scaleHeight, scaleHeight, scaleHeight));
        msgNoMP_->ComponentByType(Simian::C_TRANSFORM, fadeSprite);
        //fadeSprite->Position(Simian::Vector3(0, winHeight*0.32f));
        fadeSprite->Scale(Simian::Vector3(scaleHeight, scaleHeight, scaleHeight));
        msgStraight_->ComponentByType(Simian::C_TRANSFORM, fadeSprite);
        //fadeSprite->Position(Simian::Vector3(0, winHeight*0.32f));
        fadeSprite->Scale(Simian::Vector3(scaleHeight, scaleHeight, scaleHeight));
        UpdateMessage_();

        if (newStart)
        {
            SwapMessage_(GI_MSGID_NONE);
            newStart = false;
        }

        dt_ = param.As<Simian::EntityUpdateParameter>().Dt;
        static Simian::f32 timer = 2.0f;
        timer += dt_;
        timer = Simian::Math::Clamp<Simian::f32>(timer, 0, 2.0f);
        if (timer < 2.0f)
        {
            animateUV_();
            slashEffect_(dt_);
        }

        if (GCUI::GameStatus() == GCUI::GAME_PAUSED ||
            (GCPlayerLogic::Instance()->PlayerControlOverride()
            && PuzzleModeID() == GCGesturePuzzle::GPUZZLE_NONE) ||
            !gestureSkills_.CheckSkill(Gesture::GCMD_NONE) ||
            gestureSkills_.Skills() == 1 ||
            GCUpgradeExchange::Exchanging())
        {
            if ((GCUI::GameStatus() != GCUI::GAME_PAUSED &&
                 GCPlayerLogic::Instance()->PlayerControlOverride()))
                 Simian::Game::Instance().TimeMultiplier(1.0f);

            //--On pause, draw if there's an intent to draw
            if (GestureTutor::DrawType() != GestureTutor::GT_TOTAL)
            {
                GestureTutor::DrawGesture(dt_);
                animateUV_(); //aura energy
                return;
            }

            timer = 0.0f;
            ExitCasting_();
            resultVec_.clear();
            return;
        }

        Update(dt_);
    }

    void GCGestureInterpreter::animateUV_(void)
    {
        //ANIMATION: crazy UV movement
        Simian::u32 loopCount = vbSize_/VB_DIV +1;
        static Simian::f32 vArr[5] = {0.5f};
        const Simian::u32 arrSize = sizeof(vArr)/sizeof(vArr[0]);
        const Simian::f32 outSpeed = 0.20f;
        //Timer
        Simian::f32 timeStep = Simian::Game::Instance().FrameTime()*0.25f;
        vArr[0] += timeStep;
        vArr[0] = vArr[0] > 1.0f ? 0.5f : vArr[0];
        for (Simian::u32 i=1; i<arrSize; ++i)
            vArr[i] = vArr[i-1] - 0.125f;

        //--Unwind variable
        static Simian::f32 timer = 0.0f;
        static const Simian::u32 quadsDec = 4;
        static const Simian::f32 unwindInterval = 0.016f;
        timer += Simian::Game::Instance().FrameTime();

        if (status_ != GI_FADING)
        {
            for (Simian::u32 i=0; i<vbSize_/2; ++i)
            {
                if (i%2)
                {
                    vertices_[i*2+1].U = vertices_[i*2+1].U<=0.0f ? 0.0f :
                        vertices_[i*2+1].U - timeStep;
                    vertices_[i*2].U = vertices_[i*2].U>=1.0f ? 1.0f :
                        vertices_[i*2].U + timeStep;
                }else{
                    if (i!=0)
                    {
                        vertices_[i*2].U = vertices_[i*2-1].U;
                        vertices_[i*2+1].U = vertices_[i*2-2].U;
                    }else{
                        vertices_[i*2].U = vertices_[i*2].U<=0.0f ? 0.0f :
                            vertices_[i*2].U - timeStep;
                    vertices_[i*2+1].U = vertices_[i*2+1].U>=1.0f ? 1.0f :
                        vertices_[i*2+1].U + timeStep;
                    }
                }
            }
        } else {
            for (Simian::u32 i=0; i<vbSize_/2; ++i)
            {
                if (i%2)
                {
                    vertices_[i*2+1].U = vertices_[i*2+1].U - timeStep;
                    vertices_[i*2].U =vertices_[i*2].U + timeStep;
                }else
                {
                    vertices_[i*2].U = vertices_[i*2].U - timeStep;
                    vertices_[i*2+1].U = vertices_[i*2+1].U + timeStep;
                }
            }
        }

        for (Simian::u32 i=0; i<loopCount; ++i)
        {
            vertices_[i*VB_DIV+1].V = vertices_[i*VB_DIV].V = vArr[0];
            vertices_[i*VB_DIV+3].V = vertices_[i*VB_DIV+2].V = vArr[1];
            vertices_[i*VB_DIV+5].V = vertices_[i*VB_DIV+4].V = vArr[1];
            vertices_[i*VB_DIV+7].V = vertices_[i*VB_DIV+6].V = vArr[2];
            vertices_[i*VB_DIV+9].V = vertices_[i*VB_DIV+8].V = vArr[2];
            vertices_[i*VB_DIV+10].V = vertices_[i*VB_DIV+11].V = vArr[3];
            vertices_[i*VB_DIV+12].V = vertices_[i*VB_DIV+13].V = vArr[3];
            vertices_[i*VB_DIV+14].V = vertices_[i*VB_DIV+15].V = vArr[4];
        }

        //----------------------------------------------------------------------
        const Simian::f32 minUV = -5.0f;
        std::list<std::vector<Simian::PositionColorUVVertex> >::iterator
            ite = outgoing_.begin();
        std::list<UnwindSet>::iterator iteSize = outgoingSize_.begin();
        std::list<UnwindSet>::iterator iteSizeF = outgoingFullSize_.begin();
        while (ite != outgoing_.end())
        {
            std::vector<Simian::PositionColorUVVertex>& vertices = *ite;
            Simian::u32 max = *iteSize/VB_DIV;
            Simian::f32 incStep = timeStep + ((0.5f-U_OFFSET)-vertices[0].U)
                * outSpeed;

            //------------------------------------------------------------------
            if ( iteSize->Size == 0 ||
                (ite == outgoing_.begin() && vertices[0].U < minUV) )
            {
                ++ite;
                ++iteSize;
                ++iteSizeF;
                outgoing_.pop_front();
                outgoingSize_.pop_front();
                outgoingFullSize_.pop_front();
                continue;
            }

            Simian::f32 uw = iteSize->Unwind ? 0.3f : 1.0f ;
            //------------------------------------------------------------------
            for (Simian::u32 i=0; i<*iteSize/2; ++i)
            {
                if (i%2)
                {
                    vertices[i*2+1].U = vertices[i*2+1].U - incStep*uw;
                    vertices[i*2].U =vertices[i*2].U + incStep*uw;
                } else {
                    vertices[i*2].U = vertices[i*2].U - incStep*uw;
                    vertices[i*2+1].U = vertices[i*2+1].U + incStep*uw;
                }
            }
            for (Simian::u32 i=0; i<max; ++i)
            {
                vertices[i*VB_DIV+1].V = vertices[i*VB_DIV].V = vArr[0];
                vertices[i*VB_DIV+3].V = vertices[i*VB_DIV+2].V = vArr[1];
                vertices[i*VB_DIV+5].V = vertices[i*VB_DIV+4].V = vArr[1];
                vertices[i*VB_DIV+7].V = vertices[i*VB_DIV+6].V = vArr[2];
                vertices[i*VB_DIV+9].V = vertices[i*VB_DIV+8].V = vArr[2];
                vertices[i*VB_DIV+10].V = vertices[i*VB_DIV+11].V = vArr[3];
                vertices[i*VB_DIV+12].V = vertices[i*VB_DIV+13].V = vArr[3];
                vertices[i*VB_DIV+14].V = vertices[i*VB_DIV+15].V = vArr[4];
            }

            //------------------------------------------------------------------
            if (iteSize->Unwind && timer >= unwindInterval)
            {
                Simian::u32 decrement = iteSizeF->Size / 64 * quadsDec;
                Simian::u32 count = static_cast<Simian::u32>(timer/unwindInterval);
                for (Simian::u32 i=0; i<count; ++i)
                    iteSize->Size -= iteSize->Size == 0 ? 0 : 
                    Simian::Math::Min(decrement, iteSize->Size);
            }

            ++ite;
            ++iteSize;
            ++iteSizeF;
        }

        if (timer >= unwindInterval)
            timer = 0.0f;
    }

    void GCGestureInterpreter::slashEffect_(Simian::f32 dt)
    {
        const Simian::f32 speedMult = 1.75f, fadeSpeed = 1.25f, frameRate = 50.0f;
        std::list<std::vector<Simian::PositionColorUVVertex> >::iterator
            ite = slashShatter_.begin();
        std::list<std::vector<Simian::f32> >::iterator iteRand = slashShatterRand_.begin();
        std::list<Simian::u32>::iterator iteSize = slashShatterSize_.begin();
        while (ite != slashShatter_.end())
        {
            std::vector<Simian::PositionColorUVVertex>& vertices = *ite;
            //------------------------------------------------------------------
            GCUI::Hexa32 col(vertices[0].Diffuse); //[3]
            Simian::f32 opac = col.Byte[3]/255.f;
            opac -= dt * fadeSpeed;

            if (ite == slashShatter_.begin() && (*slashShatterSize_.begin() == 0
                || opac <= 0.0f))
            {
                ++ite;
                ++iteSize;
                ++iteRand;
                slashShatter_.pop_front();
                slashShatterSize_.pop_front();
                slashShatterRand_.pop_front();
                continue;
            }
            Simian::u32 loopCount = (*iteSize)/4;
            //col.Byte[3] = static_cast<Simian::u8>(opac*255);
            //col.Byte[2] = static_cast<Simian::u8>(col.Byte[2]*opac);
            //col.Byte[1] = static_cast<Simian::u8>(col.Byte[1]*opac);
            //col.Byte[0] = static_cast<Simian::u8>(col.Byte[0]*opac);

            for (Simian::u32 k=0; k<4; ++k)
                col.Byte[k] = static_cast<Simian::u8>(255*opac);

            Simian::f32 dtSpeed = dt * frameRate;

            for (Simian::u32 i=0; i<loopCount; ++i) {
                for (Simian::u32 j=i*4; j<i*4+4; ++j) {
                    vertices[j].Diffuse = col.Full;
                    vertices[j].X += (*iteRand)[i*8+(j-i*4)*2] * dtSpeed;
                    vertices[j].Y += (*iteRand)[i*8+1+(j-i*4)*2] * dtSpeed;
                    (*iteRand)[i*8+(j-i*4)*2] *= (1-dt*speedMult);
                    (*iteRand)[i*8+1+(j-i*4)*2] *= (1-dt*speedMult);
                }
            }
            //------------------------------------------------------------------
            ++ite;
            ++iteSize;
            ++iteRand;
        }
    }

    void GCGestureInterpreter::AddQuad(const Simian::Vector2& vec)
    {
        //NOTE: 012230 BL BR TR TR TL BL
        samplePoints_.push_back(vec);
        newStrip_ == 0 ? newStrip_ = samplePoints_.size() : newStrip_;

        const Simian::Color col;//(0.7f, 0.7f, 0.7f, 0.7f);
        Simian::Color colFade(col); //white & fullA

        //add in first point
        vertices_[vbSize_++] = Simian::PositionColorUVVertex(
            Simian::Vector3(vec.X()-lineWidth_, vec.Y()),
            colFade, Simian::Vector2(0.5f-U_OFFSET, 1));
        vertices_[vbSize_++] = Simian::PositionColorUVVertex(
            Simian::Vector3(vec.X()+lineWidth_, vec.Y()),
            colFade, Simian::Vector2(0.5f+U_OFFSET, 1));
        vertices_[vbSize_++] = Simian::PositionColorUVVertex(
            Simian::Vector3(vec.X()+lineWidth_, vec.Y()), col,
            Simian::Vector2(0.5f+U_OFFSET, 0));
        vertices_[vbSize_++] = Simian::PositionColorUVVertex(
            Simian::Vector3(vec.X()-lineWidth_, vec.Y()), col,
            Simian::Vector2(0.5f-U_OFFSET, 0));
        currAngle_ = prevAngle_ = 1.0f;
    }

    void GCGestureInterpreter::SetLastQuad(const Simian::Vector2& vec)
    {
        const Simian::Color col;//(0.5f, 0.5f, 0.5f, 0.6f); //white & fullA
        const Simian::u32 jointVB = JOINT_COUNT*4;
        const Simian::f32 minJointDist = JOINT_COUNT*JOINT_COUNT;
        //newest vector
        Simian::Vector2 newVec(vec-samplePoints_[samplePoints_.size()-1]);
        Simian::Vector2 newNormal(-newVec.Y(), newVec.X());

        if (newVec.LengthSquared() < jointVB)
            return;

        newNormal.Normalize();
        Simian::Vector2 thickness(newNormal*lineWidth_);

        if (newStrip_ != samplePoints_.size()) //not newline
        {
            //at least 2 previous points in current strip
            Simian::Vector2 oldVec(samplePoints_[samplePoints_.size()-1]-
                samplePoints_[samplePoints_.size()-2]);
            Simian::f32 angle = newVec.Dot(oldVec)/(newVec.Length()*oldVec.Length());
            Simian::Vector2 oldNormal(-oldVec.Y(), oldVec.X());
            currAngle_ = angle; //update for later check
            if (newVec.LengthSquared() >= minJointDist
                && jointCheck_(vec, angle))
            {   //control joints, current and last normal end
                oldNormal = oldNormal.Normalized() * lineWidth_;

                //last segment's end edge
                vertices_[vbSize_-jointVB-5].X = 
                    samplePoints_[samplePoints_.size()-1].X() + oldNormal.X();
                vertices_[vbSize_-jointVB-5].Y = 
                    samplePoints_[samplePoints_.size()-1].Y() + oldNormal.Y();
                vertices_[vbSize_-jointVB-6].X = 
                    samplePoints_[samplePoints_.size()-1].X() - oldNormal.X();
                vertices_[vbSize_-jointVB-6].Y = 
                    samplePoints_[samplePoints_.size()-1].Y() - oldNormal.Y();

                bool normSide = newVec.Dot(oldNormal) > 0.0f;
                Simian::f32 vAngle = std::acos(angle)/JOINT_COUNT; //RADIAN
                Simian::Vector2 pivot(samplePoints_[samplePoints_.size()-1]),
                    temp(oldNormal);//temp(oldNormal*2); //rotation setup

                if (angle > JOINT_ANGLE2) //ROUND CORNER--------------------------------
                {
                    if (normSide) //find which side the new vector is
                    {   //COUNTER-CLOCKWISE ROTATION
                        temp *= 2;
                        Simian::f32 sineA = std::sin(vAngle);
                        Simian::f32 cosA = std::cos(vAngle);
                        Gesture::Mtx2x2 rotMtx = {cosA, -sineA, sineA, cosA};
                        pivot = Simian::Vector2(vertices_[vbSize_-jointVB-5].X,
                            vertices_[vbSize_-jointVB-5].Y);
                        for (Simian::u32 i=JOINT_COUNT; i>0; --i)
                        {
                            vertices_[vbSize_-i*4-4].X = vertices_[vbSize_-i*4-1].X
                                = pivot.X();
                            vertices_[vbSize_-i*4-4].Y = vertices_[vbSize_-i*4-1].Y
                                = pivot.Y();
                            vertices_[vbSize_-i*4-3].X = pivot.X()-temp.X();
                            vertices_[vbSize_-i*4-3].Y = pivot.Y()-temp.Y();
                            Gesture::RotateVec2(temp, rotMtx);
                            vertices_[vbSize_-i*4-2].X = pivot.X()-temp.X();
                            vertices_[vbSize_-i*4-2].Y = pivot.Y()-temp.Y();
                        }
                    }else{ //CLOCKWISE ROTATION
                        temp *= 2;
                        vAngle = -vAngle;
                        Simian::f32 sineA = std::sin(vAngle);
                        Simian::f32 cosA = std::cos(vAngle);
                        Gesture::Mtx2x2 rotMtx = {cosA, -sineA, sineA, cosA};
                        pivot = Simian::Vector2(vertices_[vbSize_-jointVB-6].X,
                            vertices_[vbSize_-jointVB-6].Y);
                        for (Simian::u32 i=JOINT_COUNT; i>0; --i)
                        {
                            vertices_[vbSize_-i*4-3].X = vertices_[vbSize_-i*4-2].X
                                = pivot.X();
                            vertices_[vbSize_-i*4-3].Y = vertices_[vbSize_-i*4-2].Y
                                = pivot.Y();
                            vertices_[vbSize_-i*4-4].X = pivot.X()+temp.X();
                            vertices_[vbSize_-i*4-4].Y = pivot.Y()+temp.Y();
                            Gesture::RotateVec2(temp, rotMtx);
                            vertices_[vbSize_-i*4-1].X = pivot.X()+temp.X();
                            vertices_[vbSize_-i*4-1].Y = pivot.Y()+temp.Y();
                        }
                    }
                } else { //--------------------FAN CORNER-----------------------
                    if (normSide) //find which side the new vector is
                    {   //COUNTER-CLOCKWISE ROTATION
                        Simian::f32 sineA = std::sin(vAngle);
                        Simian::f32 cosA = std::cos(vAngle);
                        Gesture::Mtx2x2 rotMtx = {cosA, -sineA, sineA, cosA};
                        for (Simian::u32 i=JOINT_COUNT; i>0; --i)
                        {
                            vertices_[vbSize_-i*4-4].X = pivot.X()+temp.X();
                            vertices_[vbSize_-i*4-4].Y = pivot.Y()+temp.Y();
                            vertices_[vbSize_-i*4-3].X = pivot.X()-temp.X();
                            vertices_[vbSize_-i*4-3].Y = pivot.Y()-temp.Y();
                            Gesture::RotateVec2(temp, rotMtx);
                            vertices_[vbSize_-i*4-2].X = pivot.X()-temp.X();
                            vertices_[vbSize_-i*4-2].Y = pivot.Y()-temp.Y();
                            vertices_[vbSize_-i*4-1].X = pivot.X()+temp.X();
                            vertices_[vbSize_-i*4-1].Y = pivot.Y()+temp.Y();
                        }
                    }else{ //CLOCKWISE ROTATION
                        vAngle = -vAngle;
                        Simian::f32 sineA = std::sin(vAngle);
                        Simian::f32 cosA = std::cos(vAngle);
                        Gesture::Mtx2x2 rotMtx = {cosA, -sineA, sineA, cosA};

                        for (Simian::u32 i=JOINT_COUNT; i>0; --i)
                        {
                            vertices_[vbSize_-i*4-4].X = pivot.X()+temp.X();
                            vertices_[vbSize_-i*4-4].Y = pivot.Y()+temp.Y();
                            vertices_[vbSize_-i*4-3].X = pivot.X()-temp.X();
                            vertices_[vbSize_-i*4-3].Y = pivot.Y()-temp.Y();
                            Gesture::RotateVec2(temp, rotMtx);
                            vertices_[vbSize_-i*4-2].X = pivot.X()-temp.X();
                            vertices_[vbSize_-i*4-2].Y = pivot.Y()-temp.Y();
                            vertices_[vbSize_-i*4-1].X = pivot.X()+temp.X();
                            vertices_[vbSize_-i*4-1].Y = pivot.Y()+temp.Y();
                        }
                    }
                }
                //current segment fixed to join 2nd last quad of joint
                vertices_[vbSize_-4].X = vertices_[vbSize_-5].X;
                vertices_[vbSize_-4].Y = vertices_[vbSize_-5].Y;
                vertices_[vbSize_-3].X = vertices_[vbSize_-6].X;
                vertices_[vbSize_-3].Y = vertices_[vbSize_-6].Y;
            }else{
                //control only -6-5-4-3 / -2-1
                oldNormal += newNormal;
                oldNormal = oldNormal.Normalized() * lineWidth_;

                vertices_[vbSize_-5].X = vertices_[vbSize_-4].X
                    = samplePoints_[samplePoints_.size()-1].X() + oldNormal.X();
                vertices_[vbSize_-5].Y = vertices_[vbSize_-4].Y
                    = samplePoints_[samplePoints_.size()-1].Y() + oldNormal.Y();
                vertices_[vbSize_-6].X = vertices_[vbSize_-3].X
                    = samplePoints_[samplePoints_.size()-1].X() - oldNormal.X();
                vertices_[vbSize_-6].Y = vertices_[vbSize_-3].Y
                    = samplePoints_[samplePoints_.size()-1].Y() - oldNormal.Y();
            }

            //always follow mouse
            vertices_[vbSize_-1].X = vec.X() + thickness.X();
            vertices_[vbSize_-1].Y = vec.Y() + thickness.Y();
            vertices_[vbSize_-2].X = vec.X() - thickness.X();
            vertices_[vbSize_-2].Y = vec.Y() - thickness.Y();
        }else{
            //new line strip, strict follow angle
            vertices_[vbSize_-4].X = samplePoints_[samplePoints_.size()-1].X() + 
                thickness.X();
            vertices_[vbSize_-4].Y = samplePoints_[samplePoints_.size()-1].Y() + 
                thickness.Y();
            vertices_[vbSize_-3].X = samplePoints_[samplePoints_.size()-1].X() - 
                thickness.X();
            vertices_[vbSize_-3].Y = samplePoints_[samplePoints_.size()-1].Y() - 
                thickness.Y();
            vertices_[vbSize_-1].X = vec.X() + thickness.X();
            vertices_[vbSize_-1].Y = vec.Y() + thickness.Y();
            vertices_[vbSize_-2].X = vec.X() - thickness.X();
            vertices_[vbSize_-2].Y = vec.Y() - thickness.Y();

            prevAngle_ = 1.0f; //reset angle to straight line
        }
    }

    bool GCGestureInterpreter::jointCheck_(const Simian::Vector2& mouseVec,
        Simian::f32 angle)
    {
        const Simian::Color col;//(0.5f, 0.5f, 0.5f, 0.6f);
        bool isAngular = angle < JOINT_ANGLE;

        //create new joint
        if (prevAngle_ > JOINT_ANGLE && angle < JOINT_ANGLE)
        {
            prevAngle_ = angle;

            //add quads
            for (Simian::u32 i=0; i<JOINT_COUNT; ++i)
            {
                vertices_[vbSize_++] = Simian::PositionColorUVVertex(
                    Simian::Vector3(mouseVec.X()-lineWidth_, mouseVec.Y()),
                    col, Simian::Vector2(0.5f-U_OFFSET, 1));
                vertices_[vbSize_++] = Simian::PositionColorUVVertex(
                    Simian::Vector3(mouseVec.X()+lineWidth_, mouseVec.Y()),
                    col, Simian::Vector2(0.5f+U_OFFSET, 1));
                vertices_[vbSize_++] = Simian::PositionColorUVVertex(
                    Simian::Vector3(mouseVec.X()+lineWidth_, mouseVec.Y()),
                    col, Simian::Vector2(0.5f+U_OFFSET, 0));
                vertices_[vbSize_++] = Simian::PositionColorUVVertex(
                    Simian::Vector3(mouseVec.X()-lineWidth_, mouseVec.Y()),
                    col, Simian::Vector2(0.5f-U_OFFSET, 0));
            }
        }else if (prevAngle_ < JOINT_ANGLE && angle > JOINT_ANGLE)
        {
            prevAngle_ = angle;
            vbSize_ -= JOINT_COUNT * 4;
        }

        return isAngular;
    }

    void GCGestureInterpreter::ExitCasting_(void)
    {
        newStrip_ = vbSize_ = lastVB_ = 0; //reset counters
        samplePoints_.clear();
        status_ = GI_READY;
    }

    void GCGestureInterpreter::castBlazingTrine_( const BlazingTrine& trine )
    {
        // Play sounds
        if (sounds_)
            sounds_->Play(GCPlayerLogic::S_CAST_FIRE);

        // spawn the triangle effect
        const Simian::Vector3 (&triangle)[3] = trine.Points;
        Simian::Entity* effect = ParentScene()->CreateEntity(E_FLAMEEFFECT);
        GCFlameEffect* flame;
        effect->ComponentByType(GC_FLAMEEFFECT, flame);
        flame->Vertices(triangle);
        ParentScene()->AddEntity(effect);

        // get player attribs
        GCAttributes* pAttribs = GCPlayerLogic::Instance()->Attributes();

        //get new hit radius from average box length
        Simian::f32 hitRadius = trine.Radius;

        //hit all the guys inside radius
        for (Simian::u32 k=0; k<allEnemyVec_->size(); ++k)
        {
            Simian::Vector3 ePos((*allEnemyVec_)[k]->Physics()->Position());
            Simian::f32 dist = (ePos-trine.Center).LengthSquared();

            //hit if within range
            if ((*allEnemyVec_)[k]->Attributes()->Health() > 0
                && dist < (hitRadius+(*allEnemyVec_)[k]->Physics()->Radius())
                * (hitRadius+(*allEnemyVec_)[k]->Physics()->Radius()) &&
                (*allEnemyVec_)[k]->Physics()->Position().Y() >= LOWESTY)
            {
                GCAttributes* eAttribs = (*allEnemyVec_)[k]->Attributes();

                CombatCommandSystem::Instance().AddCommand(pAttribs, eAttribs,
                    CombatCommandDealDamage::Create(pAttribs->SpellDamage(pAttribs->ST_FIRE)));
            }
        }
    }

    void GCGestureInterpreter::updateBlazingTrines_( Simian::f32 dt )
    {
        // we remove only 1 trine per frame to avoid the complexity of searching
        // multiple times a frame to remove trines, at 30-60 fps the delay 
        // should be unnoticeable 16 - 32 ms delay for consecutive removes.
        for (unsigned i = 0; i < blazingTrines_.size(); ++i)
        {
            blazingTrines_[i].Timer += dt;
            if (blazingTrines_[i].Timer > TRINE_DELAY)
            {
                // do the trine
                castBlazingTrine_(blazingTrines_[i]);

                // remove iterator
                blazingTrines_.erase(blazingTrines_.begin() + i);
                break;
            }
        }
    }

    void GCGestureInterpreter::SwapMessage_(Simian::u32 msgID)
    {
        Simian::Vector2 winSize(Simian::Game::Instance().Size());

        msgStatus_ = msgID;
        messageIntp_ = 0.0f;
        GCFadeSprite* fadeSprite = NULL;

        if (msgID == GI_MSGID_FAIL)
        {
            msgFail_->ComponentByType(GC_FADESPRITEEFFECT, fadeSprite);
            fadeSprite->Reset();
            msgNoMP_->ComponentByType(GC_FADESPRITEEFFECT, fadeSprite);
            fadeSprite->ForceEnd();
            msgStraight_->ComponentByType(GC_FADESPRITEEFFECT, fadeSprite);
            fadeSprite->ForceEnd();
        } else if (msgID == GI_MSGID_NOMP) {
            msgFail_->ComponentByType(GC_FADESPRITEEFFECT, fadeSprite);
            fadeSprite->ForceEnd();
            msgNoMP_->ComponentByType(GC_FADESPRITEEFFECT, fadeSprite);
            fadeSprite->Reset();
            msgStraight_->ComponentByType(GC_FADESPRITEEFFECT, fadeSprite);
            fadeSprite->ForceEnd();
        } else if (msgID == GI_MSGID_STRAIGHT) {
            msgFail_->ComponentByType(GC_FADESPRITEEFFECT, fadeSprite);
            fadeSprite->ForceEnd();
            msgNoMP_->ComponentByType(GC_FADESPRITEEFFECT, fadeSprite);
            fadeSprite->ForceEnd();
            msgStraight_->ComponentByType(GC_FADESPRITEEFFECT, fadeSprite);
            fadeSprite->Reset();
        } else {
            msgFail_->ComponentByType(GC_FADESPRITEEFFECT, fadeSprite);
            fadeSprite->ForceEnd();
            msgNoMP_->ComponentByType(GC_FADESPRITEEFFECT, fadeSprite);
            fadeSprite->ForceEnd();
            msgStraight_->ComponentByType(GC_FADESPRITEEFFECT, fadeSprite);
            fadeSprite->ForceEnd();
        }

        Simian::CTransform* transform = NULL;
        msgFail_->ComponentByType(Simian::C_TRANSFORM, transform);
        transform->Position(Simian::Vector3(-winSize.Y()*1.5f, winSize.Y()*0.25f, 0));
        msgNoMP_->ComponentByType(Simian::C_TRANSFORM, transform);
        transform->Position(Simian::Vector3(-winSize.Y()*1.5f, winSize.Y()*0.25f, 0));
        msgStraight_->ComponentByType(Simian::C_TRANSFORM, transform);
        transform->Position(Simian::Vector3(-winSize.Y()*1.5f, winSize.Y()*0.25f, 0));
    }

    void GCGestureInterpreter::UpdateMessage_(void)
    {
        Simian::Vector2 winSize(Simian::Game::Instance().Size());
        Simian::CTransform* transform = NULL;

        if (msgStatus_ != GI_MSGID_NONE)
        {
            messageIntp_ += dt_*3;
            messageIntp_ = Simian::Math::Clamp<Simian::f32>(messageIntp_, 0.0f, 1.0f);

            if (msgStatus_ == GI_MSGID_FAIL)
            {
                msgFail_->ComponentByType(Simian::C_TRANSFORM, transform);
            } else if (msgStatus_ == GI_MSGID_NOMP) {
                msgNoMP_->ComponentByType(Simian::C_TRANSFORM, transform);
            } else if (msgStatus_ == GI_MSGID_STRAIGHT) {
                msgStraight_->ComponentByType(Simian::C_TRANSFORM, transform);
            }

            Simian::f32 xfinal = -winSize.X()*0.5f + winSize.Y()*0.2f,
                        xpos = Simian::Interpolation::Interpolate<Simian::f32>(
                        -winSize.Y(), xfinal, messageIntp_, Simian::Interpolation::EaseCubicOut);
            transform->Position(Simian::Vector3(xpos, winSize.Y()*0.25f, 0));

            if (messageIntp_ >= 1.0f)
            {
                if (msgStatus_ == GI_MSGID_NOMP)
                {
                    if (!Simian::Mouse::IsPressed(Simian::Mouse::KEY_RBUTTON))
                        msgStatus_ = GI_MSGID_NONE;
                } else {
                    msgStatus_ = GI_MSGID_NONE;
                }
            }
        }
    }
}
