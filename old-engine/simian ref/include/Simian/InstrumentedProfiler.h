/************************************************************************/
/*!
\file		InstrumentedProfiler.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_INSTRUMENTEDPROFILER_H_
#define SIMIAN_INSTRUMENTEDPROFILER_H_

#include "SimianPlatform.h"

#if SIMIAN_ATTACHPROFILER == 1

#include "Singleton.h"
#include "ProfilerWatch.h"
#include "VertexBuffer.h"

#include <vector>
#include <string>

namespace Simian
{
    class MaterialCache;
    class Material;
    class BitmapFont;
    class BitmapText;
    class IndexBuffer;

    class SIMIAN_EXPORT InstrumentedProfiler: public Singleton<InstrumentedProfiler>
    {
    private:
        class WatchInfo
        {
        private:
            InstrumentedProfiler* parent_;
            u32 index_;
            std::string label_;
            u64 time_;
            BitmapText* text_;
            Simian::Color color_;
        public:
            void Index(Simian::u32 val);
            void Label(const std::string& val);
            void Time(Simian::u64 val);
            void Color(const Simian::Color& val);
        public:
            WatchInfo(InstrumentedProfiler* parent);
            void Load();
            void Unload();
            void Update();
            void Draw();
        };
    private:
        std::vector<PositionColorVertex> vertices_;
        VertexBuffer* vertexBuffer_;
        IndexBuffer* indexBuffer_;
        Material* profilerFontMat_;
        BitmapFont* profilerFont_;

        std::vector<WatchInfo> watchTimes_;
        u64 longestWatch_;
        f32 frameTime_;
        GraphicsDevice* device_;

        bool hidden_;
    public:
        InstrumentedProfiler();

        void Init(u32 totalWatches);
        void Load(GraphicsDevice* device, MaterialCache* materialCache);
        void Unload();
        void UpdateWatch(u32 index, const std::string& label, u64 time, const Simian::Color& color);
        void Update(Simian::f32 dt);
        void Draw();

        friend class WatchInfo;
    };

    SIMIAN_IMPORT template class SIMIAN_EXPORT Singleton<InstrumentedProfiler>;
}

#define SProfilerInit(total) Simian::InstrumentedProfiler::Instance().Init(total)
#define SProfilerLoad(device, matCache) Simian::InstrumentedProfiler::Instance().Load(device, matCache)
#define SProfilerUnload() Simian::InstrumentedProfiler::Instance().Unload()
#define SProfilerUpdate(dt) Simian::InstrumentedProfiler::Instance().Update(dt)
#define SProfilerDraw() Simian::InstrumentedProfiler::Instance().Draw()
#define SProfilerWatch(id, color) Simian::ProfilerWatch profiler_watch_(id, #id, color)

#else

#define SProfilerInit(total)
#define SProfilerLoad(device, matCache)
#define SProfilerUnload()
#define SProfilerUpdate(dt)
#define SProfilerDraw()
#define SProfilerWatch(id, color)

#endif

#endif
