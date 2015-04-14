/*************************************************************************/
/*!
\file		GCEventScript.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCEVENTSCRIPT_H_
#define DESCENSION_GCEVENTSCRIPT_H_

#include "GCInputListener.h"
#include "ScriptFile.h"

#include <vector>
#include <utility>

namespace Simian
{
    class CModel;
}

namespace Descension
{
    class GCAttributes;

    class GCEventScript: public GCInputListener
    {
    public:
        struct Event
        {
            Simian::Delegate Callback;
            Simian::u32 StackAllocated;
        };

        struct Parameter: public Simian::DelegateParameter
        {
            Simian::f32 Dt;
            GCEventScript::Event* Event;
            bool Finished;
        };
    private:
        std::string scriptLocation_;
        std::string skipCallback_;
        Simian::ExplicitType<ScriptFile<GCEventScript>*> scriptFile_;
        Simian::f32 triggerTimer_;
        Simian::f32 triggerInterval_;
        Simian::f32 waitTimer_;
        bool triggered_;
        bool repeat_;
        bool disabled_;
        bool skipped_;

        GCAttributes* attributes_;
        Simian::CModel* model_;

        std::vector<Event> eventQueue_;
        std::vector<char> stack_;
        std::vector<std::pair<std::string, std::string> > globals_;
        std::vector<Simian::Entity*> entities_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCEventScript> factoryPlant_;
    private:
        // script callbacks
        // attributes
        void findEntity_(Simian::DelegateParameter&);
        void getSelfEntity_(Simian::DelegateParameter&);
        void getHealth_(Simian::DelegateParameter&);
        void getTotalHealth_(Simian::DelegateParameter&);
        void cinematicSkipCallback_(Simian::DelegateParameter&);
        void getOSTicks_(Simian::DelegateParameter&);

        // setting attributes
        void setHealth_(Simian::DelegateParameter&);
        void setForceSeek_(Simian::DelegateParameter&);

        // self affecting functions
        void wait_(Simian::DelegateParameter&);
        void playAnimation_(Simian::DelegateParameter&);
        void trigger_(Simian::DelegateParameter&);
        void scriptSetGlobal_(Simian::DelegateParameter&);
        void setEnabled_(Simian::DelegateParameter&);
        void setSkip_(Simian::DelegateParameter&);
        void setAIPassive_(Simian::DelegateParameter&);
        void setSeekInCine_(Simian::DelegateParameter&);

        // jukebox functions
        void jukeboxPlay_(Simian::DelegateParameter&);
        void jukeboxPlayNext_(Simian::DelegateParameter&);
        void jukeboxCrossfade_(Simian::DelegateParameter&);

        // sound functions
        void soundEmitterPlay_(Simian::DelegateParameter&);
        void soundEmitterStopAll_(Simian::DelegateParameter&);

        // particle systems
        void particleSystemSetEnabled_(Simian::DelegateParameter&);

        // camera
        void cameraInterpolate_(Simian::DelegateParameter&);

        // cinematic
        void cinematicFadeOut_(Simian::DelegateParameter&);
        void cinematicShowBorder_(Simian::DelegateParameter&);
        void cinematicHideBorder_(Simian::DelegateParameter&);
        void cinematicShowSubtitle_(Simian::DelegateParameter&);
        void cinematicShakeScreen_(Simian::DelegateParameter&);

        // sprites
        void spriteSetVisible_(Simian::DelegateParameter&);

        // global affecting functions
        void entityCreate_(Simian::DelegateParameter&);
        void entityAdd_(Simian::DelegateParameter&);
        void entityRemove_(Simian::DelegateParameter&);
        void entitySetPosition_(Simian::DelegateParameter&);
        void entitySetPositionAtEntity_(Simian::DelegateParameter&);
        void entitySetPositionAtEntityYOffset_(Simian::DelegateParameter&);
        void entitySetRotation_(Simian::DelegateParameter&);
        void entityPlayAnimation_(Simian::DelegateParameter&);
        void entityModelSetVisible_(Simian::DelegateParameter&);
        void entityDealDamage_(Simian::DelegateParameter&);

        void cinematicMotionTranslate_(Simian::DelegateParameter&);
        void cinematicMotionRotate_(Simian::DelegateParameter&);

        void playerLockCamera_(Simian::DelegateParameter&);
        void playerLockMovement_(Simian::DelegateParameter&);
        void playerRegainMaxHp_(Simian::DelegateParameter&);
        void playerPlayTeleport_(Simian::DelegateParameter&);
        void playerStopTeleport_(Simian::DelegateParameter&);
        void playerSpiritSwirlPlay_(Simian::DelegateParameter&);
        void playerSpiritSwirlStop_(Simian::DelegateParameter&);
        void playerAddExperience_(Simian::DelegateParameter&);
		
        void particleTimelinePlay_(Simian::DelegateParameter&);
		void particleTimelineStop_(Simian::DelegateParameter&);

        void trapEnable_(Simian::DelegateParameter&);
        void wallStartDescend_(Simian::DelegateParameter&);
        void pathfinderContinuePathfinding_(Simian::DelegateParameter&);
        void puzzleLeverTrigger_(Simian::DelegateParameter&);
        void gestureUnlock_(Simian::DelegateParameter&);
        void enablePhysics_(Simian::DelegateParameter&);
        void disablePhysics_(Simian::DelegateParameter&);
        void canEscapeCinematic_(Simian::DelegateParameter&);
        void displayName_(Simian::DelegateParameter&);
        void drawGesture_(Simian::DelegateParameter&);

        void saveGameData_(Simian::DelegateParameter&);

        void tilemapSetWall_(Simian::DelegateParameter&);
        void levelChange_(Simian::DelegateParameter&);

        // add the delegate to the event queue.
        void addFunctionToQueue_(const Event& delegate);

        char* GetStackPtr()
        {
            // pop a region of memory off the stack and return a pointer to it
            return &stack_[0];
        }

        template <class T>
        void PopStack(T& val, char*& stack)
        {
            val = *(T*)stack;
            stack += sizeof(T);
        }

        void PopStack(std::string& val, char*& stack)
        {
            Simian::s16 length;
            PopStack(length, stack);
            val = std::string(stack, stack + length);
            stack += length;
        }

        // magic runtime generated call functions
        template <void (GCEventScript::*callback)(Simian::DelegateParameter&)>
        static SQInteger call_(HSQUIRRELVM vm)
        {
            // get callee from the roottable
            sq_pushroottable(vm);
            sq_pushstring(vm, "callee", -1);
            sq_get(vm, -2);
            GCEventScript* callee;
            sq_getuserpointer(vm, -1, (SQUserPointer*)&callee);
            sq_pop(vm, 2); // pop the user pointer and the roottable

            // perform delegate
            Event event;
            event.Callback = Simian::Delegate::CreateDelegate<GCEventScript, callback>(callee);
            event.StackAllocated = callee->stack_.size();

            // get number of arguments
            SQInteger args = sq_gettop(vm) - 1;

            // put parameters on the stack
            callee->stack_.resize(callee->stack_.size() + sizeof(Simian::s32));
            memcpy(&callee->stack_[callee->stack_.size() - sizeof(Simian::s32)], &args, sizeof(Simian::s32));
            for (Simian::s32 i = 2; i <= sq_gettop(vm); ++i)
            {
                SQObjectType type = sq_gettype(vm, i);
                if (type == OT_INTEGER)
                {
                    SQInteger integer;
                    sq_getinteger(vm, i, &integer);
                    callee->stack_.resize(callee->stack_.size() + sizeof(Simian::s32));
                    memcpy(&callee->stack_[callee->stack_.size() - sizeof(Simian::s32)], &integer, sizeof(Simian::s32));
                }
                else if (type == OT_FLOAT)
                {
                    SQFloat f;
                    sq_getfloat(vm, i, &f);
                    callee->stack_.resize(callee->stack_.size() + sizeof(Simian::f32));
                    memcpy(&callee->stack_[callee->stack_.size() - sizeof(Simian::f32)], &f, sizeof(Simian::s32));
                }
                else if (type == OT_STRING)
                {
                    const SQChar* s;
                    sq_getstring(vm, i, &s);
                    Simian::s16 length = (short)strlen(s);
                    Simian::u32 oldTop = callee->stack_.size();
                    callee->stack_.resize(callee->stack_.size() + sizeof(Simian::s16) + length);
                    memcpy(&callee->stack_[oldTop], &length, sizeof(Simian::s16));
                    memcpy(&callee->stack_[oldTop + sizeof(Simian::s16)], s, length);
                }
                else if (type == OT_BOOL)
                {
                    SQBool b;
                    sq_getbool(vm, i, &b);
                    bool in = b ? true : false;
                    callee->stack_.resize(callee->stack_.size() + sizeof(bool));
                    memcpy(&callee->stack_[callee->stack_.size() - sizeof(bool)], &in, sizeof(bool));
                }
                else if (type == OT_USERPOINTER)
                {
                    SQUserPointer p;
                    sq_getuserpointer(vm, i, &p);
                    callee->stack_.resize(callee->stack_.size() + sizeof(SQUserPointer));
                    memcpy(&callee->stack_[callee->stack_.size() - sizeof(SQUserPointer)], &p, sizeof(SQUserPointer));
                }
            }
            // find the amount of stack allocated..
            event.StackAllocated = callee->stack_.size() - event.StackAllocated;

            callee->addFunctionToQueue_(event);

            // return the return value
            return false;
        }

        // anything added with this will be magically added to the queue on invocation
        template <void(GCEventScript::*func)(Simian::DelegateParameter&)>
        void registerQueueFunction_(std::string functionName)
        {
            // add an entry for the function unto the root table
            sq_pushroottable(scriptFile_->VM());
            sq_pushstring(scriptFile_->VM(), functionName.c_str(), -1);
            sq_newclosure(scriptFile_->VM(), &call_<func>, 0);
            // pops the last 2 elements and creates the entry
            sq_newslot(scriptFile_->VM(), -3, false);
            // pop the roottable
            sq_pop(scriptFile_->VM(), 1);
        }

        // trigger update
        void update_(Simian::DelegateParameter& param);

        // run trigger function
        bool onTrigger_();
        void onExecute_();
    public:
        GCEventScript();
        ~GCEventScript();

        void OnSharedLoad();
        void OnAwake();
        void OnDeinit();

        void AddGlobal(const std::string& variable, const std::string& value);

        void OnTriggered( Simian::DelegateParameter& );
        
        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
