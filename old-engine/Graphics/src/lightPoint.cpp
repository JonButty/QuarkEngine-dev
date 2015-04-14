#include "lightPoint.h"
#include "lightTypes.h"
#include "lightFactory.h"

namespace GFX
{
    Util::FactoryPlant<Light,LightPoint> LightPoint::factoryPlant_(LightFactory::InstancePtr(),LT_POINT);

    LightPoint::LightPoint( float specularPower,
                                const Color& specular )
                                :   Light(LT_POINT),
                                    specularPower_(specularPower),
                                    specular_(specular)
    {
    }

    LightPoint::LightPoint( const LightPoint& val )
        :   Light(val),
            specularPower_(val.specularPower_),
            specular_(val.specular_)
    {
    }

    //--------------------------------------------------------------------------

    float LightPoint::SpecularPower() const
    {
        return specularPower_;
    }

    void LightPoint::SpecularPower( float val )
    {
        specularPower_ = val;
    }

    GFX::Color LightPoint::Specular() const
    {
        return specular_;
    }

    void LightPoint::Specular( const Color& val )
    {
        specular_ = val;
    }
}