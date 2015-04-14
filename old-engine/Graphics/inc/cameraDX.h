#ifndef CAMERADX_H
#define CAMERADX_H

#include "angle.h"
#include "vector3.h"
#include "configGraphicsDLL.h"
#include "configGFX.h"

#include <D3DX10math.h>

namespace GFX
{
    class ENGINE_GRAPHICS_EXPORT Camera
    {
    public:
        Camera(unsigned int type,
               bool enable = true,
               Math::RadianF xAxis = 0,
               Math::RadianF yAxis = 0,
               Math::RadianF zAxis = 0,
               Math::Vec3F pos = Math::Vec3F::Zero);
        Camera(const Camera& val);
        virtual ~Camera();
        void Load();
        void Update(float dt);
        void Unload();
    public:
        bool Enable() const;
        void Enable(bool val);
        Math::Vec3F Pos() const;
        void Pos(const Math::Vec3F& val);
        void Pos(float x,
                 float y,
                 float z);
        Math::RadianF XAxisRotation() const;
        void XAxisRotation(Math::RadianF val);
        Math::RadianF YAxisRotation() const;
        void YAxisRotation(Math::RadianF val);
        Math::RadianF ZAxisRotation() const;
        void ZAxisRotation(Math::RadianF val);
        D3DXMATRIX WorldToView() const;
        unsigned int Type() const;
    protected:
        virtual void load();
        virtual void update(float dt);
        virtual void unload();
    private:
        bool enable_;
        unsigned int type_;
        Math::Vec3F pos_;
        Math::RadianF xAxisRot_;
        Math::RadianF yAxisRot_;
        Math::RadianF zAxisRot_;
        D3DXMATRIX worldToView_;
    };
}

#endif