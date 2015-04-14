#ifndef APIDX10_H
#define APIDX10_H

#include "singleton.h"
#include "os.h"
#include "configGraphicsDLL.h"
#include "color.h"

#include <string>

#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3d10.h>
#include <d3dx10.h>

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
    public:
        void RenderWireframe(bool val);
        bool RenderWireframe() const;
        ID3D10Device* Device() const;
        D3DXMATRIX ViewToPersp() const;
        D3DXMATRIX ModelToWorld() const;
        D3DXMATRIX ViewToOrtho() const;
        std::string VideoCardDescription() const;
        int VideoCardMemory() const;
        float ModelToWorldScale() const;
        void ModelToWorldScale(float val);
    private:
        bool vsyncEnabled_;
        bool renderWireFrame_;
        int videoCardMemory_;
        float modelToWorldScale_;
        char videoCardDescription_[128];
        IDXGISwapChain* swapChain_;
        ID3D10Device* device_;
        ID3D10RenderTargetView* renderTargetView_;
        ID3D10Texture2D* depthStencilBuffer_;
        ID3D10DepthStencilState* depthStencilState_;
        ID3D10DepthStencilView* depthStencilView_;
        ID3D10RasterizerState* rasterState_[RS_TOTAL];
        D3DXMATRIX viewToPersp_;
        D3DXMATRIX modelToWorld_;
        D3DXMATRIX viewToOrtho_;
    };
}
#endif