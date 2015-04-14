/************************************************************************/
/*!
\file		BitmapText.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_BITMAPTEXT_H_
#define SIMIAN_BITMAPTEXT_H_

#include "SimianPlatform.h"
#include "Vector2.h"
#include "VertexBuffer.h"

#include <vector>
#include <string>

namespace Simian
{
    class BitmapFont;
    class IndexBuffer;
    class VertexBuffer;

    // this contains the actual vertexbuffers etc needed to draw the text
    class SIMIAN_EXPORT BitmapText
    {
    private:
        struct Page
        {
            IndexBuffer* IndexBuffer;
            VertexBuffer* VertexBuffer;
            u32 NumCharacters;
        };
        
        struct ColorPoint
        {
            u32 Position;
            Simian::Color Color;

            bool operator<(const ColorPoint& col) const
            {
                return Position < col.Position;
            }
        };
    private:
        BitmapFont* parentFont_;
        std::vector<Page> pages_;
        std::string text_;
        Vector2 alignment_;
        std::vector<std::vector<Simian::PositionColorUVVertex> > vertices_;
        std::vector<ColorPoint> colors_;
        f32 opacity_;
        bool dirty_;
    public:
        const std::string& Text() const;
        void Text(const std::string& val);

        const Simian::Vector2& Alignment() const;
        void Alignment(const Simian::Vector2& val);

        float Opacity() const;
        void Opacity(float val);
    private:
        void updateVertexBuffer_();
    public:
        BitmapText(BitmapFont* parent, u32 maximumCharacters);
        ~BitmapText();

        void Draw(u32 pageId);

        void AddColor(u32 position, const Simian::Color& color);
        void ClearColors();
    };
}

#endif
