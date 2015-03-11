/************************************************************************/
/*!
\file		InstrumentedProfiler.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/SimianPlatform.h"
#if SIMIAN_ATTACHPROFILER == 1

#include "Simian/InstrumentedProfiler.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/IndexBuffer.h"
#include "Simian/BitmapFont.h"
#include "Simian/BitmapText.h"
#include "Simian/Material.h"
#include "Simian/MaterialCache.h"
#include "Simian/Keyboard.h"

namespace Simian
{
    static const f32 Y_OFFSET = 50.0f;
    static const f32 BAR_WIDTH = 220.0f;
    static const f32 BAR_HEIGHT = 20.0f;
    static const f32 BAR_PADDING = 5.0f;

    InstrumentedProfiler::InstrumentedProfiler()
        : vertexBuffer_(0),
          indexBuffer_(0),
          profilerFontMat_(0),
          profilerFont_(0),
          longestWatch_(1), 
          frameTime_(0.0f),
          device_(0),
          hidden_(true)
    {
    }

    //--------------------------------------------------------------------------

    void InstrumentedProfiler::Init( u32 totalWatches )
    {
        if (totalWatches > watchTimes_.size())
        {
            watchTimes_.resize(totalWatches, WatchInfo(this));

            // set up index correctly
            for (u32 i = 0; i < watchTimes_.size(); ++i)
                watchTimes_[i].Index(i);
        }
    }

    void InstrumentedProfiler::Load(GraphicsDevice* device, MaterialCache* materialCache)
    {
        device_ = device;

        u32 numVerts =  watchTimes_.size() * 4;
        device->CreateVertexBuffer(vertexBuffer_);
        vertexBuffer_->Load(PositionColorVertex::Format, numVerts, true);
        vertices_.resize(numVerts);

        u32 numIndices = watchTimes_.size() * 6;
        device_->CreateIndexBuffer(indexBuffer_);
        indexBuffer_->Load(IndexBuffer::IBT_U16, numIndices);

        // indices can be precomputed
        std::vector<u16> indices;
        indices.reserve(numIndices);
        for (u16 i = 0; i < watchTimes_.size(); ++i)
        {
            indices.push_back(i * 4 + 0);
            indices.push_back(i * 4 + 1);
            indices.push_back(i * 4 + 2);
            indices.push_back(i * 4 + 2);
            indices.push_back(i * 4 + 3);
            indices.push_back(i * 4 + 0);
        }

        indexBuffer_->Data(&indices[0]);

        profilerFontMat_ = materialCache->Load("Materials/inconsolata.mat");

        profilerFont_ = new BitmapFont(device);
        profilerFont_->LoadImmediate("Fonts/inconsolata.fnt");

        for (u32 i = 0; i < watchTimes_.size(); ++i)
            watchTimes_[i].Load();
    }

    void InstrumentedProfiler::Unload()
    {
        for (u32 i = 0; i < watchTimes_.size(); ++i)
            watchTimes_[i].Unload();

        profilerFont_->Unload();
        delete profilerFont_;

        indexBuffer_->Unload();
        device_->DestroyIndexBuffer(indexBuffer_);

        vertexBuffer_->Unload();
        device_->DestroyVertexBuffer(vertexBuffer_);
    }

    void InstrumentedProfiler::UpdateWatch(u32 index, const std::string& label, u64 time, const Simian::Color& color)
    {
        watchTimes_[index].Label(label);
        watchTimes_[index].Time(time);
        watchTimes_[index].Color(color);
        longestWatch_ = time > longestWatch_ ? time : longestWatch_;
    }

    void InstrumentedProfiler::Update( Simian::f32 dt )
    {
        if (Keyboard::IsTriggered(Keyboard::KEY_F12))
            hidden_ = !hidden_;

        if (hidden_)
            return;

        frameTime_ = dt;

        // set up vertex buffers for rendering
        for (u32 i = 0; i < watchTimes_.size(); ++i)
            watchTimes_[i].Update();

        // update vertex buffer
        vertexBuffer_->Data(&vertices_[0]);

        longestWatch_ = 0;
    }

    void InstrumentedProfiler::Draw()
    {
        if (hidden_)
            return;

        // draw vertex buffers, text, whatever
        Simian::Vector2 hSize = device_->Size() * 0.5f;
        Simian::Matrix world = Matrix::Translation(-hSize.X(), hSize.Y() - Y_OFFSET, 0);

        device_->World(world);
        device_->DrawIndexed(vertexBuffer_, indexBuffer_, 
            GraphicsDevice::PT_TRIANGLES, watchTimes_.size() * 2);

        world = Matrix::Translation(BAR_PADDING * 2, -BAR_HEIGHT * 0.5f, 0) * world;

        profilerFontMat_->Set();
        for (u32 m = 0; m < profilerFontMat_->Size(); ++m)
        {
            (*profilerFontMat_)[m].Set();

            for (u32 i = 0; i < watchTimes_.size(); ++i)
            {
                device_->World(world);
                watchTimes_[i].Draw();
                world = Matrix::Translation(0.0f, -BAR_HEIGHT - BAR_PADDING, 0) * world;
            }

            (*profilerFontMat_)[m].Unset();
        }
        profilerFontMat_->Unset();
    }


    //--------------------------------------------------------------------------

    InstrumentedProfiler::WatchInfo::WatchInfo(InstrumentedProfiler* parent)
        : parent_(parent),
          index_(0),
          time_(0)
    {
    }

    //--------------------------------------------------------------------------

    void InstrumentedProfiler::WatchInfo::Index( Simian::u32 val )
    {
        index_ = val;
    }

    void InstrumentedProfiler::WatchInfo::Label( const std::string& val )
    {
        label_ = val;
    }

    void InstrumentedProfiler::WatchInfo::Time( Simian::u64 val )
    {
        time_ = val;
    }

    void InstrumentedProfiler::WatchInfo::Color( const Simian::Color& val )
    {
        color_ = val;
    }

    //--------------------------------------------------------------------------

    void InstrumentedProfiler::WatchInfo::Load()
    {
        parent_->profilerFont_->CreateBitmapText(text_, 64);
    }

    void InstrumentedProfiler::WatchInfo::Unload()
    {
        parent_->profilerFont_->DestroyBitmapText(text_);
    }

    void InstrumentedProfiler::WatchInfo::Update()
    {
        s8 msg[64];
        f32 perc = (f32)time_/parent_->longestWatch_;
        sprintf(msg, "%15s: %f", label_.c_str(), perc);
        text_->Alignment(Simian::Vector2(0.0f, -0.5f));
        text_->Text(msg);

        // update the bar
        f32 x = BAR_PADDING;
        f32 y = index_ * -(BAR_HEIGHT + BAR_PADDING);
        f32 width = (BAR_WIDTH * time_)/parent_->longestWatch_;

        u32 startIndex = index_ * 4;
        parent_->vertices_[startIndex++] = 
            PositionColorVertex(Vector3(x, y, 0), color_);
        parent_->vertices_[startIndex++] = 
            PositionColorVertex(Vector3(x, y - BAR_HEIGHT, 0), color_);
        parent_->vertices_[startIndex++] = 
            PositionColorVertex(Vector3(x + width, y - BAR_HEIGHT, 0), color_);
        parent_->vertices_[startIndex++] = 
            PositionColorVertex(Vector3(x + width, y, 0), color_);
    }

    void InstrumentedProfiler::WatchInfo::Draw()
    {
        text_->Draw(0);
    }
}

#endif
