#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_10

#include "debugEllipseDX10.h"
#include "gfxAPIDX10.h"
#include "vertex.h"
#include "boundingEllipse.h"
#include "angle.h"
#include "gfxLogger.h"

static const unsigned int SUBDIVS = 48;

namespace GFX
{
    Util::FactoryPlant<DebugShape,DebugEllipse> DebugEllipse::factoryPlant_(DebugShapeFactory::InstancePtr(),DS_ELLIPSE);

    DebugEllipse::DebugEllipse()
        :   DebugShape(DS_ELLIPSE)
    {
    }

    DebugEllipse::DebugEllipse( const DebugEllipse& val )
        :   DebugShape(val)
    {
    }

    DebugEllipse::~DebugEllipse()
    {
    }

    void DebugEllipse::load()
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

    void DebugEllipse::update()
    {
        updateInfo(mappedVertexList_);
    }

    void DebugEllipse::unload()
    {
    }

    void DebugEllipse::updateInfo( Vertex*& list )
    {
        const Phy::BoundingEllipse* ellipse = reinterpret_cast<const Phy::BoundingEllipse*>(shape_);
        float iScale, jScale, kScale;
        iScale = ellipse->I().Length();
        jScale = ellipse->J().Length();
        kScale = ellipse->K().Length();

        Math::Vec3F center = ellipse->Center();

        unsigned int currIndex = 0;
        Math::DegreeF delta = 360.f/ static_cast<float>(SUBDIVS);

        // X axis
        for(unsigned int i = 0; i < SUBDIVS; ++currIndex, ++i)
        {
            Math::Vec3F offset(iScale * std::cos(static_cast<float>(i) * delta.AsRadians()),
                                 jScale * std::sin(static_cast<float>(i) * delta.AsRadians()),
                                 ellipse->I().Z() * iScale);
            list[currIndex].pos_ = center + offset;
        }

        // Y axis
        for(unsigned int i = 0; i < SUBDIVS; ++currIndex, ++i)
        {
            Math::Vec3F offset(ellipse->J().X() * jScale,
                                 jScale * std::sin(static_cast<float>(i) * delta.AsRadians()),
                                 kScale * std::cos(static_cast<float>(i) * delta.AsRadians()));
            list[currIndex].pos_ = center + offset;
        }

        // Z axis
        for(unsigned int i = 0; i < SUBDIVS; ++currIndex, ++i)
        {
            Math::Vec3F offset(iScale * std::cos(static_cast<float>(i) * delta.AsRadians()),
                                 ellipse->K().Y() * kScale,
                                 kScale * std::sin(static_cast<float>(i) * delta.AsRadians()));
            list[currIndex].pos_ = center + offset;
        }
    }
}

#endif