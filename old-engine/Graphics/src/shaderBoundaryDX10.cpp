#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_10

#include "shaderBoundaryDX10.h"
#include "shaderFactory.h"
#include "shaderTypes.h"

namespace GFX
{
    Util::FactoryPlant<Shader,ShaderBoundary> ShaderBoundary::factoryPlant_(ShaderFactory::InstancePtr(),ST_BOUNDARY);

    ShaderBoundary::ShaderBoundary()
        :   Shader(ST_BOUNDARY),
            edgeColorPtr_(0),
            boundaryColor_(Color::Red)
    {
    }

    ShaderBoundary::ShaderBoundary( const ShaderBoundary& shader )
        :   Shader(shader)
    {
    }

    //--------------------------------------------------------------------------

    Color ShaderBoundary::BoundaryColor() const
    {
        return boundaryColor_;
    }

    void ShaderBoundary::BoundaryColor( const Color& val )
    {
        boundaryColor_ = val;
    }

    //--------------------------------------------------------------------------

    void ShaderBoundary::load( ID3D10Effect* effect )
    {
        edgeColorPtr_ = effect->GetVariableByName("EdgeColor")->AsVector();
    }

    void ShaderBoundary::sendPixelShaderParams()
    {
        float vec4[4] = 
        {
            boundaryColor_.R(),
            boundaryColor_.G(),
            boundaryColor_.B(),
            1.0f
        };

        edgeColorPtr_->SetFloatVector(vec4);
    }
}
#endif