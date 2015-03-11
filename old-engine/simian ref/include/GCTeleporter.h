/*************************************************************************/
/*!
\file		GCTeleporter.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/10/30
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCTELEPORTER_H_
#define DESCENSION_GCTELEPORTER_H_

#include "GCInputListener.h"

#include "Simian/FiniteStateMachine.h"

namespace Simian
{
    class CTransform;
    class CSoundEmitter;
}

namespace Descension
{
    class GCPhysicsController;

    class GCTeleporter: public GCInputListener
    {
    public:
        enum TELEPORTER_STATE
        {
            TELEPORTER_DISABLED, //--unlock by condition
            TELEPORTER_ENABLED, //--checks for distance and splashy stuffs
            TELEPORTER_ACTIVE, //--teleporting in progress
            TELEPORTER_ARRIVE, //--final position
            
            TELEPORTER_STATE_TOTAL
        };
    private:
        bool activated_;
        Simian::CTransform *playerTransform_, *myTransform_;
        Simian::f32 toDuration_, fromDuration_, radius_, //--in/out duration
            timer_, animTimer_, dt_;
        GCTeleporter* otherNode_;
        Simian::CSoundEmitter* sounds_;
        std::string otherNodeName_;

        Simian::FiniteStateMachine fsm_;
        // statics
        static Simian::FactoryPlant<Simian::EntityComponent, GCTeleporter>
            factoryPlant_;

        static bool teleported_;
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCTeleporter();

        Simian::CTransform* Transform(void);

        void DisabledOnUpdate(Simian::DelegateParameter&);
        void EnabledOnUpdate(Simian::DelegateParameter&);
        void ActiveOnEnter(Simian::DelegateParameter&);
        void ActiveOnUpdate(Simian::DelegateParameter&);
        void ArriveOnEnter(Simian::DelegateParameter&);
        void ArriveOnUpdate(Simian::DelegateParameter&);

        void OnSharedLoad();
        void OnAwake();
        void OnInit();
        void OnDeinit();

        void OnTriggered(Simian::DelegateParameter&);

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
