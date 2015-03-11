/*************************************************************************/
/*!
\file		GCDescensionCamera.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCDESCENSIONCAMERA_H_
#define DESCENSION_GCDESCENSIONCAMERA_H_

#include "Simian/EntityComponent.h"
#include "Simian/Vector3.h"
#include "Simian/Interpolation.h"

namespace Simian
{
    class CTransform;
    class CCamera;
}

namespace Descension
{
    // super power camera controller
    class GCDescensionCamera: public Simian::EntityComponent
    {
    private:
        Simian::CTransform* baseTransform_;
        Simian::CTransform* yawTransform_;
        Simian::CTransform* pitchTransform_;
        Simian::CTransform* cameraTransform_;
        Simian::CCamera* camera_;

        bool interpolating_;
        Simian::Vector3 initialPosition_;
        Simian::Vector3 finalPosition_;
        Simian::Vector3 initialYawPitchZoom_;
        Simian::Vector3 finalYawPitchZoom_;
        Simian::f32 (*interpolator_)(Simian::f32);
        Simian::f32 timer_;
        Simian::f32 duration_;

        Simian::f32 shakeDuration_;
        Simian::f32 shakeAmount_;

        Simian::Vector3 startingYawPitchZoom_;

        static GCDescensionCamera* instance_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCDescensionCamera> factoryPlant_;
    public:
        static GCDescensionCamera* Instance();

        bool Interpolating() const;
        void Interpolating(bool val);

        const Simian::Vector3& FinalPosition() const;
        void FinalPosition(const Simian::Vector3& val);

        const Simian::Vector3& FinalYawPitchZoom() const;
        void FinalYawPitchZoom(const Simian::Vector3& val);

        Simian::Vector3 YawPitchZoom() const;

        const Simian::Vector3& StartingYawPitchZoom() const;
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCDescensionCamera();

        void OnSharedLoad();
        void OnAwake();

        void Interpolate(const Simian::Vector3& destination, const Simian::Vector3& yawPitchZoom, 
            Simian::f32 time, Simian::f32(*interpolator)(Simian::f32) = Simian::Interpolation::Linear);
        void Shake(Simian::f32 amount, Simian::f32 duration);

        void Serialize( Simian::FileObjectSubNode& data );
        void Deserialize( const Simian::FileObjectSubNode& data );
    };
}

#endif
