/*************************************************************************/
/*!
\file		GCAmbientSound.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCAmbientSound.h"
#include "ComponentTypes.h"

#include "Simian\EnginePhases.h"
#include "Simian\EngineComponents.h"
#include "Simian\CSoundEmitter.h"
#include "Simian\Math.h"
#include "Simian\LogManager.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCAmbientSound> GCAmbientSound::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_AMBIENTSOUND);

    GCAmbientSound::GCAmbientSound()
        : emitter_(0),
          soundId_(0),
          timer_(0.0f),
          interval_(1.0f),
          randomInterval_(0.0f)
    {
    }

    //--------------------------------------------------------------------------

    void GCAmbientSound::update_( Simian::DelegateParameter& param )
    {
        Simian::EntityUpdateParameter* p;
        param.As(p);

        timer_ -= p->Dt;

        if (timer_ <= 0.0f)
        {
            emitter_->Play(soundId_);
            timer_ = interval_ + Simian::Math::Random(0.0f, randomInterval_);
        }
    }

    //--------------------------------------------------------------------------

    void GCAmbientSound::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCAmbientSound, &GCAmbientSound::update_>(this));
    }

    void GCAmbientSound::OnAwake()
    {
        ComponentByType(Simian::C_SOUNDEMITTER, emitter_);
    }

    void GCAmbientSound::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("SoundId", soundId_);
        data.AddData("Interval", interval_);
        data.AddData("RandomInterval", randomInterval_);
    }

    void GCAmbientSound::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("SoundId", soundId_, soundId_);
        data.Data("Interval", interval_, interval_);
        data.Data("RandomInterval", randomInterval_, randomInterval_);
    }
}
