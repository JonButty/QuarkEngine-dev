/************************************************************************/
/*!
\file		VideoClipTheora.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/SimianPlatform.h"
#if SIMIAN_VIDEO == SIMIAN_VIDEO_THEORA

#include <fstream>

#include "Simian/VideoClipTheora.h"
#include "Simian/LogManager.h"
#include "Simian/Math.h"
#include "Simian/LockedAttribute.h"
#include "Simian/Debug.h"

namespace Simian
{
    static const u32 BUFFER_SIZE = 4096;
    static const f32 AUDIO_BUFFER_TIME = 5.0f;

    VideoClip::VideoClip()
        : fileSize_(0),
          loaded_(false),
          theoraStream_(0),
          vorbisStream_(0),
          thSetup_(0),
          thDec_(0),
          audioStream_(0),
          audioLastTime_(0),
          videoLastTime_(0),
          videoTimePassed_(0)
    {
    }

    VideoClip::~VideoClip()
    {
    }

    //--------------------------------------------------------------------------

    void VideoClip::play_()
    {
        BASS_ChannelPlay(audioStream_, false);
    }

    void VideoClip::pause_()
    {
        BASS_ChannelPause(audioStream_);
    }

    void VideoClip::stop_()
    {
        unloadStreams_();
        file_.seekg(0, std::ios::beg);
        //loadStreams_();
    }

    void VideoClip::seek_( f32 normalizedTime )
    {
        normalizedTime;

        // approximate position then synchronize page
    }

    void VideoClip::update_( f32 dt )
    {
        // try and read packets
        if (State() == PS_PLAYING)
        {
            processAudioStream_();
            processVideoStream_(dt);

            // reached the end of the stream
            if (vorbisStream_->EOS && !packetsRemaining_ &&
                BASS_ChannelIsActive(audioStream_) == BASS_ACTIVE_STOPPED)
            {
                stop_();
                State(PS_STOPPED);
            }
        }
    }

    bool VideoClip::loadStreams_()
    {
        // initialize the sync state
        if (ogg_sync_init(&oggState_))
        {
            SError("Failed to initialize ogg state.")
            unloadStreams_();
            return false;
        }

        // find the file size
        file_.seekg(0, std::ios::end);
        fileSize_ = static_cast<u32>(file_.tellg());
        file_.seekg(0, std::ios::beg);

        // create theora structs
        th_info_init(&thInfo_);
        th_comment_init(&thComment_);

        // create vorbis structs
        vorbis_info_init(&vInfo_);
        vorbis_comment_init(&vComment_);

        // initialize streams
        if (!initStreams_())
        {
            SError("Failed to initialize streams.");
            unloadStreams_();
            return false;
        }

        // setup properties
        // theora
        Width(thInfo_.frame_width);
        Height(thInfo_.frame_height);
        Fps((f32)thInfo_.fps_numerator/thInfo_.fps_denominator);
        thDec_ = th_decode_alloc(&thInfo_, thSetup_);

        // vorbis
        if (vorbis_synthesis_init(&vState_, &vInfo_))
        {
            SError("Failed to initialize vorbis stream.");
            unloadStreams_();
            return false;
        }
        if (vorbis_block_init(&vState_, &vBlock_))
        {
            SError("Failed to initialize vorbis block.");
            unloadStreams_();
            return false;
        }

        // set up playback stream
        audioStream_ = BASS_StreamCreate(vInfo_.rate, vInfo_.channels, BASS_SAMPLE_FLOAT, STREAMPROC_PUSH, 0);
        if (!audioStream_)
        {
            SError("Failed to create audio playback stream.");
            unloadStreams_();
            return false;
        }

        packetsRemaining_ = true;

        return true;
    }

    void VideoClip::unloadStreams_()
    {
        // unload audio playback stream
        if (audioStream_)
            BASS_StreamFree(audioStream_);
        audioStream_ = 0;

        if (theoraStream_)
        {
            // unload theora structs
            if (thDec_)
                th_decode_free(thDec_);
            if (thSetup_)
                th_setup_free(thSetup_);
            thDec_ = 0;
            thSetup_ = 0;
            th_comment_clear(&thComment_);
            th_info_clear(&thInfo_);
        }
        theoraStream_ = 0;

        if (vorbisStream_)
        {
            // unload vorbis structs
            vorbis_block_clear(&vBlock_);
            vorbis_dsp_clear(&vState_);
            vorbis_comment_clear(&vComment_);
            vorbis_info_clear(&vInfo_);
        }
        vorbisStream_ = 0;

        // unload all streams
        for (std::map<s32, OggStream*>::iterator i = streamTable_.begin();
            i != streamTable_.end(); ++i)
        {
            ogg_stream_clear(&i->second->State);
            delete i->second;
        }
        streamTable_.clear();
    }

    bool VideoClip::initStreams_()
    {
        // keep loading data until we have a video and audio stream
        while ((!theoraStream_ || !theoraStream_->Loaded) ||
               (!vorbisStream_ || !vorbisStream_->Loaded))
        {
            // load a page
            OggStream* stream;
            if (!loadPage_(stream))
            {
                SError("Page loading failed before headers were read.");
                return false;
            }

            // process the stream for headers
            if (!stream->Loaded)
                processHeaders_(stream);
        }

        if (theoraStream_ && vorbisStream_)
            return true;

        SError("Missing Video/Audio stream...");

        return false;
    }

    void VideoClip::processHeaders_(OggStream* stream)
    {
        ogg_packet packet;
        while (ogg_stream_packetpeek(&stream->State, &packet) != 0)
        {
            // don't continue with this stream if done loading
            bool decoded = processTheoraHeaders_(stream, &packet);
            decoded = decoded || processVorbisHeaders_(stream, &packet);

            if (!decoded || stream->Loaded)
                break;
        }
    }

    bool VideoClip::processTheoraHeaders_(OggStream* stream, ogg_packet* packet)
    {
        s32 ret = th_decode_headerin(&thInfo_, &thComment_, &thSetup_, packet);
        if (ret == TH_ENOTFORMAT)
            return false;

        // consume packet if header was read
        if (ret > 0)
        {
            theoraStream_ = stream;
            ogg_stream_packetout(&stream->State, packet);
        }
        else if (ret == 0 && stream == theoraStream_)
            stream->Loaded = true; // finished loading
        else
            return false;

        return true;
    }

    bool VideoClip::processVorbisHeaders_( OggStream* stream, ogg_packet* packet )
    {
        s32 ret = vorbis_synthesis_headerin(&vInfo_, &vComment_, packet);
        if (ret == OV_ENOTVORBIS && stream != vorbisStream_)
            return false;

        if (ret == 0)
        {
            // vorbis stream, consume
            vorbisStream_ = stream;
            ogg_stream_packetout(&stream->State, packet);
        }
        else if (stream == vorbisStream_ && ret == OV_ENOTVORBIS)
            stream->Loaded = true;
        else
            return false;

        return true;
    }

    bool VideoClip::loadPage_( OggStream*& stream )
    {
        // load a page of data then return the stream it was dumped into
        ogg_page page;
        while (ogg_sync_pageout(&oggState_, &page) != 1)
        {
            char* buffer = ogg_sync_buffer(&oggState_, BUFFER_SIZE);
            
            file_.read(buffer, BUFFER_SIZE);
            if (!file_.gcount())
                return false;

            if (ogg_sync_wrote(&oggState_, BUFFER_SIZE))
                return false;
        }

        // get associated stream
        s32 serial = ogg_page_serialno(&page);
        if (ogg_page_bos(&page))
        {
            // new page
            streamTable_[serial] = new OggStream();
            streamTable_[serial]->Loaded = false;
            streamTable_[serial]->EOS = false;
            if (ogg_stream_init(&streamTable_[serial]->State, serial))
                return false;
        }
        stream = streamTable_[serial];
        
        // signal eos
        if (ogg_page_eos(&page))
            stream->EOS = true;

        ogg_stream_pagein(&stream->State, &page);

        return true;
    }

    void VideoClip::processVideoStream_(f32 dt)
    {
        f32 position = (f32)BASS_ChannelGetPosition(audioStream_, BASS_POS_BYTE)/
            (f32)vInfo_.rate / (f32)vInfo_.channels / (f32)sizeof(f32);
        bool over = BASS_ChannelIsActive(audioStream_) == BASS_ACTIVE_STOPPED;

        f32 timePerFrame = 1.0f/Fps();
        videoTimePassed_ += dt;

        // we must wait at least the time of 1 frame
        // and the audio must exceed the video playback
        if (!over && (videoLastTime_ > position || videoTimePassed_ < timePerFrame))
            return;

        if (over && videoTimePassed_ < timePerFrame)
            return;

        // this is when we reached because of a frame delay and not a sync
        while (videoTimePassed_ > timePerFrame)
            videoTimePassed_ -= timePerFrame;

        // decode video!
        ogg_packet packet;
        f32 enterTime = videoLastTime_;
        while (ogg_stream_packetout(&theoraStream_->State, &packet))
        {
            if (packet.granulepos > 0)
                videoLastTime_ = (f32)th_granule_time(thDec_, packet.granulepos);

            // decode the packet
            s32 ret = th_decode_packetin(thDec_, &packet, &packet.granulepos);
            ret;

            // always display keyframes
            if (th_packet_iskeyframe(&packet))
                return;

            // if over we play at fps rate.. so just return
            if (over)
                return;

            // if there is no frame skipping going on continue
            if (videoLastTime_ - enterTime < timePerFrame)
                return;
            // otherwise, we stall til we catch up
            else if (videoLastTime_ >= position)
                return;
        }

        if (theoraStream_->EOS)
        {
            packetsRemaining_ = false;
        }

        // out of packets try to get another page
        OggStream* stream = 0;
        while (loadPage_(stream) && stream != theoraStream_);
    }

    void VideoClip::processAudioStream_()
    {
        // if i havent exceeded the last audio time, just skip
        if (TimeElapsed() < audioLastTime_)
            return;

        if (vorbisStream_->EOS)
            BASS_StreamPutData(audioStream_, 0, BASS_STREAMPROC_END);

        // do i have any packets remaining?
        ogg_packet packet;
        while (ogg_stream_packetout(&vorbisStream_->State, &packet))
        {
            // update audio last time
            if (packet.granulepos > 0)
            {
                audioLastTime_ = (f32)vorbis_granule_time(&vState_, packet.granulepos);
                audioLastTime_ -= AUDIO_BUFFER_TIME; // add some buffer time
            }

            s32 ret = vorbis_synthesis(&vBlock_, &packet);
            if (ret == 0)
            {
                ret = vorbis_synthesis_blockin(&vState_, &vBlock_);
                SAssert(ret == 0, "Failed to retrieve audio block.");
            }

            // while there are samples write them to the playback stream
            f32** pcm = 0;
            s32 samples = 0;
            if ((samples = vorbis_synthesis_pcmout(&vState_, &pcm)) > 0)
            {
                f32 buffer[BUFFER_SIZE];
                u32 c = 0;
                for (s32 i = 0; i < samples; ++i)
                {
                    for (s32 j = 0; j < vInfo_.channels; ++j)
                    {
                        buffer[c++] = pcm[j][i];
                        
                        // flush buffer when full
                        if (c == BUFFER_SIZE)
                        {
                            BASS_StreamPutData(audioStream_, buffer, c * sizeof(f32));
                            c = 0;
                        }
                    }
                }

                // put leftover data into stream
                BASS_StreamPutData(audioStream_, buffer, c * sizeof(f32));

                // update samples read
                ret = vorbis_synthesis_read(&vState_, samples);
                SAssert(ret == 0, "Failed to read samples.");

                if (TimeElapsed() < audioLastTime_)
                    return;
            }

            ret; // beats the unused warning in release mode.
        }

        // out of packets try to get another page
        OggStream* stream;
        while (loadPage_(stream) && stream != vorbisStream_);
    }

    //--------------------------------------------------------------------------

    bool VideoClip::Load( const std::string& file )
    {
        // open the file in binary mode
        file_.open(file, std::ios::binary);

        if (!file_)
        {
            SError("Video clip file: " << file << " does not exist.");
            Unload();
            return false;
        }

        if (!loadStreams_())
        {
            file_.close();
            return false;
        }

        // everything loaded fine..
        loaded_ = true;
        return true;
    }

    void VideoClip::Unload()
    {
        unloadStreams_();

        // unload file and sync state
        if (loaded_)
        {
            loaded_ = false;
            file_.close();
            ogg_sync_clear(&oggState_);
        }
    }


    void VideoClip::Play()
    {
        if (State() == PS_PLAYING)
            return;
        State(PS_PLAYING);
        play_();
    }

    void VideoClip::Pause()
    {
        if (State() != PS_PLAYING)
            return;
        State(PS_PAUSED);
        pause_();
    }

    void VideoClip::Stop()
    {
        if (State() == PS_STOPPED)
            return;
        State(PS_STOPPED);
        stop_();
    }

    void VideoClip::Seek( f32 normalizedTime )
    {
        seek_(normalizedTime);
    }

    void VideoClip::Update( f32 dt )
    {
        if (State() == PS_PLAYING)
            timeElapsed_ += dt;
        update_(dt);
    }

    bool VideoClip::ExtractFrame( u8** y, u32& ystride, u8** u, 
                                  u32& uStride, u8** v, u32& vStride )
    {
        if (videoLastTime_ == 0.0f)
            return false; // bail if the extraction failed

        th_ycbcr_buffer buffer;
        th_decode_ycbcr_out(thDec_, buffer);

        // point the frame to the buffer...
        *y = buffer[0].data;
        *u = buffer[1].data;
        *v = buffer[2].data;

        ystride = buffer[0].stride;
        uStride = buffer[1].stride;
        vStride = buffer[2].stride;

        return true;

        // TODO: use this code as a fallback
        /*u32 frameStride = thInfo_.frame_width * 4;

        u8* row1 = frame;
        u8* row2 = frame + frameStride;

        for (u32 y = 0; y < thInfo_.frame_height/2; ++y)
        {
            for (u32 x = 0; x < thInfo_.frame_width; ++x)
            {
#define GET_R(Y, V) (u8)Simian::Math::Clamp((int)(Y + 1.402f * (V - 128)), 0, 255)
#define GET_G(Y, U, V) (u8)Simian::Math::Clamp((int)(Y - 0.34414f * (U - 128) - 0.71414f * (V-128)), 0, 255)
#define GET_B(Y, U) (u8)Simian::Math::Clamp((int)(Y + 1.772f * (U - 128)), 0, 255)

                u8 y1 = buffer[0].data[y * 2 * buffer[0].stride + x];
                u8 y2 = buffer[0].data[(y + 1) * 2 * buffer[0].stride + x];
                u8 u = buffer[1].data[y * buffer[1].stride + x/2];
                u8 v = buffer[2].data[y * buffer[2].stride + x/2];

                *(row1++) = GET_B(y1, u);
                *(row1++) = GET_G(y1, u, v);
                *(row1++) = GET_R(y1, v);
                row1++;

                *(row2++) = GET_B(y2, u);
                *(row2++) = GET_G(y2, u, v);
                *(row2++) = GET_R(y2, v);
                row2++;
#undef GET_R
#undef GET_G
#undef GET_B
            }

            // increment both rows by stride (skips a row)
            row1 += frameStride;
            row2 += frameStride;
        }*/
    }
}

#endif
