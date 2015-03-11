/************************************************************************/
/*!
\file		BitmapText.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/BitmapText.h"
#include "Simian/BitmapFont.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/IndexBuffer.h"

#include <algorithm>

namespace Simian
{
    BitmapText::BitmapText( BitmapFont* parent, u32 maxCharacters )
        : parentFont_(parent),
          opacity_(1.0f),
          dirty_(false)
    {
        vertices_.resize(parent->NumPages());

        std::vector<u16> ibData(maxCharacters * 6);
        for (u16 i = 0; i < maxCharacters; ++i)
        {
            ibData[i * 6 + 0] = i * 4 + 0;
            ibData[i * 6 + 1] = i * 4 + 1;
            ibData[i * 6 + 2] = i * 4 + 2;
            ibData[i * 6 + 3] = i * 4 + 2;
            ibData[i * 6 + 4] = i * 4 + 3;
            ibData[i * 6 + 5] = i * 4 + 0;
        }

        // create vertex buffer for each page
        for (u32 i = 0; i < parent->NumPages(); ++i)
        {
            Page page;

            IndexBuffer* ib;
            parentFont_->Device().CreateIndexBuffer(ib);
            ib->Load(Simian::IndexBuffer::IBT_U16, maxCharacters * 6, false);
            ib->Data(&ibData[0]);
            page.IndexBuffer = ib;

            VertexBuffer* vb;
            parentFont_->Device().CreateVertexBuffer(vb);
            vb->Load(Simian::PositionColorUVVertex::Format, maxCharacters * 4, true);
            page.VertexBuffer = vb;

            vertices_[i].reserve(maxCharacters * 4);
            page.NumCharacters = 0;

            pages_.push_back(page);
        }
    }

    BitmapText::~BitmapText()
    {
        for (u32 i = 0; i < pages_.size(); ++i)
        {
            pages_[i].VertexBuffer->Unload();
            parentFont_->Device().DestroyVertexBuffer(pages_[i].VertexBuffer);

            pages_[i].IndexBuffer->Unload();
            parentFont_->Device().DestroyIndexBuffer(pages_[i].IndexBuffer);
        }
        pages_.clear();
    }

    //--------------------------------------------------------------------------

    const std::string& BitmapText::Text() const
    {
        return text_;
    }

    void BitmapText::Text( const std::string& val )
    {
        text_ = val;
        dirty_ = true;
    }

    const Simian::Vector2& BitmapText::Alignment() const
    {
        return alignment_;
    }

    void BitmapText::Alignment( const Simian::Vector2& val )
    {
        alignment_ = val;
        dirty_ = true;
    }

    float BitmapText::Opacity() const
    {
        return opacity_;
    }

    void BitmapText::Opacity( float val )
    {
        opacity_ = val;
        dirty_ = true;
    }

    //--------------------------------------------------------------------------

    void BitmapText::updateVertexBuffer_()
    {
        // reject if there is no text
        if (!text_.length())
            return;

        // calculate where to start drawing each vertex

        f32 width = 0, height = 0;
        f32 curWidth = 0;

        // iterate characters and find the width and height
        for (u32 i = 0; i < text_.length(); ++i)
        {
            // add height for newlines
            if (text_[i] == '\n')
            {
                width = curWidth > width ? curWidth : width;
                ++height;
            }
            else
                // get the xadvance for the character
                curWidth += parentFont_->Glyphs()[text_[i]].XAdvance;
        }

        // take curwidth if no new lines were found
        width = !width ? curWidth : width;
        height = height * parentFont_->LineHeight() + parentFont_->LineHeight();

        // determine start x and y
        f32 startX = width * alignment_.X();
        f32 startY = height * alignment_.Y() + parentFont_->LineHeight();

        // clear out vertices (no worries reserve should keep the memory fresh)
        for (u32 i = 0; i < parentFont_->NumPages(); ++i)
        {
            vertices_[i].clear();
            pages_[i].NumCharacters = 0;
        }

        f32 curX = startX;

        std::vector<ColorPoint>::iterator colorIter = colors_.begin();
        u32 nextColor = colorIter != colors_.end() ? (*colorIter).Position : -1; // max char, we'll never reach it
        Simian::Color currentColor(1.0f, 1.0f, 1.0f, opacity_);

        // start writing to a vector to stuff into the vertex buffer
        for (u32 i = 0; i < text_.length(); ++i)
        {
            if (colorIter != colors_.end() && i == nextColor)
            {
                currentColor = (*colorIter).Color;
                currentColor.A(opacity_);
                ++colorIter;
                nextColor = colorIter != colors_.end() ? (*colorIter).Position : nextColor;
            }

            if (text_[i] == '\n')
            {
                curX = startX;
                startY -= parentFont_->LineHeight();
                continue; 
            }

            const BitmapFont::Glyph& glyph = parentFont_->Glyphs()[text_[i]];

            // increment the number of characters
            ++pages_[glyph.Page].NumCharacters;

            f32 x = curX + glyph.Offset.X();
            f32 y = startY - glyph.Offset.Y();
            f32 width = glyph.Size.X()/parentFont_->Size().X();
            f32 height = glyph.Size.Y()/parentFont_->Size().Y();

            // push 4 vertices
            vertices_[glyph.Page].push_back(PositionColorUVVertex(
                Vector3(x, y, 0), currentColor,
                glyph.UV));
            vertices_[glyph.Page].push_back(PositionColorUVVertex(
                Vector3(x + glyph.Size.X(), y, 0), currentColor,
                glyph.UV + Vector2(width, 0)));
            vertices_[glyph.Page].push_back(PositionColorUVVertex(
                Vector3(x + glyph.Size.X(), y - glyph.Size.Y(), 0), currentColor,
                glyph.UV + Vector2(width, height)));
            vertices_[glyph.Page].push_back(PositionColorUVVertex(
                Vector3(x, y - glyph.Size.Y(), 0), currentColor,
                glyph.UV + Vector2(0, height)));

            curX += glyph.XAdvance;
        }

        // update the vertex buffers
        for (u32 i = 0; i < pages_.size(); ++i)
            pages_[i].VertexBuffer->Data(&vertices_[i][0], pages_[i].NumCharacters * 4 * PositionColorUVVertex::Format.Size());
    }

    //--------------------------------------------------------------------------

    void BitmapText::Draw(u32 pageId)
    {
        if (dirty_)
            updateVertexBuffer_();
        dirty_ = false;

        if (pages_[pageId].NumCharacters)
            parentFont_->Device().DrawIndexed(
                pages_[pageId].VertexBuffer, pages_[pageId].IndexBuffer, 
                GraphicsDevice::PT_TRIANGLES, pages_[pageId].NumCharacters * 2);
    }

    void BitmapText::AddColor( u32 position, const Simian::Color& color )
    {
        ColorPoint cp;
        cp.Position = position;
        cp.Color = color;
        colors_.push_back(cp);
        std::sort(colors_.begin(), colors_.end());
    }

    void BitmapText::ClearColors()
    {
        colors_.clear();
    }
}
