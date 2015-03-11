/*****************************************************************************/
/*!
\file		Audio.h
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	March 20, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_AUDIO_H_
#define SIMIAN_AUDIO_H_

#include "SimianPlatform.h"
#include "Vector3.h"
#include <string>

namespace Simian
{
    class SIMIAN_EXPORT Audio
    {
    protected:
        enum AudioState
        {
            AS_PLAYING,
            AS_PAUSED,
            AS_STOPPED
        };
    protected:
        f32 volume_;
        f32 pan_;
        bool is3D_;
        AudioState audioState_;
        Simian::Vector3 position_;
        Simian::Vector3 orientation_;
        Simian::Vector3 velocity_;
    public:
        f32 Volume() const;
        virtual void Volume(f32 volume);

        f32 Pan() const;
        virtual void Pan(f32 pan);

        bool Is3D() const;
        virtual bool IsPlaying();
        bool IsPaused() const;
        bool IsStopped() const;

        const Simian::Vector3& Position() const;
        virtual void Position(const Simian::Vector3& position);

        const Simian::Vector3& Orientation() const;
        virtual void Orientation(const Simian::Vector3& orientation);

        const Simian::Vector3& Velocity() const;
        virtual void Velocity(const Simian::Vector3& velocity);
    protected:
        Audio();
        virtual ~Audio();
        virtual void load_(const std::string& filePath);
        virtual void load_(const s8* buffer, size_t size) = 0;
        virtual void unload_() = 0;
    public:
        virtual void Play() = 0;
        virtual void Stop() = 0;
        virtual void Pause() = 0;

        friend class AudioManager;
    };
}

#endif
