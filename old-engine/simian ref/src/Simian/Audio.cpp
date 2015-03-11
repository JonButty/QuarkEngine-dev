/*****************************************************************************/
/*!
\file		Audio.cpp
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	March 20, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/Audio.h"
#include "Simian/FileSystem.h"

namespace Simian
{
    Audio::Audio()
        : volume_(1.0f),
          pan_(0.0f),
          is3D_(false),
          audioState_(AS_STOPPED)
    {
    }

    Audio::~Audio()
    {
    }

    f32 Audio::Volume() const
    {
        return volume_;
    }

    void Audio::Volume(f32 volume)
    {
        volume_ = volume;
    }

    f32 Audio::Pan() const
    {
        return pan_;
    }

    void Audio::Pan(f32 pan)
    {
        pan_ = pan;
    }

    bool Audio::Is3D() const
    {
        return is3D_;
    }

    bool Audio::IsPlaying()
    {
        return audioState_ == AS_PLAYING;
    }

    bool Audio::IsPaused() const
    {
        return audioState_ == AS_PAUSED;
    }

    bool Audio::IsStopped() const
    {
        return audioState_ == AS_STOPPED;
    }

    const Simian::Vector3& Audio::Position() const
    {
        return position_;
    }

    void Audio::Position(const Simian::Vector3& position)
    {
        position_ = position;
    }

    const Simian::Vector3& Audio::Orientation() const
    {
        return orientation_;
    }

    void Audio::Orientation(const Simian::Vector3& orientation)
    {
        orientation_ = orientation;
    }

    const Simian::Vector3& Audio::Velocity() const
    {
        return velocity_;
    }

    void Audio::Velocity(const Simian::Vector3& velocity)
    {
        velocity_ = velocity;
    }

    void Audio::load_(const std::string& filePath)
    {
        s8* buffer;
        size_t size;
        Simian::FileSystem::MemoryFromFilePath(filePath, buffer, size);

        load_(buffer, size);
        delete[] buffer;
    }
}
