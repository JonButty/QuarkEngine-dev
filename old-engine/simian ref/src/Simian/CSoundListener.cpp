/*****************************************************************************/
/*!
\file		CSoundListener.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/CSoundListener.h"
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/AudioManager.h"
#include "Simian/CTransform.h"

namespace Simian
{
    FactoryPlant<EntityComponent, CSoundListener> CSoundListener::factoryPlant_(
        &GameFactory::Instance().ComponentFactory(), C_SOUNDLISTENER);

    CSoundListener::CSoundListener()
        : transform_(0),
          roll_(0.0f),
          distance_(0.0f),
          doppler_(0.0f)
    {
    }

    //--------------------------------------------------------------------------
    
    Simian::f32 CSoundListener::Roll() const
    {
        return roll_;
    }

    void CSoundListener::Roll( Simian::f32 val )
    {
        roll_ = val;
        AudioManager::Instance().SetFactors(distance_, roll_, doppler_);
    }

    Simian::f32 CSoundListener::Doppler() const
    {
        return doppler_;
    }

    void CSoundListener::Doppler( Simian::f32 val )
    {
        doppler_ = val;
        AudioManager::Instance().SetFactors(distance_, roll_, doppler_);
    }

    Simian::f32 CSoundListener::Distance() const
    {
        return distance_;
    }

    void CSoundListener::Distance( Simian::f32 val )
    {
        distance_ = val;
        AudioManager::Instance().SetFactors(distance_, roll_, doppler_);
    }

    //--------------------------------------------------------------------------

    void CSoundListener::update_( DelegateParameter& )
    {
        AudioManager::Instance().Position(transform_->World().Position(),
            Simian::Vector3::Zero, transform_->World().Forward(), 
            Simian::Vector3::UnitY);
    }

    //--------------------------------------------------------------------------

    void CSoundListener::OnSharedLoad()
    {
        RegisterCallback(P_UPDATE, Delegate::CreateDelegate<CSoundListener, &CSoundListener::update_>(this));
    }

    void CSoundListener::OnAwake()
    {
        ComponentByType(C_TRANSFORM, transform_);

        AudioManager::Instance().SetFactors(distance_, roll_, doppler_);
    }

    void CSoundListener::Serialize( FileObjectSubNode& data )
    {
        data.AddData("Distance", distance_);
        data.AddData("Roll", roll_);
        data.AddData("Doppler", doppler_);
    }

    void CSoundListener::Deserialize( const FileObjectSubNode& data )
    {
        data.Data("Distance", distance_, distance_);
        data.Data("Roll", roll_, roll_);
        data.Data("Doppler", doppler_, doppler_);

        // set the factors immediately (audio is loaded wayy before anything 
        // anyway)
        AudioManager::Instance().SetFactors(distance_, roll_, doppler_);
    }
}
