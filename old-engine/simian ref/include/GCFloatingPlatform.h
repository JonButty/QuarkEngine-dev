/*************************************************************************/
/*!
\file		GCFloatingPlatform.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/10/11
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCFLOATINGPLATFORM_H_
#define DESCENSION_GCFLOATINGPLATFORM_H_

#include "GCInputListener.h"

class Simian::CTransform;
class Simian::CSoundEmitter;

namespace Descension
{
    class GCPhysicsController;

    class GCFloatingPlatform: public GCInputListener
    {
    private:
        GCPhysicsController* physics_;
        Simian::CSoundEmitter* sounds_;
        Simian::f32 speed_, height_;
        Simian::f32 intp_, intStart_, intEnd_;
        Simian::CTransform* transform_;
        bool sink_, wasSinking_;
        
        Simian::Vector3 direction_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCFloatingPlatform>
            factoryPlant_;
    private:
        void update_(Simian::DelegateParameter& param);
        inline void startInterpolate(Simian::f32 target);
    public:
        GCFloatingPlatform();

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
