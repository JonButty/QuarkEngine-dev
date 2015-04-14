/************************************************************************/
/*!
\file		BitmapFont.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/BitmapFont.h"
#include "Simian/LogManager.h"
#include "Simian/BitmapText.h"

#include <cstdlib>
#include <sstream>

namespace Simian
{
    static const u32 GLYPHS_TOTAL = 256;

    BitmapFont::BitmapFont(GraphicsDevice* device)
        : device_(device),
          lineHeight_(0),
          numPages_(0)
    {
        Glyph nullGlyph = 
        { 
            Simian::Vector2::Zero, 
            Simian::Vector2::Zero, 
            Simian::Vector2::Zero,
            0.0f, 0
        };
        glyphs_.resize(GLYPHS_TOTAL, nullGlyph);
    }

    //--------------------------------------------------------------------------

    GraphicsDevice& BitmapFont::Device() const
    {
        return *device_;
    }

    Simian::u32 BitmapFont::NumPages() const
    {
        return numPages_;
    }

    const std::vector<BitmapFont::Glyph>& BitmapFont::Glyphs() const
    {
        return glyphs_;
    }

    //--------------------------------------------------------------------------

    bool BitmapFont::loadCommon_(std::stringstream& ss)
    {
        // parse keys
        while (ss)
        {
            std::string keyValuePair;
            ss >> keyValuePair;
            if (!ss)
                continue;
            u32 equalPos = keyValuePair.find_first_of('=');
            if (equalPos == std::string::npos)
            {
                SWarn("Invalid key/value pair in BitmapFont.");
                continue;
            }
            std::string key = keyValuePair.substr(0, equalPos);
            std::string value = keyValuePair.substr(equalPos + 1);

            if (key == "lineHeight")
                lineHeight_ = static_cast<f32>(std::atoi(value.c_str()));
            else if (key == "pages")
                numPages_ = std::atoi(value.c_str());
            else if (key == "scaleW")
                size_.X(static_cast<f32>(std::atoi(value.c_str())));
            else if (key == "scaleH")
                size_.Y(static_cast<f32>(std::atoi(value.c_str())));
        }
        return true;
    }

    bool BitmapFont::loadGlyph_( std::stringstream& ss )
    {
        Glyph glyph;
        s32 index = -1;
        while (ss)
        {
            std::string keyValuePair;
            ss >> keyValuePair;
            if (!ss)
                continue;
            u32 equalPos = keyValuePair.find_first_of('=');
            if (equalPos == std::string::npos)
            {
                SWarn("Invalid key/value pair in BitmapFont.");
                continue;
            }
            std::string key = keyValuePair.substr(0, equalPos);
            std::string value = keyValuePair.substr(equalPos + 1);

            if (key == "id")
                index = std::atoi(value.c_str());
            else if (key == "x")
                glyph.UV.X(static_cast<f32>(std::atoi(value.c_str()))/size_.X());
            else if (key == "y")
                glyph.UV.Y(static_cast<f32>(std::atoi(value.c_str()))/size_.Y());
            else if (key == "width")
                glyph.Size.X(static_cast<f32>(std::atoi(value.c_str())));
            else if (key == "height")
                glyph.Size.Y(static_cast<f32>(std::atoi(value.c_str())));
            else if (key == "xoffset")
                glyph.Offset.X(static_cast<f32>(std::atoi(value.c_str())));
            else if (key == "yoffset")
                glyph.Offset.Y(static_cast<f32>(std::atoi(value.c_str())));
            else if (key == "xadvance")
                glyph.XAdvance = static_cast<f32>(std::atoi(value.c_str()));
            else if (key == "page")
                glyph.Page = std::atoi(value.c_str());
        }

        if (index >= 0)
            glyphs_[index] = glyph;

        return index != -1;
    }

    Simian::f32 BitmapFont::LineHeight() const
    {
        return lineHeight_;
    }

    const Simian::Vector2& BitmapFont::Size() const
    {
        return size_;
    }

    //--------------------------------------------------------------------------

    bool BitmapFont::LoadImmediate( const DataLocation& data )
    {
        return LoadFile(data);
    }

    bool BitmapFont::LoadFile( const DataLocation& data )
    {
        if (!data)
        {
            SWarn(std::string("Bitmap font could not be loaded: ") + data.Identifier());
            return false;
        }

        std::stringstream ss;
        ss << std::string(data.Memory(), data.Memory() + data.Size());

        // load tags
        std::string line;
        while (std::getline(ss, line))
        {
            // parse the line
            std::stringstream ss;
            ss << line;

            std::string key;
            ss >> key;

            if (key == "common")
            {
                // load common
                if (!loadCommon_(ss))
                    return false;
            }
            else if (key == "char")
            {
                // load glyph
                if (!loadGlyph_(ss))
                {
                    SWarn("Glyph failed to load in BitmapFont.");
                    continue;
                }
            }
        }

        return true;
    }

    void BitmapFont::Unload()
    {
        glyphs_.clear();
    }

    void BitmapFont::Draw( BitmapText* text )
    {
        for (u32 i = 0; i < numPages_; ++i)
            text->Draw(i);
    }

    void BitmapFont::Draw( BitmapText* text, u32 pageId )
    {
        text->Draw(pageId);
    }

    bool BitmapFont::CreateBitmapText( BitmapText*& text, u32 maxCharacters )
    {
        text = new BitmapText(this, maxCharacters);
        return text != NULL;
    }

    void BitmapFont::DestroyBitmapText( BitmapText*& text )
    {
        delete text;
        text = 0;
    }
}
