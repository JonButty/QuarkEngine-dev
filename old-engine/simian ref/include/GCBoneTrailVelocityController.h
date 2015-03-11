/******************************************************************************/
/*!
\file		GCBoneTrailVelocityController.h
\author 	Bryan Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GCBONETRAILVELOCITYCONTROLLER_H_
#define DESCENSION_GCBONETRAILVELOCITYCONTROLLER_H_

#include "Simian/EntityComponent.h"
#include "Simian/Vector3.h"

namespace Simian
{
    class CBoneTrail;
}

namespace Descension
{
    class GCBoneTrailVelocityController: public Simian::EntityComponent
    {
    private:
        Simian::CBoneTrail* trail_;

        Simian::f32 minVelocity_;
        Simian::f32 maxVelocity_;

        bool enabled_;

        Simian::Vector3 lastPosition_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCBoneTrailVelocityController> factoryPlant_;
    public:
        void Enabled(bool enabled);
        bool Enabled() const;

        void MinVelocity(Simian::f32 minVelocity);
        Simian::f32 MinVelocity() const;
        
        void MaxVelocity(Simian::f32 maxVelocity);
        Simian::f32 MaxVelocity() const;
    private:
        void update_(Simian::DelegateParameter&);
        Simian::Vector3 currentPosition_() const;
    public:
        GCBoneTrailVelocityController();

        void OnSharedLoad();
        void OnAwake();

        void Serialize( Simian::FileObjectSubNode& );
        void Deserialize( const Simian::FileObjectSubNode& );
    };
}

#endif
