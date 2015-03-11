/************************************************************************/
/*!
\file		CJukebox.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CJUKEBOX_H_
#define SIMIAN_CJUKEBOX_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"

#include <vector>

namespace Simian
{
    class Music;

    class SIMIAN_EXPORT CJukebox: public EntityComponent
    {
    public:
        static const Simian::f32 VOLUME_ORIGINAL_MULTIPLIER;
        static const Simian::f32 VOLUME_GAME_PAUSE_MULTIPLIER;

    public:
        enum RepeatMode
        {
            RM_NOREPEAT,
            RM_REPEATSINGLE,
            RM_REPEATALL
        };
    private:
        std::vector<std::string> trackFiles_;
        std::vector<Music*> tracks_;
        std::vector<Music*> shuffleList_;
        u32 currentTrackId_;
        bool shuffle_;
        RepeatMode repeat_;
        bool startPlaying_;
        bool stopped_;

        bool fading_;
        u32 fadingTrackId_;
        f32 timer_;
        f32 fadeDuration_;
        f32 volumeMultiplier_;

        static FactoryPlant<EntityComponent, CJukebox> factoryPlant_;
    public:
        Simian::u32 CurrentTrackId() const;
        void CurrentTrackId(Simian::u32 val);

        bool Shuffle() const;
        void Shuffle(bool val);

        Simian::f32 VolumeMultiplier() const;
        void VolumeMultiplier(Simian::f32 val);

        Simian::CJukebox::RepeatMode Repeat() const;
        void Repeat(Simian::CJukebox::RepeatMode val);

        const std::string& TrackName(u32 trackId);
        bool TrackId(u32& trackId, const std::string& trackName);
    private:
        void update_(DelegateParameter& param);
    public:
        CJukebox();

        void OnSharedLoad();
        void OnAwake();

        void Play(u32 trackId);
        void Play(const std::string& trackName);
        void Stop();
        void PlayNext();
        void CrossFade(u32 trackId, float time);
        void CrossFade(const std::string& trackName, float time);

        void Serialize( FileObjectSubNode& data );
        void Deserialize( const FileObjectSubNode& data );
    };
}

#endif
