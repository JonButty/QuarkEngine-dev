/************************************************************************/
/*!
\file		CJukebox.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/CJukebox.h"
#include "Simian/EngineComponents.h"
#include "Simian/EnginePhases.h"
#include "Simian/AudioManager.h"
#include "Simian/Music.h"
#include "Simian/Math.h"

namespace Simian
{
    const Simian::f32 CJukebox::VOLUME_ORIGINAL_MULTIPLIER = 1.0f;
    const Simian::f32 CJukebox::VOLUME_GAME_PAUSE_MULTIPLIER = 0.2f;

    FactoryPlant<EntityComponent, CJukebox> CJukebox::factoryPlant_(
        &GameFactory::Instance().ComponentFactory(), C_JUKEBOX);

    CJukebox::CJukebox()
        : currentTrackId_(0),
          shuffle_(false),
          repeat_(RM_NOREPEAT),
          startPlaying_(true),
          fading_(false),
          fadingTrackId_(0),
          timer_(0),
          fadeDuration_(0),
          volumeMultiplier_(1.0f),
          stopped_(false)
    {
    }

    //--------------------------------------------------------------------------

    Simian::u32 CJukebox::CurrentTrackId() const
    {
        return currentTrackId_;
    }

    void CJukebox::CurrentTrackId( Simian::u32 val )
    {
        currentTrackId_ = val;
    }

    bool CJukebox::Shuffle() const
    {
        return shuffle_;
    }

    void CJukebox::Shuffle( bool val )
    {
        shuffle_ = val;
    }

    Simian::CJukebox::RepeatMode CJukebox::Repeat() const
    {
        return repeat_;
    }

    void CJukebox::Repeat( RepeatMode val )
    {
        repeat_ = val;
    }

    Simian::f32 CJukebox::VolumeMultiplier() const
    {
        return volumeMultiplier_;
    }

    void CJukebox::VolumeMultiplier( Simian::f32 val )
    {
        volumeMultiplier_ = val;
    }

    const std::string& CJukebox::TrackName( u32 trackId )
    {
        return trackFiles_[trackId];
    }

    bool CJukebox::TrackId( u32& trackId, const std::string& trackName )
    {
        std::vector<std::string>::iterator i = std::find(trackFiles_.begin(), trackFiles_.end(), trackName);
        trackId = i != trackFiles_.end() ? i - trackFiles_.begin() : trackId;
        return i != trackFiles_.end();
    }

    //--------------------------------------------------------------------------

    void CJukebox::update_( DelegateParameter& param )
    {
        EntityUpdateParameter* p;
        param.As(p);

        timer_ += p->Dt;

        // if cross fading
        if (fading_)
        {
            f32 intp = timer_/fadeDuration_;
            intp = Math::Clamp(intp, 0.0f, 1.0f);
            
            tracks_[fadingTrackId_]->Volume(intp*volumeMultiplier_);
            tracks_[currentTrackId_]->Volume((1.0f - intp) *volumeMultiplier_);

            if (intp >= 1.0f)
            {
                // cross fading done!
                tracks_[currentTrackId_]->Stop();
                currentTrackId_ = fadingTrackId_;
                fading_ = false;
            }
        }
        else if (currentTrackId_ < tracks_.size() && !tracks_[currentTrackId_]->IsPlaying() && !stopped_)
        {
            // if the song has finished
            PlayNext();
        }
        else if (currentTrackId_ < tracks_.size() && tracks_[currentTrackId_]->IsPlaying())
            tracks_[currentTrackId_]->Volume(volumeMultiplier_);
    }

    //--------------------------------------------------------------------------

    void CJukebox::OnSharedLoad()
    {
        RegisterCallback(P_UPDATE, Delegate::CreateDelegate<CJukebox, &CJukebox::update_>(this));
    }

    void CJukebox::OnAwake()
    {
        // load all the tracks
        for (u32 i = 0; i < trackFiles_.size(); ++i)
        {
            Music* music = AudioManager::Instance().LoadMusic(trackFiles_[i]);
            if (!music)
                continue;
            tracks_.push_back(music);
            
            // don't add the initial track the shuffle list
            if (i != currentTrackId_)
                shuffleList_.push_back(music);
        }

        // shuffle the shuffle list
        std::random_shuffle(shuffleList_.begin(), shuffleList_.end());

        if (!tracks_.size())
            stopped_ = true;

        if (startPlaying_ && currentTrackId_ < tracks_.size())
            Play(currentTrackId_);
    }

    void CJukebox::Play( u32 trackId )
    {
        // stop the current track
        tracks_[currentTrackId_]->Stop();
        currentTrackId_ = trackId;
        tracks_[currentTrackId_]->Volume(1.0f*volumeMultiplier_);
        tracks_[currentTrackId_]->Play();
    }

    void CJukebox::Play( const std::string& trackName )
    {
        u32 trackid;
        if (TrackId(trackid, trackName))
            Play(trackid);
    }

    void CJukebox::Stop()
    {
        tracks_[currentTrackId_]->Stop();
        stopped_ = true;
    }

    void CJukebox::PlayNext()
    {
        if (currentTrackId_ < tracks_.size())
            tracks_[currentTrackId_]->Stop();

        // check repeat mode first
        if (repeat_ == RM_REPEATSINGLE)
            tracks_[currentTrackId_]->Play();
        else
        {
            if (shuffle_)
            {
                // pop the back off the shuffled list
                if (shuffleList_.size())
                {
                    shuffleList_.back()->Play();
                    shuffleList_.pop_back();
                }
                else if (repeat_ == RM_REPEATALL)
                {
                    for (u32 i = 0; i < tracks_.size(); ++i)
                        shuffleList_.push_back(tracks_[i]);
                    std::random_shuffle(shuffleList_.begin(), shuffleList_.end());
                    shuffleList_.back()->Play();
                    shuffleList_.pop_back();
                }
            }
            else if (currentTrackId_ < tracks_.size())
            {
                ++currentTrackId_;
                if (currentTrackId_ > tracks_.size())
                {
                    // repeat? 
                    if (repeat_ == RM_REPEATALL)
                    {
                        currentTrackId_ = 0;
                        tracks_[currentTrackId_]->Play();
                    }
                }
                else
                    tracks_[currentTrackId_]->Play();
            }
        }
    }

    void CJukebox::CrossFade( u32 trackId, float time )
    {
        if (trackId == currentTrackId_ || 
            trackId > trackFiles_.size())
            return;

        // if a track was fading in..
        if (fading_ && fadingTrackId_ < tracks_.size())
            tracks_[fadingTrackId_]->Stop();

        fading_ = true;
        timer_ = 0;
        fadeDuration_ = time;
        fadingTrackId_ = trackId;

        tracks_[fadingTrackId_]->Volume(0.0f*volumeMultiplier_);
        tracks_[fadingTrackId_]->Play();
    }

    void CJukebox::CrossFade( const std::string& trackName, float time )
    {
        u32 trackid;
        if (TrackId(trackid, trackName))
            CrossFade(trackid, time);
    }

    void CJukebox::Serialize( FileObjectSubNode& data )
    {
        data.AddData("InitialTrack", currentTrackId_);
        data.AddData("Shuffle", shuffle_);
        data.AddData("StartPlaying", startPlaying_);

        data.AddData("Repeat", repeat_ == RM_NOREPEAT ? "NOREPEAT" :
                               repeat_ == RM_REPEATSINGLE ? "REPEATSINGLE" :
                               repeat_ == RM_REPEATALL ? "REPEATALL" :
                               "NOREPEAT");

        FileArraySubNode* tracks = data.AddArray("Tracks");
        for (u32 i = 0; i < trackFiles_.size(); ++i)
            tracks->AddData("Track", trackFiles_[i]);
    }

    void CJukebox::Deserialize( const FileObjectSubNode& data )
    {
        data.Data("InitialTrack", currentTrackId_, currentTrackId_);
        data.Data("Shuffle", shuffle_, shuffle_);
        data.Data("StartPlaying", startPlaying_, startPlaying_);

        std::string repeat;
        data.Data("Repeat", repeat, "");
        repeat_ = repeat == "NOREPEAT" ? RM_NOREPEAT :
                  repeat == "REPEATSINGLE" ? RM_REPEATSINGLE :
                  repeat == "REPEATALL" ? RM_REPEATALL :
                  repeat_;

        const FileArraySubNode* tracks = data.Array("Tracks");
        if (tracks)
        {
            trackFiles_.clear();
            for (u32 i = 0; i < tracks->Size(); ++i)
            {
                const FileDataSubNode* track = tracks->Data(i);
                trackFiles_.push_back(track->AsString());
            }
        }
    }
}
