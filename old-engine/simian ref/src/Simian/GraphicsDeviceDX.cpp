/************************************************************************/
/*!
\file		GraphicsDeviceDX.cpp
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

#include "Simian/GraphicsDeviceDX.h"
#include "Simian/Window.h"
#include "Simian/Texture.h"
#include "Simian/RenderTexture.h"
#include "Simian/VertexBuffer.h"
#include "Simian/IndexBuffer.h"
#include "Simian/Viewport.h"
#include "Simian/VertexProgramDX.h"
#include "Simian/LogManager.h"

#include "IL/il.h"
#include "IL/ilu.h"

#include <Windows.h>

#include <algorithm>

namespace Simian
{
    static const u32 TEXTURE_MATRIX_COUNT = 8;
    static const D3DFORMAT FRAMEBUFFER_FORMAT = D3DFMT_X8R8G8B8;

    GraphicsDevice::GraphicsDevice()
        : direct3d_(0),
          device_(0),
          deviceLost_(false)
    {
        textureMatrix_.resize(TEXTURE_MATRIX_COUNT);
    }

    //--------------------------------------------------------------------------

    LPDIRECT3DDEVICE9 GraphicsDevice::D3DDevice() const
    {
        return device_;
    }

    void GraphicsDevice::RenderState( u32 state, u32 value )
    {
        device_->SetRenderState(static_cast<D3DRENDERSTATETYPE>(state), value);
    }

    Simian::u32 GraphicsDevice::RenderState( u32 state )
    {
        DWORD ret;
        device_->GetRenderState(static_cast<D3DRENDERSTATETYPE>(state), &ret);
        return ret;
    }

    const Matrix& GraphicsDevice::World() const
    {
        return GraphicsDeviceBase::World();
    }

    void GraphicsDevice::World( const Simian::Matrix& world )
    {
        GraphicsDeviceBase::World(world);
        device_->SetTransform(D3DTS_WORLD, 
            reinterpret_cast<const D3DMATRIX*>(world.Transposed()[0]));
    }

    const Matrix& GraphicsDevice::View() const
    {
        return GraphicsDeviceBase::View();
    }

    void GraphicsDevice::View( const Simian::Matrix& view )
    {
        GraphicsDeviceBase::View(view);
        device_->SetTransform(D3DTS_VIEW,
            reinterpret_cast<const D3DMATRIX*>(view.Transposed()[0]));
    }

    const Matrix& GraphicsDevice::Projection() const
    {
        return GraphicsDeviceBase::Projection();
    }

    void GraphicsDevice::Projection( const Simian::Matrix& projection )
    {
        GraphicsDeviceBase::Projection(projection);
        device_->SetTransform(D3DTS_PROJECTION,
            reinterpret_cast<const D3DMATRIX*>(projection.Transposed()[0]));
    }

    const Matrix& GraphicsDevice::TextureMatrix( u32 index ) const
    {
        return GraphicsDeviceBase::TextureMatrix(index);
    }

    void GraphicsDevice::TextureMatrix( u32 index, const Matrix& val )
    {
        GraphicsDeviceBase::TextureMatrix(index, val);
        device_->SetTransform(static_cast<D3DTRANSFORMSTATETYPE>(
            static_cast<u32>(D3DTS_TEXTURE0) + index),
            reinterpret_cast<const D3DMATRIX*>(val.Transposed()[0]));
    }

    void GraphicsDevice::TextureState( u32 stage, u32 state, u32 value )
    {
        device_->SetTextureStageState(stage, static_cast<D3DTEXTURESTAGESTATETYPE>(state), value);
    }

    Simian::u32 GraphicsDevice::TextureState( u32 stage, u32 state )
    {
        DWORD value;
        device_->GetTextureStageState(stage, static_cast<D3DTEXTURESTAGESTATETYPE>(state), &value);
        return value;
    }

    //--------------------------------------------------------------------------

    bool GraphicsDevice::init_()
    {
        direct3d_ = Direct3DCreate9(D3D_SDK_VERSION);

        if (direct3d_ == NULL)
            return false;

        HRESULT result = direct3d_->CheckDeviceType(D3DADAPTER_DEFAULT, 
            D3DDEVTYPE_HAL, FRAMEBUFFER_FORMAT, FRAMEBUFFER_FORMAT, Fullscreen());

        if (FAILED(result))
            return false;

        if (ParentWindow())
            setupWindow_();

        D3DPRESENT_PARAMETERS pp;
        setupPresentParameters_(pp);
        
#if SIMIAN_USENVPERFHUD == 1
        UINT adapter = D3DADAPTER_DEFAULT;
        D3DDEVTYPE devType = D3DDEVTYPE_HAL;

        for (UINT i = 0; i < direct3d_->GetAdapterCount(); ++i)
        {
            D3DADAPTER_IDENTIFIER9 identifier;
            if (SUCCEEDED(direct3d_->GetAdapterIdentifier(i, 0, &identifier)) &&
                strstr(identifier.Description, "PerfHUD") != 0)
            {
                adapter = i;
                devType = D3DDEVTYPE_REF;
                break;
            }
        }
        result = direct3d_->CreateDevice(adapter, devType, hWnd_(),
            D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &device_);
#else
        result = direct3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd_(),
            D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &device_);
#endif

        if (FAILED(result))
        {
            direct3d_->Release();
            direct3d_ = 0;
            return false;
        }

        device_->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        device_->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

        ilInit();
        iluInit();

        return true;
    }

    void GraphicsDevice::deinit_()
    {
        ilShutDown();

        if (device_)
            device_->Release();
        device_ = 0;

        if (direct3d_)
            direct3d_->Release();
        direct3d_ = 0;
    }

    void GraphicsDevice::setupPresentParameters_(D3DPRESENT_PARAMETERS& pp)
    {
        ZeroMemory(&pp, sizeof(pp));
        pp.BackBufferWidth = static_cast<UINT>(Size().X());
        pp.BackBufferHeight = static_cast<UINT>(Size().Y());
        pp.BackBufferFormat = FRAMEBUFFER_FORMAT;
        pp.BackBufferCount = 1;
        pp.MultiSampleType = D3DMULTISAMPLE_NONE;
        pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        pp.hDeviceWindow = hWnd_();
        pp.Windowed = static_cast<BOOL>(!Fullscreen());
        pp.EnableAutoDepthStencil = TRUE;
        pp.AutoDepthStencilFormat = D3DFMT_D24S8;
        pp.FullScreen_RefreshRateInHz = Fullscreen() ? D3DPRESENT_RATE_DEFAULT : 0;
        pp.PresentationInterval = VSync() ? (D3DPRESENT_INTERVAL_ONE | D3DPRESENT_DONOTWAIT) : D3DPRESENT_INTERVAL_IMMEDIATE;
    }

    void GraphicsDevice::setupWindow_()
    {
        if (Fullscreen())
        {
            ParentWindow()->Position(Simian::Vector2::Zero);
            ParentWindow()->Size(Size());
            ParentWindow()->Style(OS::SWS_POPUP);
        }
        else
        {
            Simian::f32 width, height;
            Simian::OS::Instance().DesktopResolution(width, height);
            ParentWindow()->Position((Simian::Vector2(width, height) - Size()) * 0.5f);
            ParentWindow()->Size(Size());
            ParentWindow()->Style(OS::SWS_BORDERS);
        }
    }

    HWND& GraphicsDevice::hWnd_() const
    {
        return *reinterpret_cast<HWND*>(ParentWindowHandle());
    }

    void GraphicsDevice::resetDevice_()
    {
        if (device_->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ApplyChanges();
    }

    //--------------------------------------------------------------------------

    void GraphicsDevice::EnumerateResolutions( std::vector<Vector2>& resolutions )
    {
        D3DDISPLAYMODE displayMode;

        UINT modes = direct3d_->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);

        resolutions.clear();
        for (u32 i = 0; i < modes; ++i)
        {
            // find all resolutions in the device
            direct3d_->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8,
                i, &displayMode);

            resolutions.push_back(Simian::Vector2(
                static_cast<f32>(displayMode.Width), 
                static_cast<f32>(displayMode.Height)));
        }

        // make resolutions unique
        struct
        {
            bool operator()(const Simian::Vector2& a, const Simian::Vector2& b) const
            {
                return a.X() == b.X() && a.Y() == b.Y();
            }
        } compare_resolutions;
        std::vector<Simian::Vector2>::iterator newEnd = std::unique(resolutions.begin(), resolutions.end(), compare_resolutions);
        resolutions.resize(newEnd - resolutions.begin());
    }

    void GraphicsDevice::Begin()
    {
        if (deviceLost_)
            resetDevice_();

        device_->BeginScene();
    }

    void GraphicsDevice::Clear( const Simian::Color& color )
    {
        device_->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER,
            color.ColorU32(), 1.0f, 0);
    }

    void GraphicsDevice::End()
    {
        device_->EndScene();
    }

    void GraphicsDevice::Swap()
    {
        deviceLost_ = device_->Present(NULL, NULL, NULL, NULL) == D3DERR_DEVICELOST;
    }

    bool GraphicsDevice::ApplyChanges()
    {
        D3DPRESENT_PARAMETERS pp;
        setupPresentParameters_(pp);

        // release default pool objects
        for (u32 i = 0; i < indexBuffers_.size(); ++i)
            indexBuffers_[i]->onLostDevice_();
        for (u32 i = 0; i < vertexBuffers_.size(); ++i)
            vertexBuffers_[i]->onLostDevice_();
        for (u32 i = 0; i < renderTextures_.size(); ++i)
            renderTextures_[i]->onLostDevice_();
        for (u32 i = 0; i < textures_.size(); ++i)
            textures_[i]->onLostDevice_();

        // release any other resources
        releaseCallbacks_();

        // reset the device
        device_->Reset(&pp);

        // reset any other resources
        resetCallbacks_();

        // reload the default pool objects
        for (u32 i = 0; i < textures_.size(); ++i)
            textures_[i]->onResetDevice_();
        for (u32 i = 0; i < renderTextures_.size(); ++i)
            renderTextures_[i]->onResetDevice_();
        for (u32 i = 0; i < vertexBuffers_.size(); ++i)
            vertexBuffers_[i]->onResetDevice_();
        for (u32 i = 0; i < indexBuffers_.size(); ++i)
            indexBuffers_[i]->onResetDevice_();

        if (ParentWindow())
            setupWindow_();

        device_->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

        return true;
    }

    bool GraphicsDevice::CreateTexture( Texture*& texture )
    {
        texture = new Texture(this);
        textures_.push_back(texture);
        return texture ? true : false;
    }

    void GraphicsDevice::DestroyTexture( Texture*& texture )
    {
        std::vector<Texture*>::iterator i = std::find(textures_.begin(), textures_.end(), texture);
        if (i != textures_.end())
            textures_.erase(i);
        delete texture;
        texture = 0;
    }

    bool GraphicsDevice::CreateRenderTexture( RenderTexture*& texture )
    {
        texture = new RenderTexture(this);
        renderTextures_.push_back(texture);
        return texture ? true : false;
    }

    void GraphicsDevice::DestroyRenderTexture( RenderTexture*& texture )
    {
        std::vector<RenderTexture*>::iterator i = std::find(renderTextures_.begin(), renderTextures_.end(), texture);
        if (i != renderTextures_.end())
            renderTextures_.erase(i);
        delete texture;
        texture = 0;
    }

    bool GraphicsDevice::CreateVertexBuffer( VertexBuffer*& vertexBuffer )
    {
        vertexBuffer = new VertexBuffer(this);
        vertexBuffers_.push_back(vertexBuffer);
        return vertexBuffer ? true : false;
    }

    void GraphicsDevice::DestroyVertexBuffer( VertexBuffer*& vertexBuffer )
    {
        std::vector<VertexBuffer*>::iterator i = std::find(vertexBuffers_.begin(), vertexBuffers_.end(), vertexBuffer);
        if (i != vertexBuffers_.end())
            vertexBuffers_.erase(i);
        delete vertexBuffer;
        vertexBuffer = 0;
    }

    bool GraphicsDevice::CreateIndexBuffer( IndexBuffer*& indexBuffer )
    {
        indexBuffer = new IndexBuffer(this);
        indexBuffers_.push_back(indexBuffer);
        return indexBuffer ? true : false;
    }

    void GraphicsDevice::DestroyIndexBuffer(IndexBuffer*& indexBuffer)
    {
        std::vector<IndexBuffer*>::iterator i = std::find(indexBuffers_.begin(), indexBuffers_.end(), indexBuffer);
        if (i != indexBuffers_.end())
            indexBuffers_.erase(i);
        delete indexBuffer;
        indexBuffer = 0;
    }

    bool GraphicsDevice::CreateViewport( Viewport*& viewport )
    {
        viewport = new Viewport(this);
        return viewport ? true : false;
    }

    void GraphicsDevice::DestroyViewport( Viewport*& viewport )
    {
        delete viewport;
        viewport = 0;
    }

    bool GraphicsDevice::CreateVertexProgram( VertexProgram*& vertexProgram )
    {
        vertexProgram = new VertexProgram(this);
        return vertexProgram ? true : false;
    }

    void GraphicsDevice::DestroyVertexProgram( VertexProgram*& vertexProgram )
    {
        delete vertexProgram;
        vertexProgram = 0;
    }

    bool GraphicsDevice::CreateFragmentProgram( FragmentProgram*& fragmentProgram )
    {
        fragmentProgram = new FragmentProgram(this);
        return fragmentProgram ? true : false;
    }

    void GraphicsDevice::DestroyFragmentProgram( FragmentProgram*& fragmentProgram )
    {
        delete fragmentProgram;
        fragmentProgram = 0;
    }

    void GraphicsDevice::Draw(PrimitiveType type, u32 count, u32 startVertex)
    {
        device_->DrawPrimitive(static_cast<D3DPRIMITIVETYPE>(type), startVertex, count);
    }

    void GraphicsDevice::Draw(VertexBuffer* vertexBuffer, PrimitiveType type, u32 count, u32 startVertex)
    {
        vertexBuffer->Set();
        Draw(type, count, startVertex);
        vertexBuffer->Unset();
    }

    void GraphicsDevice::DrawIndexed( PrimitiveType type, u32 count, u32 numVertex, u32 startVertex, u32 startIndex )
    {
        device_->DrawIndexedPrimitive(static_cast<D3DPRIMITIVETYPE>(type), startVertex, 0, numVertex, startIndex, count);
    }

    void GraphicsDevice::DrawIndexed( VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, PrimitiveType type, u32 count, u32 numVertex, u32 startVertex, u32 startIndex )
    {
        vertexBuffer->Set();
        indexBuffer->Set();
        DrawIndexed(type, count, numVertex ? numVertex : vertexBuffer->Size(), startVertex, startIndex);
        indexBuffer->Unset();
        vertexBuffer->Unset();
    }

    void GraphicsDevice::registerResource_( const Delegate& release, const Delegate& reset )
    {
        releaseCallbacks_ += release;
        resetCallbacks_ += reset;
    }

    void GraphicsDevice::unregisterResource_( const Delegate& release,const Delegate& reset )
    {
        releaseCallbacks_ -= release;
        resetCallbacks_ -= reset;
    }
}

#endif
