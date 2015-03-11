/************************************************************************/
/*!
\file		VertexBufferDX.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_VERTEXBUFFERDX_H_
#define SIMIAN_VERTEXBUFFERDX_H_

#include "SimianPlatform.h"
#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX

#include "VertexBuffer.h"

#include <d3d9.h>

namespace Simian
{
    class SIMIAN_EXPORT VertexBuffer: public VertexBufferBase
    {
    private:
        LPDIRECT3DVERTEXDECLARATION9 vertexDeclaration_;
        LPDIRECT3DVERTEXBUFFER9 vertexBuffer_;
    private:
        void update_(u32 offset, u32 size);
        bool load_();
        void unload_();

        bool loadVertexBuffer_();
        void unloadVertexBuffer_();

        void onLostDevice_();
        void onResetDevice_();
    public:
        VertexBuffer(GraphicsDevice* device);

        void Set(u32 offset = 0);
        void Unset();

        friend class GraphicsDevice;
    };
}

#endif

#endif
