#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_10

#include "debugAABBDX10.h"
#include "debugShapeTypes.h"
#include "gfxAPIDX10.h"
#include "vertex.h"
#include "boundingAABB.h"
#include "boundingVolumeFactory.h"
#include "boundingVolumeTypes.h"

namespace GFX
{
    Util::FactoryPlant<DebugShape,DebugAABB> DebugAABB::factoryPlant_(DebugShapeFactory::InstancePtr(),DS_AABB);

    DebugAABB::DebugAABB()
        :   DebugShape(DS_AABB)
    {
    }

    DebugAABB::DebugAABB( const DebugAABB& val )
        :   DebugShape(val)
    {
    }

    DebugAABB::~DebugAABB()
    {
    }

    //--------------------------------------------------------------------------

    void DebugAABB::load()
    {
        ID3D10Device* device = API::Instance().Device();
        D3D10_BUFFER_DESC  vertexBufferDesc, indexBufferDesc;
        D3D10_SUBRESOURCE_DATA vertexData, indexData;
        HRESULT result;

        vertexCount_ = 8;
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

        indexCount_ = 24;
        unsigned int* indexList = new unsigned int[indexCount_];

        // Bottom            // Top                // Sides
        indexList[0] = 0;    indexList[8]  = 4;    indexList[16] = 4;
        indexList[1] = 1;    indexList[9]  = 5;    indexList[17] = 0;

        indexList[2] = 1;    indexList[10] = 5;    indexList[18] = 5;
        indexList[3] = 2;    indexList[11] = 6;    indexList[19] = 1;

        indexList[4] = 2;    indexList[12] = 6;    indexList[20] = 6;
        indexList[5] = 3;    indexList[13] = 7;    indexList[21] = 2;

        indexList[6] = 3;    indexList[14] = 7;    indexList[22] = 7;
        indexList[7] = 0;    indexList[15] = 4;    indexList[23] = 3;

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

    void DebugAABB::update()
    {
        updateInfo(mappedVertexList_);
    }

    void DebugAABB::unload()
    {
    }

    void DebugAABB::updateInfo( Vertex*& list )
    {
        const Phy::BoundingAABB* box = reinterpret_cast<const Phy::BoundingAABB*>(shape_);
        Math::Vec3F minPos(box->MinPt()), maxPos(box->MaxPt());

        // Bottom
        list[0].pos_ = Math::Vec3F(minPos.X(),minPos.Y(),minPos.Z());
        list[1].pos_ = Math::Vec3F(maxPos.X(),minPos.Y(),minPos.Z());
        list[2].pos_ = Math::Vec3F(maxPos.X(),minPos.Y(),maxPos.Z());
        list[3].pos_ = Math::Vec3F(minPos.X(),minPos.Y(),maxPos.Z());

        // Top
        list[4].pos_ = Math::Vec3F(minPos.X(),maxPos.Y(),minPos.Z());
        list[5].pos_ = Math::Vec3F(maxPos.X(),maxPos.Y(),minPos.Z());
        list[6].pos_ = Math::Vec3F(maxPos.X(),maxPos.Y(),maxPos.Z());
        list[7].pos_ = Math::Vec3F(minPos.X(),maxPos.Y(),maxPos.Z());
    }
}

#endif