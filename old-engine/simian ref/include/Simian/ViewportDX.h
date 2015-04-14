/************************************************************************/
/*!
\file		ViewportDX.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_VIEWPORTDX_H_
#define SIMIAN_VIEWPORTDX_H_

#include "SimianPlatform.h"
#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX

#include "Viewport.h"
#include <d3d9.h>

namespace Simian
{
    class SIMIAN_EXPORT Viewport: public ViewportBase
    {
    private:
        D3DVIEWPORT9 viewport_;
    public:
        const Simian::Vector2& Position() const;
        void Position(const Simian::Vector2& val);

        const Simian::Vector2& Size() const;
        void Size(const Simian::Vector2& val);

        const Simian::Vector2& Depth() const;
        void Depth(const Simian::Vector2& val);
    public:
        Viewport(GraphicsDevice* device);

        bool Load();
        void Unload();

        void Set();
        void Unset();
    };
}

#endif

#endif
