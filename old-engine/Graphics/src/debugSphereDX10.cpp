#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_10

#include "debugSphereDX10.h"
#include "debugShapeTypes.h"
#include "boundingSphere.h"
#include "gfxAPIDX10.h"
#include "vertex.h"
#include "spherical.h"

static const unsigned int SUBDIVS = 48;

namespace GFX
{
    Util::FactoryPlant<DebugShape,DebugSphere> DebugSphere::factoryPlant_(DebugShapeFactory::InstancePtr(),DS_SPHERE);

    DebugSphere::DebugSphere()
        :   DebugShape(DS_SPHERE)
    {
    }

    DebugSphere::DebugSphere( const DebugSphere& val )
        :   DebugShape(val)
    {
    }

    DebugSphere::~DebugSphere()
    {
    }

    void DebugSphere::load()
    {
        ID3D10Device* device = API::Instance().Device();
        D3D10_BUFFER_DESC  vertexBufferDesc, indexBufferDesc;
        D3D10_SUBRESOURCE_DATA vertexData, indexData;
        HRESULT result;
    
        vertexCount_ = SUBDIVS * 3;
        Vertex* vertList = new Vertex[vertexCount_];
        updateInfo(vertList);

        // Set up the description of the static vertex buffer.
        vertexBufferDesc.Usage               = D3D10_USAGE_DYNAMIC;
        vertexBufferDesc.ByteWidth           = sizeof(Vertex) * vertexCount_;
        vertexBufferDesc.BindFlags           = D3D10_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags      = D3D10_CPU_ACCESS_WRITE;
        vertexBufferDesc.MiscFlags           = 0;

        // Give the subresource structure a pointer to the vertex data.
        vertexData.pSysMem = vertList;

        // Now create the vertex buffer.
        result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertBuffer_);
        ENGINE_ASSERT_MSG(!FAILED(result), "Failed to create vertex buffer");

        vertBuffer_->Map(D3D10_MAP_WRITE_DISCARD,0,reinterpret_cast<void**>(&mappedVertexList_));

        for(unsigned int i = 0; i < vertexCount_; ++i)
            mappedVertexList_[i] = vertList[i];

        vertBuffer_->Unmap();

        indexCount_ = SUBDIVS * 6;
        unsigned int* indexList = new unsigned int[indexCount_];

        // X axis
        unsigned int currIndex = 0;
        unsigned int startingIndex = 0;
        for(unsigned int axis = 0; axis < 3; ++axis)
        {
            for(unsigned int i = startingIndex; i < startingIndex + SUBDIVS; ++i, ++currIndex)
            {
                indexList[currIndex]   = i;
                indexList[++currIndex] = startingIndex + (i + 1) % SUBDIVS;
            }
            startingIndex += SUBDIVS;
        }
        
        // Set up the description of the static index buffer for each strip
        indexBufferDesc.Usage               = D3D10_USAGE_DEFAULT;
        indexBufferDesc.BindFlags           = D3D10_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags      = 0;
        indexBufferDesc.MiscFlags           = 0;
        indexBufferDesc.ByteWidth = sizeof(unsigned int) * indexCount_;

        // Give the subresource structure a pointer to the index data.
        indexData.pSysMem = indexList;

        // Create the index buffer.
        result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer_);
        ENGINE_ASSERT_MSG(!FAILED(result), "Failed to create index buffer");

        if(vertList)
        {
            delete [] vertList;
            vertList = 0;
        }

        delete [] indexList;
        indexList = 0;
    }

    void DebugSphere::update()
    {
        updateInfo(mappedVertexList_);
    }

    void DebugSphere::unload()
    {
    }

    void DebugSphere::updateInfo( Vertex*& list )
    {
        const Phy::BoundingSphere* sphere = reinterpret_cast<const Phy::BoundingSphere*>(shape_);
        Math::SphericalF spherical;
        spherical.Radius(sphere->Radius());
        
        unsigned int currIndex = 0;
        Math::DegreeF vertDelta = 360.f/ static_cast<float>(SUBDIVS);
        Math::DegreeF horizDelta = 360.f/ static_cast<float>(SUBDIVS);
        Math::Vec3F center(sphere->Center());

        // X axis
        spherical.ThetaHoriz(0);
        for(unsigned int y = 0; y < SUBDIVS; ++currIndex, ++y)
        {
            spherical.ThetaVert(Math::DegreeF(static_cast<float>(y) * vertDelta.AsDegrees()));
            list[currIndex].pos_ = center + spherical.AsCartesian();
        }

        // Y axis
        spherical.ThetaVert(0);
        for(unsigned int x = 0; x < SUBDIVS; ++currIndex, ++x)
        {
            spherical.ThetaHoriz(Math::DegreeF(static_cast<float>(x) * horizDelta.AsDegrees()));
            list[currIndex].pos_ = center + spherical.AsCartesian();
        }

        // Z axis
        spherical.ThetaHoriz(Math::DegreeF(90.f));
        for(unsigned int y = 0; y < SUBDIVS; ++currIndex, ++y)
        {
            spherical.ThetaVert(Math::DegreeF(static_cast<float>(y) * vertDelta.AsDegrees()));
            list[currIndex].pos_ = center + spherical.AsCartesian();
        }
    }
}

#endif