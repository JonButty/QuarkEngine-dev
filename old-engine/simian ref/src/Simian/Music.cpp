/*****************************************************************************/
/*!
\file		Music.cpp
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	March 20, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/Music.h"
#include <memory>

namespace Simian
{
    Music::Music()
        : Audio(),
          stream_(0),
          buffer_(0)
    {
    }

    Music::~Music()
    {
    }

    bool Music::IsPlaying()
    {
        if (audioState_ == AS_PLAYING && BASS_ChannelIsActive(*stream_) != BASS_ACTIVE_PLAYING)
        {
            audioState_ = AS_STOPPED;
            return false;
        }
        return Audio::IsPlaying();
    }

    void Music::Volume(f32 volume)
    {
        Audio::Volume(volume);
        BASS_ChannelSetAttribute(*stream_, BASS_ATTRIB_VOL, volume);
    }

    Simian::f32 Music::Volume()
    {
        Simian::f32 vol;
        BASS_ChannelGetAttribute(*stream_, BASS_ATTRIB_VOL, &vol);
        return vol;
    }

    void Music::Pan(f32 pan)
    {
        Audio::Pan(pan);
        if (audioState_ != AS_STOPPED)
            BASS_ChannelSetAttribute(*stream_, BASS_ATTRIB_PAN, pan);
    }
    
    void Music::Position(const Simian::Vector3& position)
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

            BASS_ChannelSet3DPosition(*stream_, &position, &orientation, &velocity);
            BASS_Apply3D();
        }
    }

    void Music::Orientation(const Simian::Vector3& orientation)
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

            BASS_ChannelSet3DPosition(*stream_, &position, &orientation, &velocity);
            BASS_Apply3D();
        }
    }

    void Music::Velocity(const Simian::Vector3& velocity)
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

            BASS_ChannelSet3DPosition(*stream_, &position, &orientation, &velocity);
            BASS_Apply3D();
        }
    }

    void Music::load_(const std::string& filePath)
    {
        stream_ = new HSTREAM;
        *stream_ = BASS_StreamCreateFile(FALSE, filePath.c_str(), 0, 0, is3D_ ? BASS_SAMPLE_3D : 0);
    }

    void Music::load_(const s8* buffer, size_t size)
    {
        stream_ = new HSTREAM;
        buffer_ = new s8[size];
        std::memcpy(buffer_, buffer, size);
        *stream_ = BASS_StreamCreateFile(TRUE, buffer_, 0, size, is3D_ ? BASS_SAMPLE_3D : 0);
    }

    void Music::unload_()
    {
        BASS_StreamFree(*stream_);
        delete stream_;
        stream_ = 0;
        delete buffer_;
        buffer_ = 0;
    }

    void Music::Play()
    {
        if (audioState_ == AS_STOPPED || audioState_ == AS_PAUSED)
            BASS_ChannelPlay(*stream_, TRUE);
        audioState_ = AS_PLAYING;
    }

    void Music::Stop()
    {
        if (audioState_ == AS_PLAYING || audioState_ == AS_PAUSED)
        {
            BASS_ChannelStop(*stream_);
            audioState_ = AS_STOPPED;
        }
    }

    void Music::Pause()
    {
        if (audioState_ == AS_PLAYING)
        {
            BASS_ChannelPause(*stream_);
            audioState_ = AS_PAUSED;
        }
    }
}
