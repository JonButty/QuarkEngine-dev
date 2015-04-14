/************************************************************************/
/*!
\file		ViewportDX.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/SimianPlatform.h"
#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX

#include "Simian/ViewportDX.h"
#include "Simian/GraphicsDevice.h"

namespace Simian
{
    Viewport::Viewport( GraphicsDevice* device )
        : ViewportBase(device)
    {
    }

    //--------------------------------------------------------------------------

    const Simian::Vector2& Viewport::Position() const
    {
        return ViewportBase::Position();
    }

    void Viewport::Position( const Simian::Vector2& val )
    {
        ViewportBase::Position(val);
        viewport_.X = static_cast<DWORD>(val.X());
        viewport_.Y = static_cast<DWORD>(val.Y());
    }

    const Simian::Vector2& Viewport::Size() const
    {
        return ViewportBase::Size();
    }

    void Viewport::Size( const Simian::Vector2& val )
    {
        ViewportBase::Size(val);
        viewport_.Width = static_cast<DWORD>(val.X());
        viewport_.Height = static_cast<DWORD>(val.Y());
    }

    const Simian::Vector2& Viewport::Depth() const
    {
        return ViewportBase::Depth();
    }

    void Viewport::Depth( const Simian::Vector2& val )
    {
        ViewportBase::Depth(val);
        viewport_.MinZ = val.X();
        viewport_.MaxZ = val.Y();
    }


    //--------------------------------------------------------------------------

    bool Viewport::Load()
    {
        ZeroMemory(&viewport_, sizeof(D3DVIEWPORT9));
        return true;
    }

    void Viewport::Unload()
    {
    }

    void Viewport::Set()
    {
        Device().D3DDevice()->SetViewport(&viewport_);
    }

    void Viewport::Unset()
    {
    }
}

#endif
