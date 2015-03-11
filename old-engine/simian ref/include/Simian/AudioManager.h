/*****************************************************************************/
/*!
\file		AudioManager.h
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	March 20, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_AUDIOMANAGER_H_
#define SIMIAN_AUDIOMANAGER_H_

#include "SimianPlatform.h"
#include "Singleton.h"
#include "OS.h"
#include <map>
#include <string>
#include <vector>

#include "Vector3.h"

namespace Simian
{
    class Audio;
    class Sound;
    class Music;

    class SIMIAN_EXPORT AudioManager: public Singleton<AudioManager>
    {
    private:
        std::map<std::string, Audio*> soundCache_;
        std::vector<Audio*> instances_;
        Vector3 position_;
        Vector3 velocity_;
        Vector3 top_;
        Vector3 front_;
        f32 distance_, roll_, doppler_;
        f32 musicVolume_;
        f32 soundVolume_;
    public:
        void Position(const Vector3& position, const Vector3& velocity, const Vector3& front, const Vector3& top);
        const Vector3& Position() const;
        const Vector3& Velocity() const;
        const Vector3& Front() const;
        const Vector3& Top() const;

        void SetFactors(f32 distance, f32 roll, f32 doppler);
        f32 DistanceFactor() const;
        f32 RollFactor() const;
        f32 DopplerFactor() const;

        void MasterVolume(f32 volume);

        Simian::f32 MusicVolume() const;
        void MusicVolume(Simian::f32 val);

        Simian::f32 SoundVolume() const;
        void SoundVolume(Simian::f32 val);
    private:
        AudioManager();
        virtual ~AudioManager();
    public:
        bool Startup(const WindowHandle& wind);
        void Shutdown();
        void Update(f32 dt);

        Sound* LoadSound(const std::string& filePath, bool is3D = false, size_t numChannels = 65535);
        //Sound* LoadSound(const char* buffer, size_t size, bool is3D = false, size_t numChannels = 65535);
        Music* LoadMusic(const std::string& filePath, bool is3D = false);
        //Music* LoadMusic(const char* buffer, size_t size, bool is3D = false);
        void StopAll();
        void UnloadAll();

        void Mute();
        void Unmute();

        friend class Singleton<AudioManager>;
    };

    SIMIAN_IMPORT template class SIMIAN_EXPORT Singleton<AudioManager>;
}

#endif
