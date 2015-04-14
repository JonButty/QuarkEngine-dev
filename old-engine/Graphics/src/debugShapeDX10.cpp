#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_10

#include "debugShapeDX10.h"
#include "gfxAPIDX10.h"
#include "vertex.h"
#include "cameraDX.h"
#include "shaderManager.h"
#include "shaderTypes.h"
#include "boundingVolumeTypes.h"
#include "boundingVolumeFactory.h"

namespace GFX
{
    const unsigned int DebugShape::BoundingVolumeTypeMap[DS_TOTAL] = 
    {
        Phy::BV_AABB,
        Phy::BV_OBB,
        Phy::BV_SPHERE,
        Phy::BV_ELLIPSE
    };

    DebugShape::DebugShape( unsigned int type )
        :   type_(type),
            enable_(true),
            mappedVertexList_(0),
            vertBuffer_(0),
            indexBuffer_(0),
            shapeShader_(0),
            shape_(0),
            vertexCount_(0),
            indexCount_(0),
            shapeColor_(Color::Green)
    {
    }

    DebugShape::DebugShape( const DebugShape& val )
        :   type_(val.type_),
            enable_(true),
            mappedVertexList_(0),
            vertBuffer_(0),
            indexBuffer_(0),
            shapeShader_(0),
            shape_(0),
            vertexCount_(0),
            indexCount_(0),
            shapeColor_(val.shapeColor_)
    {
    }

    DebugShape::~DebugShape()
    {
    }

    void DebugShape::Load()
    {
        if(!shapeShader_)
            ShaderManager::Instance().GetShader(ST_DEFAULT,shapeShader_);

        if(!shape_)
            Phy::BoundingVolumeFactory::Instance().CreateInstance(BoundingVolumeTypeMap[type_],shape_);

        load();
    }

    void DebugShape::Render( Camera* camera,
                             Math::Vec3F* offsets,
                             unsigned int offsetCount)
    {
        if(!enable_)
            return;
        update();

        D3DXMATRIX modelToWorld = API::Instance().ModelToWorld();
        
        D3DXMATRIX translation;
        D3DXMATRIX worldToView  = camera->WorldToView();
        D3DXMATRIX viewToPersp  = API::Instance().ViewToPersp();

        for(unsigned int i = 0; i < vertexCount_; ++i)
            mappedVertexList_[i].color_ = shapeColor_;

        bool noOffsetGiven = false;
        if(!offsets || (offsetCount == 0))
        {
            noOffsetGiven = true;
            offsets = new Math::Vec3F(Math::Vec3F::Zero);
            offsetCount = 1;
        }

        // Loop through each position translation
        for(unsigned int i = 0; i < offsetCount; ++i)
        {
            D3DXMatrixTranslation(&translation,offsets[i].X(), offsets[i].Y(), offsets[i].Z());
            shapeShader_->Render(indexCount_,
                                 modelToWorld,
                                 translation,
                                 worldToView,
                                 viewToPersp);
        }

        if(noOffsetGiven)
            delete offsets;
    }

    void DebugShape::Unload()
    {
        if(vertBuffer_)
        {
            vertBuffer_->Release();
            vertBuffer_ = 0;
        }

        if(indexBuffer_)
        {
            indexBuffer_->Release();
            indexBuffer_ = 0;
        }

        if(!shapeShader_)
            shapeShader_ = 0;

        if(!shape_)
        {
            Phy::BoundingVolumeFactory::Instance().DestroyInstance(BoundingVolumeTypeMap[type_],shape_);
            shape_ = 0;
        }

        unload();
    }

    void DebugShape::RenderSetup()
    {
        ID3D10Device* device = API::Instance().Device();
        unsigned int stride = sizeof(Vertex);
        unsigned int offset = 0;

        device->IASetVertexBuffers(0, 1, &vertBuffer_, &stride, &offset);
        device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
        device->IASetIndexBuffer(indexBuffer_, DXGI_FORMAT_R32_UINT, 0);
    }

    //--------------------------------------------------------------------------

    void DebugShape::Enable( bool val )
    {
        enable_ = val;
    }

    bool DebugShape::Enable() const
    {
        return enable_;
    }

    void DebugShape::ShapeColor( const Color& val )
    {
        shapeColor_ = val;
    }

    GFX::Color DebugShape::ShapeColor() const
    {
        return shapeColor_;
    }

    unsigned int DebugShape::Type() const
    {
        return type_;
    }

    Phy::BoundingVolume* DebugShape::Shape() const
    {
        return shape_;
    }

    void DebugShape::Shape(Phy::BoundingVolume* val)
    {
        shape_ = val;
    }
}

#endif