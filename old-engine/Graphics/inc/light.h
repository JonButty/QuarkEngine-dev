#ifndef LIGHT_H
#define LIGHT_H

#include "vector3.h"
#include "color.h"
#include "configGraphicsDLL.h"

namespace GFX
{
    class ENGINE_GRAPHICS_EXPORT Light
    {
    public:
        Light(unsigned int type,
              const Math::Vec3F& pos = Math::Vec3F::Zero,
              const Color& ambient = Color::White,
              const Color& diffuse = Color::White);
        Light(const Light& val);
        virtual ~Light();
    public:
        unsigned int Type() const;
        Math::Vec3F Pos() const;
        void Pos(const Math::Vec3F& val);
        Color Ambient() const;
        void Ambient(const Color& val);
        Color Diffuse() const;
        void Diffuse(const Color& val);
    private:
        unsigned int type_;
        Math::Vec3F pos_;
        Color ambient_;
        Color diffuse_;
    };
}

#endif