/************************************************************************/
/*!
\file		GraphicsDeviceDX.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_GRAPHICSDEVICEDX_H_
#define SIMIAN_GRAPHICSDEVICEDX_H_

#include "SimianPlatform.h"

#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX
#include "GraphicsDevice.h"
#include "Color.h"
#include "Delegate.h"

#include <d3d9.h>

#include <vector>

namespace Simian
{
    class SIMIAN_EXPORT GraphicsDevice: public GraphicsDeviceBase
    {
    public:
        enum PrimitiveType
        {
            PT_POINT            = D3DPT_POINTLIST,
            PT_LINE             = D3DPT_LINELIST,
            PT_TRIANGLES        = D3DPT_TRIANGLELIST,
            PT_TRIANGLESTRIP    = D3DPT_TRIANGLESTRIP,
            PT_TRIANGLEFAN      = D3DPT_TRIANGLEFAN
        };

        enum RenderStateFlag
        {
            RS_LIGHTING         = D3DRS_LIGHTING,
            RS_CULLING          = D3DRS_CULLMODE,
            RS_BLENDING         = D3DRS_ALPHABLENDENABLE,
            RS_DEPTHWRITE       = D3DRS_ZWRITEENABLE,
            RS_DEPTHENABLED     = D3DRS_ZENABLE,
            RS_SRCBLEND         = D3DRS_SRCBLEND,
            RS_DSTBLEND         = D3DRS_DESTBLEND,
            RS_ALPHATEST        = D3DRS_ALPHATESTENABLE,
            RS_ALPHAREF         = D3DRS_ALPHAREF,
            RS_ALPHAFUNC        = D3DRS_ALPHAFUNC
        };

        enum CullMode
        {
            SCM_NONE    = D3DCULL_NONE,
            SCM_CW      = D3DCULL_CW,
            SCM_CCW     = D3DCULL_CCW
        };

        enum BlendMode
        {
            BM_ZERO         = D3DBLEND_ZERO,
            BM_ONE          = D3DBLEND_ONE,
            BM_SRCALPHA     = D3DBLEND_SRCALPHA,
            BM_INVSRCALPHA  = D3DBLEND_INVSRCALPHA
        };

        enum AlphaTestFunc
        {
            AF_ALWAYS   = D3DCMP_ALWAYS,
            AF_NEVER    = D3DCMP_NEVER,
            AF_GREATER  = D3DCMP_GREATER,
            AF_LESS     = D3DCMP_LESS
        };
    private:
        LPDIRECT3D9 direct3d_;
        LPDIRECT3DDEVICE9 device_;
        bool deviceLost_;

        std::vector<Texture*> textures_;
        std::vector<RenderTexture*> renderTextures_;
        std::vector<VertexBuffer*> vertexBuffers_;
        std::vector<IndexBuffer*> indexBuffers_;

        DelegateList releaseCallbacks_;
        DelegateList resetCallbacks_;
    public:
        S_NIMPL void RenderState(u32 state, u32 value);
        S_NIMPL u32 RenderState(u32 state);
        
        const Matrix& World() const;
        void World(const Simian::Matrix& world);

        const Matrix& View() const;
        void View(const Simian::Matrix& view);

        const Matrix& Projection() const;
        void Projection(const Simian::Matrix& projection);

        const Matrix& TextureMatrix(u32 index) const;
        void TextureMatrix(u32 index, const Matrix& val);

        void TextureState(u32 stage, u32 state, u32 value);
        u32 TextureState(u32 stage, u32 state);
    private:
        LPDIRECT3DDEVICE9 D3DDevice() const;
        void registerResource_(const Delegate& release, const Delegate& reset);
        void unregisterResource_(const Delegate& release,const Delegate& reset);
    private:
        bool init_();
        void deinit_();

        void setupPresentParameters_(D3DPRESENT_PARAMETERS& pp);
        void setupWindow_();
        HWND& hWnd_() const;

        void resetDevice_();
    public:
        GraphicsDevice();

        void EnumerateResolutions(std::vector<Vector2>& resolutions);

        void Begin();
        void Clear(const Simian::Color& color = Simian::Color(0.0f, 0.0f, 0.0f, 1.0f));
        void End();
        void Swap();

        bool ApplyChanges();

        bool CreateTexture(Texture*& texture);
        void DestroyTexture(Texture*& texture);

        bool CreateRenderTexture(RenderTexture*&);
        void DestroyRenderTexture(RenderTexture*&);

        bool CreateVertexBuffer(VertexBuffer*& vertexBuffer);
        void DestroyVertexBuffer(VertexBuffer*& vertexBuffer);

        bool CreateIndexBuffer(IndexBuffer*& indexBuffer);
        void DestroyIndexBuffer(IndexBuffer*& indexBuffer);

        bool CreateViewport(Viewport*& viewport);
        void DestroyViewport(Viewport*& viewport);

        bool CreateVertexProgram(VertexProgram*& vertexProgram);
        void DestroyVertexProgram(VertexProgram*& vertexProgram);

        bool CreateFragmentProgram(FragmentProgram*& fragmentProgram);
        void DestroyFragmentProgram(FragmentProgram*& fragmentProgram);

        void Draw(PrimitiveType type, u32 count, u32 startVertex = 0);
        void Draw(VertexBuffer* vertexBuffer, PrimitiveType type, u32 count, u32 startVertex = 0);

        void DrawIndexed(PrimitiveType type, u32 count, u32 numVertex, u32 startVertex = 0, u32 startIndex = 0);
        void DrawIndexed(VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, PrimitiveType type, u32 count, u32 numVertex = 0, u32 startVertex = 0, u32 startIndex = 0);

        friend class Texture;
        friend class RenderTexture;
        friend class VertexBuffer;
        friend class IndexBuffer;
        friend class Viewport;
        friend class VertexProgram;
        friend class FragmentProgram;
        friend class ShaderParameter;

        // HACK: to get theora video to work making a YUV surface is the most 
        // practical
        friend class CVideo;
    };
}
#endif

#endif
