#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_10

#include "gfxAPIDX10.h"
#include "utilAssert.h"

namespace GFX
{
    API::API()
        :   swapChain_(0),
            device_(0),
            modelToWorldScale_(1.0f),
            renderTargetView_(0),
            depthStencilBuffer_(0),
            depthStencilState_(0),
            depthStencilView_(0)
    {
    }

    API::~API()
    {
    }

    void API::Load( int screenWidth,
                    int screenHeight,
                    bool vsync,
                    Util::WindowHandle handle,
                    bool fullscreen, 
                    float screenDepth, 
                    float screenNear )
    {
        HRESULT result;
        IDXGIFactory* factory;
        IDXGIAdapter* adapter;
        IDXGIOutput* adapterOutput;
        unsigned int numModes, numerator = 0, denominator = 0, stringLength;
        DXGI_MODE_DESC* displayModeList;
        DXGI_ADAPTER_DESC adapterDesc;
        int error;
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ID3D10Texture2D* backBufferPtr;
        D3D10_TEXTURE2D_DESC depthBufferDesc;
        D3D10_DEPTH_STENCIL_DESC depthStencilDesc;
        D3D10_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
        D3D10_VIEWPORT viewport;
        float fieldOfView, screenAspect;
        D3D10_RASTERIZER_DESC rasterDesc;

        // Store the vsync setting.
        vsyncEnabled_ = vsync;

        // Create a DirectX graphics interface factory.
        result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
        ENGINE_ASSERT_MSG(!FAILED(result), "Failed to create DXIFactory.");

        // Use the factory to create an adapter for the primary graphics interface (video card).
        result = factory->EnumAdapters(0, &adapter);
        ENGINE_ASSERT_MSG(!FAILED(result), "Failed to create GFX adapter.");

        // Enumerate the primary adapter output (monitor).
        result = adapter->EnumOutputs(0, &adapterOutput);
        ENGINE_ASSERT_MSG(!FAILED(result), "Failed to enumerateate GFX adapter output.");

        // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
        result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
        ENGINE_ASSERT_MSG(!FAILED(result), "Failed to get GFX display modes.");

        // Create a list to hold all the possible display modes for this monitor/video card combination.
        displayModeList = new DXGI_MODE_DESC[numModes];
        ENGINE_ASSERT_MSG(!FAILED(result), "Failed to create GFX display modes array.");

        // Now fill the display mode list structures.
        result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
        ENGINE_ASSERT_MSG(!FAILED(result), "Failed to get GFX display mode list.");

        // Now go through all the display modes and find the one that matches the screen width and height.
        // When a match is found store the numerator and denominator of the refresh rate for that monitor.
        for(unsigned int i = 0; i < numModes; ++i)
        {
            if(displayModeList[i].Width == (unsigned int)screenWidth)
            {
                if(displayModeList[i].Height == (unsigned int)screenHeight)
                {
                    numerator = displayModeList[i].RefreshRate.Numerator;
                    denominator = displayModeList[i].RefreshRate.Denominator;
                }
            }
        }

        // Get the adapter (video card) description.
        result = adapter->GetDesc(&adapterDesc);
        ENGINE_ASSERT_MSG(!FAILED(result), "Failed to get GFX adapter.");

        // Store the dedicated video card memory in megabytes.
        videoCardMemory_ = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

        // Convert the name of the video card to a character array and store it.
        error = wcstombs_s(&stringLength, videoCardDescription_, 128, adapterDesc.Description, 128);
        videoCardDescription_[127] = '\0';

        ENGINE_ASSERT_MSG(!error, "Failed to convert name of video card.");

        // Release the display mode list.
        delete [] displayModeList;
        displayModeList = 0;

        // Release the adapter output.
        adapterOutput->Release();
        adapterOutput = 0;

        // Release the adapter.
        adapter->Release();
        adapter = 0;

        // Release the factory.
        factory->Release();
        factory = 0;

        // Initialize the swap chain description.
        ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

        // Set to a single back buffer.
        swapChainDesc.BufferCount = 1;

        // Set the width and height of the back buffer.
        swapChainDesc.BufferDesc.Width = screenWidth;
        swapChainDesc.BufferDesc.Height = screenHeight;

        // Set regular 32-bit surface for the back buffer.
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

        // Set the refresh rate of the back buffer.
        if(vsyncEnabled_)
        {
            swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
            swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
        }
        else
        {
            swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
            swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        }

        // Set the usage of the back buffer.
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

        // Set the handle for the window to render to.
        swapChainDesc.OutputWindow = handle;

        // Turn multisampling off.
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;

        // Set to full screen or windowed mode.
        if(fullscreen)
            swapChainDesc.Windowed = false;
        else
            swapChainDesc.Windowed = true;

        // Set the scan line ordering and scaling to unspecified.
        swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

        // Discard the back buffer contents after presenting.
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        // Don't set the advanced flags.
        swapChainDesc.Flags = 0;

        // Create the swap chain, Direct3D device, and Direct3D device context.
        result = D3D10CreateDeviceAndSwapChain(NULL,
                                               D3D10_DRIVER_TYPE_HARDWARE,
                                               NULL,
                                               0,
                                               D3D10_SDK_VERSION, 
                                               &swapChainDesc,
                                               &swapChain_,
                                               &device_);

        ENGINE_ASSERT_MSG(!FAILED(result), "Failed to create the swap chain, Direct3D device, and Direct3D device context.");

        // Get the pointer to the back buffer.
        result = swapChain_->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&backBufferPtr);
        ENGINE_ASSERT_MSG(!error, "Failed to get the pointer to the back buffer.");

