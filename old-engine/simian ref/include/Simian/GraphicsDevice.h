/************************************************************************/
/*!
\file		GraphicsDevice.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_GRAPHICSDEVICE_H_
#define SIMIAN_GRAPHICSDEVICE_H_

#include "SimianPlatform.h"
#include "Vector2.h"
#include "Debug.h"
#include "Singleton.h"
#include "Matrix.h"
#include "OS.h"
#include "Utility.h"
#include "Window.h"

#include <vector>

namespace Simian
{
    class Window;
    class Color;
    class Texture;
    class RenderTexture;
    class VertexBuffer;
    class IndexBuffer;
    class Viewport;
    class VertexProgram;
    class FragmentProgram;

    class SIMIAN_EXPORT GraphicsDeviceBase
    {
    private:
        enum PrimitiveType
        {
            PT_POINT,
            PT_LINE,
            PT_TRIANGLES,
            PT_TRIANGLESTRIP,
            PT_TRIANGLEFAN
        };

        enum DeviceFlag
        {
            DF_FULLSCREEN,
            DF_VSYNC,
            DF_TOTAL
        };

        enum RenderStateFlag
        {
            RS_LIGHTING,
            RS_CULLING,
            RS_BLENDING,
            RS_DEPTHWRITE,
            RS_DEPTHENABLED,
            RS_SRCBLEND,
            RS_DSTBLEND,
            RS_ALPHATEST,
            RS_ALPHAREF,
            RS_ALPHAFUNC
        };

        enum CullMode
        {
            SCM_NONE,
            SCM_CW,
            SCM_CCW
        };

        enum BlendMode
        {
            BM_ZERO,
            BM_ONE,
            BM_SRCALPHA,
            BM_INVSRCALPHA
        };

        enum AlphaTestFunc
        {
            AF_ALWAYS,
            AF_NEVER,
            AF_GREATER,
            AF_LESS
        };
    private:
        Simian::Vector2 size_;
        u8 flags_;

        Matrix world_;
        Matrix view_;
        Matrix projection_;

        Window* parentWindow_;
        WindowHandle parentWindowHandle_;
    protected:
        std::vector<Matrix> textureMatrix_;
    public:
        const Simian::Vector2& Size() const { return size_; }
        void Size(const Simian::Vector2& val) { size_ = val; }

        bool Fullscreen() const { return SIsFlagSet(flags_, DF_FULLSCREEN); }
        void Fullscreen(bool val) { SFlagSet(flags_, DF_FULLSCREEN, val); }

        bool VSync() const { return SIsFlagSet(flags_, DF_VSYNC); }
        void VSync(bool val) { SFlagSet(flags_, DF_VSYNC, val); }

        void RenderState(u32 state, u32 value) { state; value; }
        u32 RenderState(u32 state) { state; return 0; }

        const Matrix& World() const { return world_; }
        void World(const Matrix& val) { world_ = val; }

        const Matrix& View() const { return view_; }
        void View(const Matrix& val) { view_ = val; }

        const Matrix& Projection() const { return projection_; }
        void Projection(const Matrix& val) { projection_ = val; }

        const Matrix& TextureMatrix(u32 index) const 
        {
            SAssert(index < textureMatrix_.size(), "Accessing invalid texture matrix.");
            return textureMatrix_[index]; 
        }

        void TextureMatrix(u32 index, const Matrix& val) 
        { 
            SAssert(index < textureMatrix_.size(), "Accessing invalid texture matrix.");
            textureMatrix_[index] = val;
        }

        void TextureState(u32 sampler, u32 state, u32 value)
        {
            sampler; state; value;
        }

        u32 TextureState(u32 sampler, u32 state)
        {
            sampler; state;
            return 0;
        }

        Window* ParentWindow() const { return parentWindow_; }

        Simian::WindowHandle ParentWindowHandle() const { return parentWindowHandle_; }
    protected:
        S_NIMPL virtual bool init_() = 0;
        S_NIMPL virtual void deinit_() = 0;
    public:
        GraphicsDeviceBase()
            : size_(800.0f, 600.0f),
            flags_(0),
            parentWindow_(0) {}

        virtual ~GraphicsDeviceBase() {}

        bool Startup(Window& window)
        {
            SAssert(textureMatrix_.size() > 0, "Must define texture matrix count.");
            parentWindow_ = &window;
            parentWindowHandle_ = parentWindow_->Handle();
            return init_();
        }

        bool Startup(WindowHandle& handle)
        {
            SAssert(textureMatrix_.size() > 0, "Must define texture matrix count.");
            parentWindow_ = NULL;
            parentWindowHandle_ = handle;
            return init_();
        }

        void Shutdown()
        {
            deinit_();
        }

        S_NIMPL void EnumerateResolutions(std::vector<Vector2>& resolutions) { resolutions; }

        // we don't specify virtual for these for efficiency
        S_NIMPL void Begin() {}
        S_NIMPL void Clear(const Simian::Color&) {}
        S_NIMPL void End() {}
        S_NIMPL void Swap() {}

        S_NIMPL bool ApplyChanges() { return false; }

        S_NIMPL bool CreateTexture(Texture*&) { return false; }
        S_NIMPL void DestroyTexture(Texture*&) {}

        S_NIMPL bool CreateRenderTexture(RenderTexture*&) { return false; }
        S_NIMPL void DestroyRenderTexture(RenderTexture*&) {}

        S_NIMPL bool CreateVertexBuffer(VertexBuffer*&) { return false; }
        S_NIMPL void DestroyVertexBuffer(VertexBuffer*&) {}

        S_NIMPL bool CreateIndexBuffer(IndexBuffer*&) { return false; }
        S_NIMPL void DestroyIndexBuffer(IndexBuffer*&) {}

        S_NIMPL bool CreateViewport(Viewport*&) { return false; }
        S_NIMPL void DestroyViewport(Viewport*&) {}

        S_NIMPL bool CreateVertexProgram(VertexProgram*&) { return false; }
        S_NIMPL void DestroyVertexProgram(VertexProgram*&) {}

        S_NIMPL bool CreateFragmentProgram(FragmentProgram*&) { return false; }
        S_NIMPL void DestroyFragmentProgram(FragmentProgram*&) {}

        S_NIMPL void Draw(PrimitiveType type, u32 count, u32 startVertex) { type; count; startVertex; }
        S_NIMPL void Draw(VertexBuffer* vertexBuffer, PrimitiveType type, u32 count, u32 startVertex) { vertexBuffer; type; count; startVertex; }

        S_NIMPL void DrawIndexed(PrimitiveType type, u32 count, u32 numVertex, u32 startVertex, u32 startIndex) { type; count; numVertex; startVertex; startIndex; }
        S_NIMPL void DrawIndexed(VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, PrimitiveType type, u32 count, u32 numVertex, u32 startVertex, u32 startIndex) { vertexBuffer; indexBuffer; type; count; numVertex; startVertex; startIndex; }
    };
}

#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX
#include "GraphicsDeviceDX.h"
#elif SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_GL
#include "GraphicsDeviceGL.h"
#endif

#endif
