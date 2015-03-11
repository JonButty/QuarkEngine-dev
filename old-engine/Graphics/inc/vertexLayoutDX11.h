#ifndef VERTEXLAYOUTDX11_H
#define VERTEXLAYOUTDX11_H

#include <d3d11.h>

namespace GFX
{
    static const D3D11_INPUT_ELEMENT_DESC VertexLayout[] =
    {
        { "POSITION",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",       0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    static const unsigned int VertexLayoutElementCount = sizeof(VertexLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC);
}

#endif