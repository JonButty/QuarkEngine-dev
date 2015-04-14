/************************************************************************/
/*!
\file		Viewport.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_VIEWPORT_H_
#define SIMIAN_VIEWPORT_H_

#include "SimianPlatform.h"
#include "Vector2.h"

namespace Simian
{
    class GraphicsDevice;

    class SIMIAN_EXPORT ViewportBase
    {
    private:
        GraphicsDevice* device_;
        Simian::Vector2 position_;
        Simian::Vector2 size_;
        Simian::Vector2 depth_;
    public:
        GraphicsDevice& Device() const { return *device_; }

        const Simian::Vector2& Position() const { return position_; }
        void Position(const Simian::Vector2& val) { position_ = val; }

        const Simian::Vector2& Size() const { return size_; }
        void Size(const Simian::Vector2& val) { size_ = val; }
        
        const Simian::Vector2& Depth() const { return depth_; }
        void Depth(const Simian::Vector2& val) { depth_ = val; }
    public:
        ViewportBase(GraphicsDevice* device)
            : device_(device) {}
        virtual ~ViewportBase() {}

        S_NIMPL bool Load() { return false; }
        S_NIMPL void Unload() {}

        S_NIMPL void Set() {}
        S_NIMPL void Unset() {}
    };
}

#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX
#include "ViewportDX.h"
#elif SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_GL
#include "ViewportGL.h"
#endif

#endif
