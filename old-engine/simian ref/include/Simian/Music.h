/*****************************************************************************/
/*!
\file		Music.h
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	March 20, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_MUSIC_H_
#define SIMIAN_MUSIC_H_

#include "SimianPlatform.h"
#include "Audio.h"
#include "bass.h"

namespace Simian
{
    class SIMIAN_EXPORT Music: public Audio
    {
    private:
        HSTREAM* stream_;
        s8* buffer_;
    public:
        bool IsPlaying();

        void Volume(f32 volume);
        f32 Volume();
        void Pan(f32 pan);
        void Position(const Simian::Vector3& position);
        void Orientation(const Simian::Vector3& orientation);
        void Velocity(const Simian::Vector3& velocity);
    private:
        Music();
        virtual ~Music();
        void load_(const std::string& filePath);
        void load_(const s8* buffer, size_t size);
        void unload_();
    public:
        void Play();
        void Stop();
        void Pause();

        friend class AudioManager;
    };
}

#endif