        // Create the render target view with the back buffer pointer.
        result = device_->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView_);
        ENGINE_ASSERT_MSG(!error, "Failed to create the render target view with the back buffer pointer.");

        // Release pointer to the back buffer as we no longer need it.
        backBufferPtr->Release();
        backBufferPtr = 0;

        // Initialize the description of the depth buffer.
        ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

        // Set up the description of the depth buffer.
        depthBufferDesc.Width = screenWidth;
        depthBufferDesc.Height = screenHeight;
        depthBufferDesc.MipLevels = 1;
        depthBufferDesc.ArraySize = 1;
        depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthBufferDesc.SampleDesc.Count = 1;
        depthBufferDesc.SampleDesc.Quality = 0;
        depthBufferDesc.Usage = D3D10_USAGE_DEFAULT;
        depthBufferDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
        depthBufferDesc.CPUAccessFlags = 0;
        depthBufferDesc.MiscFlags = 0;

        // Create the texture for the depth buffer using the filled out description.
        result = device_->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer_);
        ENGINE_ASSERT_MSG(!error, "Failed to create the texture for the depth buffer.");

        // Initialize the description of the stencil state.
        ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

        // Set up the description of the stencil state.
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;

        depthStencilDesc.StencilEnable = true;
        depthStencilDesc.StencilReadMask = 0xFF;
        depthStencilDesc.StencilWriteMask = 0xFF;

        // Stencil operations if pixel is front-facing.
        depthStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
        depthStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

        // Stencil operations if pixel is back-facing.
        depthStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
        depthStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

        // Create the depth stencil state.
        result = device_->CreateDepthStencilState(&depthStencilDesc, &depthStencilState_);
        ENGINE_ASSERT_MSG(!error, "Failed to create the depth stencil state.");

        // Set the depth stencil state.
        device_->OMSetDepthStencilState(depthStencilState_, 1);

        // Initialize the depth stencil view.
        ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

        // Set up the depth stencil view description.
        depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Texture2D.MipSlice = 0;

        // Create the depth stencil view.
        result = device_->CreateDepthStencilView(depthStencilBuffer_, &depthStencilViewDesc, &depthStencilView_);
        ENGINE_ASSERT_MSG(!error, "Failed to create the depth stencil view.");

        // Bind the render target view and depth stencil buffer to the output render pipeline.
        device_->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);

        // Setup the raster description which will determine how and what polygons will be drawn.
        rasterDesc.AntialiasedLineEnable = true;
        rasterDesc.CullMode              = D3D10_CULL_BACK;
        rasterDesc.DepthBias             = 0;
        rasterDesc.DepthBiasClamp        = 0.0f;
        rasterDesc.DepthClipEnable       = true;
        rasterDesc.FillMode              = D3D10_FILL_SOLID;
        rasterDesc.FrontCounterClockwise = false;
        rasterDesc.MultisampleEnable     = false;
        rasterDesc.ScissorEnable         = false;
        rasterDesc.SlopeScaledDepthBias  = 0.0f;

        // Create the rasterizer state from the description we just filled out.
        result = device_->CreateRasterizerState(&rasterDesc, &rasterState_[RS_SOLID]);
        ENGINE_ASSERT_MSG(!error, "Failed to create the rasterizer.");

        rasterDesc.AntialiasedLineEnable = true;
        rasterDesc.CullMode              = D3D10_CULL_BACK;
        rasterDesc.DepthBias             = 0;
        rasterDesc.DepthBiasClamp        = 0.0f;
        rasterDesc.DepthClipEnable       = true;
        rasterDesc.FillMode              = D3D10_FILL_WIREFRAME;
        rasterDesc.FrontCounterClockwise = false;
        rasterDesc.MultisampleEnable     = false;
        rasterDesc.ScissorEnable         = false;
        rasterDesc.SlopeScaledDepthBias  = 0.0f;

        // Create the rasterizer state from the description we just filled out.
        result = device_->CreateRasterizerState(&rasterDesc, &rasterState_[RS_WIRE]);
        ENGINE_ASSERT_MSG(!error, "Failed to create the rasterizer.");

        // Now set the rasterizer state.
        device_->RSSetState(rasterState_[RS_SOLID]);

        // Setup the viewport for rendering.
        viewport.Width = screenWidth;
        viewport.Height = screenHeight;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;

        // Create the viewport.
        device_->RSSetViewports(1, &viewport);

        // Setup the projection matrix.
        fieldOfView = (float)D3DX_PI / 4.0f;
        screenAspect = (float)screenWidth / (float)screenHeight;

        // Create the projection matrix for 3D rendering.
        D3DXMatrixPerspectiveFovLH(&viewToPersp_, fieldOfView, screenAspect, screenNear, screenDepth);

        // Initialize the world matrix to the identity matrix.
        D3DXMatrixScaling(&modelToWorld_,modelToWorldScale_,modelToWorldScale_,modelToWorldScale_);

        // Create an orthographic projection matrix for 2D rendering.
        D3DXMatrixOrthoLH(&viewToOrtho_, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);
    }

    void API::BeginScene( const Color& color )
    {
        float colorComponent[4];

        color(&colorComponent);
        // Clear the back buffer.
        device_->ClearRenderTargetView(renderTargetView_, colorComponent);

        // Clear the depth buffer.
        device_->ClearDepthStencilView(depthStencilView_, D3D10_CLEAR_DEPTH, 1.0f, 0);
    }

    void API::EndScene()
    {
        if(vsyncEnabled_)
            swapChain_->Present(1,0);
        else
            swapChain_->Present(0,0);
    }

    void API::Unload()
    {
        // Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
        if(swapChain_)
            swapChain_->SetFullscreenState(false, NULL);

        if(rasterState_)
        {
            for(unsigned int i = 0; i < RS_TOTAL; ++i)
            {
                rasterState_[i]->Release();
                rasterState_[i] = 0;
            }
        }

        if(depthStencilView_)
        {
            depthStencilView_->Release();
            depthStencilView_ = 0;
        }

        if(depthStencilState_)
        {
            depthStencilState_->Release();
            depthStencilState_ = 0;
        }

        if(depthStencilBuffer_)
        {
            depthStencilBuffer_->Release();
            depthStencilBuffer_ = 0;
        }

        if(renderTargetView_)
        {
            renderTargetView_->Release();
            renderTargetView_ = 0;
        }

        if(device_)
        {
            device_->Release();
            device_ = 0;
        }

        if(swapChain_)
        {
            swapChain_->Release();
            swapChain_ = 0;
        }
    }

    void API::RenderWireframe( bool val )
    {
        renderWireFrame_ = val;
        if(renderWireFrame_)
            device_->RSSetState(rasterState_[RS_WIRE]);
        else
            device_->RSSetState(rasterState_[RS_SOLID]);
    }

    bool API::RenderWireframe() const
    {
        return renderWireFrame_;
    }

    ID3D10Device* API::Device() const
    {
        return device_;
    }

    D3DXMATRIX API::ViewToPersp() const
    {
        return viewToPersp_;
    }

    D3DXMATRIX API::ModelToWorld() const
    {
        return modelToWorld_;
    }

    D3DXMATRIX API::ViewToOrtho() const
    {
        return viewToOrtho_;
    }

    std::string API::VideoCardDescription() const
    {
        return videoCardDescription_;
    }

    int API::VideoCardMemory() const
    {
        return videoCardMemory_;
    }

    void API::ModelToWorldScale( float val )
    {
        modelToWorldScale_ = max(0.0001f,val);
        D3DXMatrixScaling(&modelToWorld_,modelToWorldScale_,modelToWorldScale_,modelToWorldScale_);
    }

    float API::ModelToWorldScale() const
    {
        return modelToWorldScale_;
    }
}
#endif