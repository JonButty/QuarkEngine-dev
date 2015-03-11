#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_11

#include "meshDX11.h"
#include "vertex.h"
#include "face.h"
#include "halfEdge.h"
#include "gfxLogger.h"
#include "utilAssert.h"
#include "GFXAPIDX11.h"
#include "cameraDX.h"
#include "shaderDX11.h"

namespace GFX
{
    Mesh::Mesh( Vertex*& vertList,
                unsigned int vertCount,
                unsigned int**& indexList,
                std::list<unsigned int>*& stripSizes,
                std::map<unsigned int, std::map<unsigned int,HalfEdge*>* >* edgeList,
                Face*& faceList,
                unsigned int faceCount)
                :   vertList_(vertList),
                    vertCount_(vertCount),
                    indexList_(indexList),
                    stripSizes_(stripSizes),
                    edgeList_(edgeList),
                    faceList_(faceList),
                    faceCount_(faceCount),
                    vertBuffer_(0),
                    indexBuffer_(0)
    {
    }

    Mesh::Mesh( const Mesh& )
    {
    }

    Mesh::~Mesh()
    {
    }

    void Mesh::Load()
    {
        ID3D11Device* device = API::Instance().Device();
        D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
        D3D11_SUBRESOURCE_DATA vertexData, indexData;
        HRESULT result;

        // Set up the description of the static vertex buffer.
        vertexBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.ByteWidth           = sizeof(GFX::Vertex) * vertCount_;
        vertexBufferDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags      = 0;
        vertexBufferDesc.MiscFlags           = 0;
        vertexBufferDesc.StructureByteStride = 0;

        // Give the subresource structure a pointer to the vertex data.
        vertexData.pSysMem          = vertList_;
        vertexData.SysMemPitch      = 0;
        vertexData.SysMemSlicePitch = 0;

        // Now create the vertex buffer.
        result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertBuffer_);
        ENGINE_ASSERT_MSG(!FAILED(result), "Failed to create vertex buffer");

        unsigned int stripCount = stripSizes_->size();
        indexBuffer_ = new ID3D11Buffer* [stripCount];

        // Set up the description of the static index buffer for each strip
        indexBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
        indexBufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags      = 0;
        indexBufferDesc.MiscFlags           = 0;
        indexBufferDesc.StructureByteStride = 0;
        indexData.SysMemPitch      = 0;
        indexData.SysMemSlicePitch = 0;

        std::list<unsigned int>::iterator it = stripSizes_->begin();
        for(unsigned int i = 0; i < stripCount; ++i, ++it)
        {
            indexBufferDesc.ByteWidth = sizeof(unsigned int) * (*it);

            // Give the subresource structure a pointer to the index data.
            indexData.pSysMem = indexList_[i];

            // Create the index buffer.
            result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer_[i]);
            ENGINE_ASSERT_MSG(!FAILED(result), "Failed to create index buffer");
        }

        // Release the arrays now that the vertex and index buffers have been created and loaded.
        if(vertList_)
        {
            delete [] vertList_;
            vertList_ = 0;
        }
        
        if(indexList_ && stripSizes_)
        {
            unsigned int strips = stripSizes_->size();
            for(unsigned int i = 0; i < strips; ++i)
                delete [] indexList_[i];
            delete[] indexList_;
            indexList_ = 0;
        }

        if(faceList_)
        {
            delete faceList_;
            faceList_ = 0;
        }

        if(edgeList_)
        {
            unsigned int listSize = edgeList_->size();
            for(unsigned int i = 0; i < listSize; ++i)
            {
                std::map<unsigned int,HalfEdge*>* list2 = (*edgeList_)[i];
                if(!list2)
                    continue;
                unsigned int edgeCount = list2->size();
                for(unsigned int j = 0; j < edgeCount; ++j)
                {
                    if(!(*list2)[j])
                        continue;
                    delete (*list2)[j];
                    (*list2)[j] = 0;
                }
                delete (*edgeList_)[i];
                (*edgeList_)[i] = 0;
            }
            delete edgeList_;
            edgeList_ = 0;
        }
    }

    void Mesh::Render( Shader* shader,
                       Camera* camera)
    {
        ID3D11DeviceContext* context = API::Instance().DeviceContext();
        unsigned int stripCount = stripSizes_->size();
        std::list<unsigned int>::iterator it = stripSizes_->begin();
        for(unsigned int i = 0; i < stripCount; ++i, ++it)
        {
            unsigned int stride;
            unsigned int offset;

            // Set vertex buffer stride and offset.
            stride = sizeof(Vertex); 
            offset = 0;

            // Set the vertex buffer to active in the input assembler so it can be rendered.
            context->IASetVertexBuffers(0, 1, &vertBuffer_, &stride, &offset);

            // Set the index buffer to active in the input assembler so it can be rendered.
            context->IASetIndexBuffer(indexBuffer_[i], DXGI_FORMAT_R32_UINT, 0);

            // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

            shader->Render(*it,
                           API::Instance().ModelToWorld(),
                           camera->WorldToView(),
                           API::Instance().ViewToPersp());
        }
    }

    void Mesh::Unload()
    {
        // Release the index buffer.
        if(indexBuffer_)
        {
            unsigned int strips = stripSizes_->size();
            for(unsigned int i = 0; i < strips; ++i)
            {
                indexBuffer_[i]->Release();
                indexBuffer_[i] = 0;
            }
            delete [] indexBuffer_;
            indexBuffer_ = 0;
        }

        // Release the vertex buffer.
        if(vertBuffer_)
        {
            vertBuffer_->Release();
            vertBuffer_ = 0;
        }

        if(indexList_ && stripSizes_)
        {
            unsigned int strips = stripSizes_->size();
            for(unsigned int i = 0; i < strips; ++i)
                delete [] indexList_[i];
            delete[] indexList_;
            indexList_ = 0;
            delete stripSizes_;
            stripSizes_ = 0;
        }

        if(vertList_)
        {
            delete vertList_;
            vertList_ = 0;
        }

        if(faceList_)
        {
            delete faceList_;
            faceList_ = 0;
        }

        if(edgeList_)
        {
            unsigned int listSize = edgeList_->size();
            for(unsigned int i = 0; i < listSize; ++i)
            {
                std::map<unsigned int,HalfEdge*>* list2 = (*edgeList_)[i];
                if(!list2)
                    continue;
                unsigned int edgeCount = list2->size();
                for(unsigned int j = 0; j < edgeCount; ++j)
                {
                    if(!(*list2)[j])
                        continue;
                    delete (*list2)[j];
                    (*list2)[j] = 0;
                }
                delete (*edgeList_)[i];
                (*edgeList_)[i] = 0;
            }
            delete edgeList_;
            edgeList_ = 0;
        }
    }
}

#endif