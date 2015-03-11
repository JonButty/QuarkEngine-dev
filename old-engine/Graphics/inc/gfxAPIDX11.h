#ifndef GFXAPIDX11_H
#define GFXAPIDX11_H

#include "singleton.h"
#include "os.h"
#include "configGraphicsDLL.h"
#include "color.h"

#include <string>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>

namespace GFX
{
    class ENGINE_GRAPHICS_EXPORT API
        :   public Util::Singleton<API>
    {
    public:
        enum RasterState
        {
            RS_SOLID,
            RS_WIRE,
            RS_TOTAL
        };
    public:
        API();
        ~API();
        void Load(int screenWidth,
                  int screenHeight,
                  bool vsync,
                  Util::WindowHandle handle,
                  bool fullscreen, 
                  float screenDepth,
                  float screenNear);
        void BeginScene(const Color& color);
        void EndScene();
        void Unload();
        void RenderWireframe(unsigned int type);
    public:
        ID3D11Device* Device() const;
        ID3D11DeviceContext* DeviceContext() const;
        D3DXMATRIX ViewToPersp() const;
        D3DXMATRIX ModelToWorld() const;
        D3DXMATRIX ViewToOrtho() const;
        std::string VideoCardDescription() const;
        int VideoCardMemory() const;
    private:
        bool vsyncEnabled_;
        int videoCardMemory_;
        char videoCardDescription_[128];
        IDXGISwapChain* swapChain_;
        ID3D11Device* device_;
        ID3D11DeviceContext* deviceContext_;
        ID3D11RenderTargetView* renderTargetView_;
        ID3D11Texture2D* depthStencilBuffer_;
        ID3D11DepthStencilState* depthStencilState_;
        ID3D11DepthStencilView* depthStencilView_;
        ID3D11RasterizerState* rasterState_[RS_TOTAL];
        D3DXMATRIX viewToPersp_;
        D3DXMATRIX modelToWorld_;
        D3DXMATRIX viewToOrtho_;
    };
}

#endif