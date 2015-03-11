#ifndef LIGHTPOINT_H
#define LIGHTPOINT_H

#include "light.h"
#include "factory.h"
#include "configGraphicsDLL.h"

namespace GFX
{
    class ENGINE_GRAPHICS_EXPORT LightPoint
        :   public Light
    {
    public:
        LightPoint(float specularPower = 0.f,
                     const Color& specular = Color::White);
        LightPoint(const LightPoint& val);
    public:
        float SpecularPower() const;
        void SpecularPower(float val);
        Color Specular() const;
        void Specular(const Color& val);
    private:
        float specularPower_;
        Color specular_;
        static Util::FactoryPlant<Light,LightPoint>factoryPlant_;
    };
}

#endif