/*****************************************************************************/
/*!
\file		Sound.cpp
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	March 20, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/Sound.h"

namespace Simian
{
    Sound::Sound(size_t numChannels)
        : Audio(),
          sample_(0),
          numChannels_(numChannels)
    {
    }

    Sound::~Sound()
    {
    }

    bool Sound::IsPlaying()
    {
        if (audioState_ == AS_PLAYING && BASS_ChannelIsActive(channel_) != BASS_ACTIVE_PLAYING)
        {
            audioState_ = AS_STOPPED;
            return false;
        }
        return Audio::IsPlaying();
    }

    void Sound::Volume(f32 volume)
    {
        Audio::Volume(volume);
        BASS_ChannelSetAttribute(channel_, BASS_ATTRIB_VOL, volume);
    }

    void Sound::Pan(f32 pan)
    {
        Audio::Pan(pan);
        if (audioState_ != AS_STOPPED)
            BASS_ChannelSetAttribute(channel_, BASS_ATTRIB_PAN, pan);
    }
    
    void Sound::Position(const Simian::Vector3& position)
    {
        Audio::Position(position);
        if (audioState_ != AS_STOPPED && is3D_)
        {
            BASS_3DVECTOR position;
            position.x = position_.X();
            position.y = position_.Y();
            position.z = position_.Z();

            BASS_3DVECTOR orientation;
            orientation.x = orientation_.X();
            orientation.y = orientation_.Y();
            orientation.z = orientation_.Z();
            
            BASS_3DVECTOR velocity;
            velocity.x = velocity_.X();
            velocity.y = velocity_.Y();
            velocity.z = velocity_.Z();

            BASS_ChannelSet3DPosition(channel_, &position, &orientation, &velocity);
            BASS_Apply3D();
        }
    }

    void Sound::Orientation(const Simian::Vector3& orientation)
    {
        Audio::Orientation(orientation);
        if (audioState_ != AS_STOPPED && is3D_)
        {
            BASS_3DVECTOR position;
            position.x = position_.X();
            position.y = position_.Y();
            position.z = position_.Z();

            BASS_3DVECTOR orientation;
            orientation.x = orientation_.X();
            orientation.y = orientation_.Y();
            orientation.z = orientation_.Z();
            
            BASS_3DVECTOR velocity;
            velocity.x = velocity_.X();
            velocity.y = velocity_.Y();
            velocity.z = velocity_.Z();

            BASS_ChannelSet3DPosition(channel_, &position, &orientation, &velocity);
            BASS_Apply3D();
        }
    }

    void Sound::Velocity(const Simian::Vector3& velocity)
    {
        Audio::Velocity(velocity);
        if (audioState_ != AS_STOPPED && is3D_)
        {
            BASS_3DVECTOR position;
            position.x = position_.X();
            position.y = position_.Y();
            position.z = position_.Z();

            BASS_3DVECTOR orientation;
            orientation.x = orientation_.X();
            orientation.y = orientation_.Y();
            orientation.z = orientation_.Z();
            
            BASS_3DVECTOR velocity;
            velocity.x = velocity_.X();
            velocity.y = velocity_.Y();
            velocity.z = velocity_.Z();

            BASS_ChannelSet3DPosition(channel_, &position, &orientation, &velocity);
            BASS_Apply3D();
        }
    }

    void Sound::load_(const std::string& filePath)
    {
        Audio::load_(filePath);
    }

    void Sound::load_(const s8* buffer, size_t size)
    {                                                                                      
        sample_ = new HSAMPLE;
        *sample_ = BASS_SampleLoad(TRUE, buffer, 0, size, numChannels_, is3D_ ? BASS_SAMPLE_3D : 0);
    }

    void Sound::unload_()
    {
        BASS_SampleFree(*sample_);
        delete sample_;
        sample_ = 0;
    }

    void Sound::Play()
    {
        if (audioState_ == AS_STOPPED)
        {
            channel_ = BASS_SampleGetChannel(*sample_, FALSE);
            BASS_ChannelPlay(channel_, TRUE);
        }
        else if (audioState_ == AS_PAUSED)
            BASS_ChannelPlay(channel_, FALSE);
        audioState_ = AS_PLAYING;
    }

    void Sound::Stop()
    {
        if (audioState_ == AS_PLAYING || audioState_ == AS_PAUSED)
        {
            BASS_ChannelStop(channel_);
            audioState_ = AS_STOPPED;
        }
    }

    void Sound::Pause()
    {
        if (audioState_ == AS_PLAYING)
        {
            BASS_ChannelPause(channel_);
            audioState_ = AS_PAUSED;
        }
    }
}
