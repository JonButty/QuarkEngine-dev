/************************************************************************/
/*!
\file		BitmapFont.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_BITMAPFONT_H_
#define SIMIAN_BITMAPFONT_H_

#include "SimianPlatform.h"
#include "DataLocation.h"
#include "Vector2.h"

#include <vector>

namespace Simian
{
    class GraphicsDevice;
    class BitmapText;

    // this loads the bitmap font file and contains the texture coordinates
    // for the alphabets. It also instantiates bitmap texts.
    class SIMIAN_EXPORT BitmapFont
    {
    public:
        struct SIMIAN_EXPORT Glyph
        {
            Vector2 UV;
            Vector2 Size;
            Vector2 Offset;
            f32 XAdvance;
            u32 Page;
        };
    private:
        GraphicsDevice* device_;
        f32 lineHeight_;
        Vector2 size_;
        u32 numPages_;
        std::vector<Glyph> glyphs_;
    public:
        GraphicsDevice& Device() const;
        Simian::u32 NumPages() const;
        const std::vector<Glyph>& Glyphs() const;
        Simian::f32 LineHeight() const;
        const Simian::Vector2& Size() const;
    private:
        bool loadCommon_(std::stringstream& ss);
        bool loadGlyph_(std::stringstream& ss);
    public:
        BitmapFont(GraphicsDevice* device);

        bool LoadImmediate(const DataLocation& data);
        bool LoadFile(const DataLocation& data);

        void Unload();

        void Draw(BitmapText* text);
        void Draw(BitmapText* text, u32 pageId);

        bool CreateBitmapText(BitmapText*& text, u32 maxCharacters = 1024);
        void DestroyBitmapText(BitmapText*& text);
    };
}

#endif
