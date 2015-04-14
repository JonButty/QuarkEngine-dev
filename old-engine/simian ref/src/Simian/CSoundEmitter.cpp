/*****************************************************************************/
/*!
\file		CSoundEmitter.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/CSoundEmitter.h"
#include "Simian/EngineComponents.h"
#include "Simian/EnginePhases.h"
#include "Simian/AudioManager.h"
#include "Simian/Sound.h"
#include "Simian/CTransform.h"

namespace Simian
{
    FactoryPlant<EntityComponent, CSoundEmitter> CSoundEmitter::factoryPlant_(
        &GameFactory::Instance().ComponentFactory(), C_SOUNDEMITTER);

    CSoundEmitter::CSoundEmitter()
        : transform_(0),
          is3D_(true)
    {
    }

    //--------------------------------------------------------------------------

    bool CSoundEmitter::SoundId( u32& soundId, const std::string& soundName )
    {
        std::vector<SoundElement>::iterator i = std::find(soundElements_.begin(), soundElements_.end(), soundName);
        soundId = i != soundElements_.end() ? i - soundElements_.begin() : soundId;
        return i != soundElements_.end();
    }

    const std::string& CSoundEmitter::SoundName( u32 soundId )
    {
        return soundElements_[soundId].Location;
    }

    //--------------------------------------------------------------------------

    void CSoundEmitter::OnAwake()
    {
        ComponentByType(C_TRANSFORM, transform_);

        // load all the sounds
        for (u32 i = 0; i < soundElements_.size(); ++i)
        {
            Sound* sound = AudioManager::Instance().LoadSound(soundElements_[i].Location, is3D_);
            sounds_.push_back(sound);
        }
    }

    void CSoundEmitter::Play( u32 soundId, f32 volume )
    {
        if (sounds_[soundId]->IsPlaying())
            sounds_[soundId]->Stop(); // stop if already playing
        sounds_[soundId]->Play();
        sounds_[soundId]->Volume(volume >= 0.0f ? volume : soundElements_[soundId].Volume);
        sounds_[soundId]->Position(transform_->World().Position());
    }

    void CSoundEmitter::Play( const std::string& soundName, f32 volume )
    {
        u32 soundid;
        if (SoundId(soundid, soundName))
            Play(soundid, volume);
    }

    void CSoundEmitter::Stop( u32 soundId )
    {
        sounds_[soundId]->Stop();
    }

    void CSoundEmitter::StopAll()
    {
        for (u32 i = 0; i < sounds_.size(); ++i)
            sounds_[i]->Stop();
    }

    void CSoundEmitter::Serialize( FileObjectSubNode& data )
    {
        FileArraySubNode* sounds = data.AddArray("Sounds");
        for (u32 i = 0; i < soundElements_.size(); ++i)
        {
            FileObjectSubNode* sound = sounds->AddObject("Sound");
            sound->AddData("Location", soundElements_[i].Location);
            sound->AddData("Volume", soundElements_[i].Volume);
        }

        data.AddData("Is3D", is3D_);
    }

    void CSoundEmitter::Deserialize( const FileObjectSubNode& data )
    {
        const FileArraySubNode* sounds = data.Array("Sounds");
        if (sounds)
        {
            soundElements_.clear();
            for (u32 i = 0; i < sounds->Size(); ++i)
            {
                const FileObjectSubNode* sound = sounds->Object(i);
                SoundElement element;
                sound->Data("Location", element.Location, "");
                sound->Data("Volume", element.Volume, 1.0f);
                soundElements_.push_back(element);
            }
        }

        data.Data("Is3D", is3D_);
    }
}
