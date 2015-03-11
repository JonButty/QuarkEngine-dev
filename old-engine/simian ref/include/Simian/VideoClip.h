/************************************************************************/
/*!
\file		VideoClip.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_VIDEOCLIP_H_
#define SIMIAN_VIDEOCLIP_H_

#include "SimianPlatform.h"

#include <string>

namespace Simian
{
    class SIMIAN_EXPORT VideoClipBase
    {
    public:
        enum PlaybackState
        {
            PS_STOPPED,
            PS_PAUSED,
            PS_PLAYING
        };
    protected:
        f32 timeElapsed_;

        u32 width_;
        u32 height_;
        f32 fps_;

        PlaybackState state_;
    protected:
        void TimeElapsed(Simian::f32 val) { timeElapsed_ = val; }

        void Width(Simian::u32 val) { width_ = val; }
        void Height(Simian::u32 val) { height_ = val; }
        void Fps(Simian::f32 val) { fps_ = val; }

        void State(Simian::VideoClipBase::PlaybackState val) { state_ = val; }
    public:
        Simian::f32 TimeElapsed() const { return timeElapsed_; }

        Simian::u32 Width() const { return width_; }
        Simian::u32 Height() const { return height_; }
        Simian::f32 Fps() const { return fps_; }

        Simian::VideoClipBase::PlaybackState State() const { return state_; }
    public:
        VideoClipBase()
            : timeElapsed_(0),
              width_(0),
              height_(0),
              fps_(0)
        {
        }

        virtual ~VideoClipBase()
        {
        }

        S_NIMPL bool Load(const std::string& file) { file; return false; }
        S_NIMPL void Unload() {}

        S_NIMPL void Play() {}
        S_NIMPL void Pause() {}
        S_NIMPL void Stop() {}
        S_NIMPL void Seek(f32 normalizedTime) { normalizedTime; }
        S_NIMPL void Update(f32 dt) { dt; }

        S_NIMPL void ExtractFrame(u8* frame) { frame; }
    };
}

#if SIMIAN_VIDEO == SIMIAN_VIDEO_THEORA
#include "VideoClipTheora.h"
#endif

#endif
