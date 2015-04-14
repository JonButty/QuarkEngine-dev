/*************************************************************************/
/*!
\file		GCGestureSkills.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/09/05
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCGESTURESKILLS_H_
#define DESCENSION_GCGESTURESKILLS_H_

#include "GCInputListener.h"

namespace Simian
{
    class CTransform;
}

namespace Descension
{
    class GCPhysicsController;

    class GCGestureSkills: public GCInputListener
    {
    private:
        Simian::CTransform* transform_;
        GCPhysicsController* physics_;

        Simian::f32 angle_, rotationSpeed_;
        //Simian::u32 skillID_;

        // factory plant
        static Simian::FactoryPlant<Simian::EntityComponent, GCGestureSkills> factoryPlant_;
    public:
        Simian::CTransform& Transform();
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCGestureSkills();

        void OnSharedLoad();
        void OnAwake();
        void OnInit();

        void OnTriggered(Simian::DelegateParameter& param);

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
