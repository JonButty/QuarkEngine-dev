#include "light.h"

namespace GFX
{
    Light::Light( unsigned int type,
                  const Math::Vec3F& pos,
                  const Color& ambient, 
                  const Color& diffuse )
                  : type_(type),
                    pos_(pos),
                    ambient_(ambient),
                    diffuse_(diffuse)
    {
    }

    Light::Light( const Light& val )
        :   type_(val.type_),
            pos_(val.pos_),
            ambient_(val.ambient_),
            diffuse_(val.diffuse_)
    {
    }

    Light::~Light()
    {
    }

    //--------------------------------------------------------------------------

    unsigned int Light::Type() const
    {
        return type_;
    }

    Math::Vec3F Light::Pos() const
    {
        return pos_;
    }

    void Light::Pos( const Math::Vec3F& val )
    {
        pos_ = val;
    }

    GFX::Color Light::Ambient() const
    {
        return ambient_;
    }

    void Light::Ambient( const Color& val )
    {
        ambient_ = val;
    }

    GFX::Color Light::Diffuse() const
    {
        return diffuse_;
    }

    void Light::Diffuse( const Color& val )
    {
        diffuse_ = val;
    }
}