/*****************************************************************************/
/*!
\file		AudioManager.cpp
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	March 20, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/AudioManager.h"
#include "Simian/Debug.h"
#include "Simian/Utility.h"
#include "Simian/Audio.h"
#include "Simian/Sound.h"
#include "Simian/Music.h"

#include "bass.h"

#include <algorithm>
#include <functional>

namespace Simian
{
    static const DWORD MAX_VOLUME = 10000;
    static const DWORD FREQUENCY = 44100;
    static const size_t INSTANCE_COUNT = 100;

    AudioManager::AudioManager()
        : distance_(1.0f),
          roll_(1.0f),
          doppler_(1.0f),
          front_(Simian::Vector3::UnitZ),
          top_(Simian::Vector3::UnitY),
          musicVolume_(1.0f),
          soundVolume_(1.0f)
    {
        instances_.reserve(INSTANCE_COUNT);
    }

    AudioManager::~AudioManager()
    {
    }

    void AudioManager::Position(const Vector3& pos, const Vector3& vel, const Vector3& f, const Vector3& t)
    {
        position_ = pos;
        velocity_ = vel;
        front_ = f;
        top_ = t;

        BASS_3DVECTOR position;
        position.x = position_.X();
        position.y = position_.Y();
        position.z = position_.Z();
        
        BASS_3DVECTOR velocity;
        velocity.x = velocity_.X();
        velocity.y = velocity_.Y();
        velocity.z = velocity_.Z();
        
        BASS_3DVECTOR front;
        front.x = front_.X();
        front.y = front_.Y();
        front.z = front_.Z();
        
        BASS_3DVECTOR top;
        top.x = top_.X();
        top.y = top_.Y();
        top.z = top_.Z();
        BASS_Set3DPosition(&position, &velocity, &front, &top);
        BASS_Apply3D();
    }

    const Vector3& AudioManager::Position() const
    {
        return position_;
    }

    const Vector3& AudioManager::Velocity() const
    {
        return velocity_;
    }

    const Vector3& AudioManager::Front() const
    {
        return front_;
    }

    const Vector3& AudioManager::Top() const
    {
        return top_;
    }

    void AudioManager::SetFactors(f32 distance, f32 roll, f32 doppler)
    {
        distance_ = distance;
        roll_ = roll;
        doppler_ = doppler;

        BASS_Set3DFactors(distance_, roll_, doppler_);
        BASS_Apply3D();
    }

    f32 AudioManager::DistanceFactor() const
    {
        return distance_;
    }

    f32 AudioManager::RollFactor() const
    {
        return roll_;
    }

    f32 AudioManager::DopplerFactor() const
    {
        return doppler_;
    }
    
    void AudioManager::MasterVolume(f32 volume)
    {
        musicVolume_ = volume;
        soundVolume_ = volume;
        BASS_SetConfig(BASS_CONFIG_GVOL_MUSIC, static_cast<DWORD>(musicVolume_ * MAX_VOLUME));
        BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, static_cast<DWORD>(musicVolume_ * MAX_VOLUME));
        BASS_SetConfig(BASS_CONFIG_GVOL_SAMPLE, static_cast<DWORD>(soundVolume_ * MAX_VOLUME));
    }

    Simian::f32 AudioManager::MusicVolume() const
    {
        return musicVolume_;
    }

    void AudioManager::MusicVolume( Simian::f32 val )
    {
        musicVolume_ = val;
        BASS_SetConfig(BASS_CONFIG_GVOL_MUSIC, static_cast<DWORD>(musicVolume_ * MAX_VOLUME));
        BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, static_cast<DWORD>(musicVolume_ * MAX_VOLUME));
    }

    Simian::f32 AudioManager::SoundVolume() const
    {
        return soundVolume_;
    }

    void AudioManager::SoundVolume( Simian::f32 val )
    {
        soundVolume_ = val;
        BASS_SetConfig(BASS_CONFIG_GVOL_SAMPLE, static_cast<DWORD>(soundVolume_ * MAX_VOLUME));
    }

    //--------------------------------------------------------------------------

    bool AudioManager::Startup(const WindowHandle& wind)
    {
        BOOL success = BASS_Init(-1, FREQUENCY, BASS_DEVICE_3D, wind ? *reinterpret_cast<HWND*>(wind) : NULL, NULL);
        if (success != TRUE)
            return false;

        Position(position_, velocity_, front_, top_);

        return true;
    }

    void AudioManager::Shutdown()
    {
        BASS_Free();
    }

    void AudioManager::Update(f32 dt)
    {
        DWORD time = static_cast<DWORD>(dt*1000.0f);
        BASS_Update(time);
    }

    Sound* AudioManager::LoadSound(const std::string& filePath, bool is3D, size_t numChannels)
    {
        Sound* newSound = new Sound(numChannels);
        std::map<std::string, Audio*>::iterator i = soundCache_.find(filePath);
        if (i == soundCache_.end() )
        {
            Sound* sound = new Sound(numChannels);
            sound->is3D_ = is3D;
            sound->load_(filePath);
            soundCache_[filePath] = sound;
            *newSound = *sound;
        }
        else
            *newSound = *reinterpret_cast<Sound*>((*i).second);
        instances_.push_back(newSound);
        return newSound;
    }

    /*
    Sound* AudioManager::LoadSound(const char* buffer, size_t size, bool is3D, size_t numChannels)
    {
        Sound* newSound = new Sound(numChannels);
        std::map<std::string, Audio*>::iterator i = soundCache_.find(buffer);
        if (i == soundCache_.end() )
        {
            Sound* sound = new Sound(numChannels);
            sound->is3D_ = is3D;
            sound->load_(buffer, size);
            soundCache_[buffer] = sound;
            *newSound = *sound;
        }
        else
            *newSound = *reinterpret_cast<Sound*>((*i).second);
        instances_.push_back(newSound);
        return newSound;
    }
    */

    Music* AudioManager::LoadMusic(const std::string& filePath, bool is3D)
    {
        Music* newMusic = new Music();
        std::map<std::string, Audio*>::iterator i = soundCache_.find(filePath);
        if (i == soundCache_.end() )
        {
            Music* music = new Music();
            music->is3D_ = is3D;
            music->load_(filePath);
            soundCache_[filePath] = music;
            *newMusic = *music;
        }
        else
            *newMusic = *reinterpret_cast<Music*>((*i).second);
        instances_.push_back(newMusic);
        return newMusic;
    }

    /*
    Music* AudioManager::LoadMusic(const char* buffer, size_t size, bool is3D)
    {
        Music* newMusic = new Music();
        std::map<std::string, Audio*>::iterator i = soundCache_.find(buffer);
        if (i == soundCache_.end() )
        {
            Music* music = new Music();
            music->is3D_ = is3D;
            music->load_(buffer, size);
            soundCache_[buffer] = music;
            *newMusic = *music;
        }
        else
            *newMusic = *reinterpret_cast<Music*>((*i).second);
        instances_.push_back(newMusic);
        return newMusic;
    }
    */

    void AudioManager::StopAll()
    {
        std::for_each(instances_.begin(), instances_.end(),
            std::mem_fun(&Audio::Stop));

        for (std::map<std::string, Audio*>::iterator i = soundCache_.begin();
            i != soundCache_.end(); ++i)
        {
            (*i).second->Stop();
        }
    }

    void AudioManager::UnloadAll()
    {
        for (size_t i = 0; i < instances_.size(); ++i)
            delete instances_[i];
        instances_.clear();

        for (std::map<std::string, Audio*>::iterator i = soundCache_.begin();
            i != soundCache_.end(); ++i)
        {
            (*i).second->unload_();
            delete (*i).second;
        }
        soundCache_.clear();
    }

    void AudioManager::Mute()
    {
        BASS_SetConfig(BASS_CONFIG_GVOL_MUSIC, 0);
        BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 0);
        BASS_SetConfig(BASS_CONFIG_GVOL_SAMPLE, 0);
    }

    void AudioManager::Unmute()
    {
        BASS_SetConfig(BASS_CONFIG_GVOL_MUSIC, static_cast<DWORD>(musicVolume_ * MAX_VOLUME));
        BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, static_cast<DWORD>(musicVolume_ * MAX_VOLUME));
        BASS_SetConfig(BASS_CONFIG_GVOL_SAMPLE, static_cast<DWORD>(soundVolume_ * MAX_VOLUME));
    }
}
