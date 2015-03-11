/*************************************************************************/
/*!
\file		GCMovablePlatform.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/09/30
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCMOVABLEPLATFORM_H_
#define DESCENSION_GCMOVABLEPLATFORM_H_

#include "GCInputListener.h"

namespace Simian
{
    class CTransform;
    class CCamera;
    class CSoundEmitter;
}

namespace Descension
{
    class GCPhysicsController;
    class GCMousePlaneIntersection;

    class GCMovablePlatform: public GCInputListener
    {
    public:
        enum MOVABLEPLATFORM_FLAG
        {
            PLATFORM_DISABLED,
            PLATFORM_ENABLED,
            PLATFORM_ACTIVE,
            PLATFORM_MOVING
        };
    private:
        char bitFlag_;

        Simian::CCamera* camera_;
        Simian::CTransform* transform_, *cameraTransform_;
        GCPhysicsController* physics_;
        Simian::CSoundEmitter* sounds_;
        Simian::f32 speed_, rotSpeed_, radius_, vSpeed_;
        Simian::f32 waterParticlesTimer_;
        Simian::f32 waterSoundTimer_;

        Simian::Vector3 direction_;
        GCMousePlaneIntersection * spirit_;
        // statics
        static Simian::FactoryPlant<Simian::EntityComponent, GCMovablePlatform>
            factoryPlant_;
    public:
        Simian::CTransform& Transform();
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCMovablePlatform();

        void SpiritSwirlEffect(Simian::f32 dt);

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
