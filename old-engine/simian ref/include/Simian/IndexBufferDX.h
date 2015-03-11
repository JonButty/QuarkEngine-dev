/************************************************************************/
/*!
\file		IndexBufferDX.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_INDEXBUFFERDX_H_
#define SIMIAN_INDEXBUFFERDX_H_

#include "SimianPlatform.h"
#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX

#include "IndexBuffer.h"

#include <d3d9.h>

namespace Simian
{
    class IndexBuffer: public IndexBufferBase
    {
    private:
        LPDIRECT3DINDEXBUFFER9 indexBuffer_;
    private:
        void update_( u32 offset, u32 size );
        bool load_();
        void unload_();

        void onLostDevice_();
        void onResetDevice_();
    public:
        IndexBuffer(GraphicsDevice* device);

        void Set();
        void Unset();

        friend class GraphicsDevice;
    };
}

#endif

#endif
