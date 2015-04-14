/************************************************************************/
/*!
\file		VideoClipTheora.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_VIDEOCLIPTHEORA_H_
#define SIMIAN_VIDEOCLIPTHEORA_H_

#include "SimianPlatform.h"
#if SIMIAN_VIDEO == SIMIAN_VIDEO_THEORA

#include "VideoClip.h"
#include "LockedAttribute.h"

#include "ogg/ogg.h"
#include "theora/codec.h"
#include "theora/theoradec.h"
#include "vorbis/codec.h"

#include "bass.h"

#include <string>
#include <map>
#include <fstream>

namespace Simian
{
    class SIMIAN_EXPORT VideoClip: public VideoClipBase
    {
    private:
        struct OggStream
        {
            ogg_stream_state State;
            bool Loaded;
            bool EOS;
        };
    private:
        ogg_sync_state oggState_;
        std::map<s32, OggStream*> streamTable_;
        std::ifstream file_;
        u32 fileSize_;
        bool loaded_;

        OggStream* theoraStream_;
        OggStream* vorbisStream_;

        // theora stuff
        th_info thInfo_;
        th_comment thComment_;
        th_setup_info* thSetup_;
        th_dec_ctx* thDec_;

        // vorbis stuff
        vorbis_info vInfo_;
        vorbis_comment vComment_;
        vorbis_dsp_state vState_;
        vorbis_block vBlock_;
        f32 audioLastTime_;
        f32 videoLastTime_;
        f32 videoTimePassed_;
        bool packetsRemaining_;

        // audio playback
        HSTREAM audioStream_;
    private:
        void play_();
        void pause_();
        void stop_();

        void seek_(f32 normalizedTime);
        void update_(f32 dt);

        bool loadStreams_();
        void unloadStreams_();

        bool initStreams_();
        void processHeaders_(OggStream* stream);
        bool processTheoraHeaders_(OggStream* stream, ogg_packet* packet);
        bool processVorbisHeaders_(OggStream* stream, ogg_packet* packet);
        bool loadPage_(OggStream*& stream);

        void processVideoStream_(f32 dt);
        void processAudioStream_();
    public:
        VideoClip();
        ~VideoClip();

        // we do not use datalocations for movie clips since datalocations loads
        // the entire file into memory.. (we should probably improve this)
        bool Load(const std::string& file);
        void Unload();

        void Play();
        void Pause();
        void Stop();
        void Seek(f32 normalizedTime);
        void Update(f32 dt);

        // expects an RGB buffer
        bool ExtractFrame(u8** y, u32& yStride, u8** u, u32& uStride, u8** v, u32& vStride);
    };
}

#endif

#endif
