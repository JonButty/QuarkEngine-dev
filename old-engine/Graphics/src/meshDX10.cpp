#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_10

#include "meshDX10.h"
#include "vertex.h"
#include "face.h"
#include "halfEdge.h"
#include "gfxLogger.h"
#include "utilAssert.h"
#include "GFXAPIDX10.h"
#include "cameraDX.h"
#include "shader.h"
#include "shaderTypes.h"
#include "shaderManager.h"
#include "shaderBoundaryDX10.h"
#include "color.h"
#include "debugShape.h"
#include "debugAABB.h"
#include "debugSphere.h"
#include "debugShapeFactory.h"
#include "debugShapeTypes.h"
#include "boundingAABB.h"
#include "boundingSphere.h"
#include "gfxLogger.h"
#include "debugShapeBuilder.h"
#include "aabbTree.h"
#include "boundingSphereTree.h"

namespace GFX
{
    const D3D_PRIMITIVE_TOPOLOGY Mesh::RenderMethodArray[RM_TOTAL] = 
    {
        D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
        D3D10_PRIMITIVE_TOPOLOGY_LINELIST,
        D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP,
        D3D10_PRIMITIVE_TOPOLOGY_POINTLIST
    };
    
    Mesh::Mesh( unsigned int type,
                Vertex*& vertList,
                unsigned int vertCount,
                unsigned int**& indexList,
                std::list<unsigned int>*& stripSizes,
                unsigned int* boundaryIndexList,
                unsigned int boundaryIndexCount,
                RenderMethod method)
                :   renderBoundaryEdges_(false),
                    renderMesh_(true),
                    vertList_(vertList),
                    vertCount_(vertCount),
                    indexList_(indexList),
                    boundaryIndexList_(boundaryIndexList),
                    stripSizes_(stripSizes),
                    vertBuffer_(0),
                    indexBuffer_(0),
                    boundaryIndexBuffer_(0),
                    boundaryIndexCount_(boundaryIndexCount),
                    type_(type),
                    renderMethod_(method),
                    currBVLevel_(0)
    {
    }

    Mesh::Mesh( const Mesh& )
    {
    }

    Mesh::~Mesh()
    {
    }

    void Mesh::Load(bool generateBV)
    {
        ID3D10Device* device = API::Instance().Device();
        D3D10_BUFFER_DESC  vertexBufferDesc, indexBufferDesc;
        D3D10_SUBRESOURCE_DATA vertexData, indexData;
        HRESULT result;

        currBVLevel_ = 0;

        // Set up the description of the static vertex buffer.
        vertexBufferDesc.Usage               = D3D10_USAGE_IMMUTABLE;
        vertexBufferDesc.ByteWidth           = sizeof(Vertex) * vertCount_;
        vertexBufferDesc.BindFlags           = D3D10_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags      = 0;
        vertexBufferDesc.MiscFlags           = 0;
        vertexData.pSysMem                   = vertList_;

        // Now create the vertex buffer.
        result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertBuffer_);
        ENGINE_ASSERT_MSG(!FAILED(result), "Failed to create vertex buffer");

        unsigned int stripCount = stripSizes_->size();
        indexBuffer_ = new ID3D10Buffer* [stripCount];

        // Set up the description of the static index buffer for each strip
        indexBufferDesc.Usage               = D3D10_USAGE_DEFAULT;
        indexBufferDesc.BindFlags           = D3D10_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags      = 0;
        indexBufferDesc.MiscFlags           = 0;

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
        
        if(boundaryIndexCount_ > 0)
        {
            indexData.pSysMem = boundaryIndexList_;
            indexBufferDesc.ByteWidth = sizeof(unsigned int) * boundaryIndexCount_;
            result = device->CreateBuffer(&indexBufferDesc, &indexData, &boundaryIndexBuffer_);
            ENGINE_ASSERT_MSG(!FAILED(result), "Failed to create boundary index buffer");
        }
        
        if(generateBV)
        {
            bvLevels_[BVL_AABB] = DebugShapeBuilder::Instance().BuildAABB(vertList_,vertCount_);
            bvLevels_[BVL_SPHERE_CENTROID] = DebugShapeBuilder::Instance().BuildCentroidSphere(vertList_,vertCount_);
            bvLevels_[BVL_SPHERE_RITTER] = DebugShapeBuilder::Instance().BuildRitterSphere(vertList_,vertCount_);
            bvLevels_[BVL_SPHERE_LARSSON] = DebugShapeBuilder::Instance().BuildLarssonSphere(vertList_,vertCount_);
            bvLevels_[BVL_SPHERE_PCA] = DebugShapeBuilder::Instance().BuildPCASphere(vertList_,vertCount_);
            bvLevels_[BVL_OBB_PCA] = DebugShapeBuilder::Instance().BuildPCAOBB(vertList_,vertCount_);
            bvLevels_[BVL_AABB_PCA] = DebugShapeBuilder::Instance().BuildPCAAABB(vertList_,vertCount_);
            bvLevels_[BVL_ELLIPSE_PCA] = DebugShapeBuilder::Instance().BuildPCAEllipse(vertList_,vertCount_);
        }
        else
        {
            bvLevels_[BVL_AABB] = 0;
            bvLevels_[BVL_SPHERE_CENTROID] = 0;
            bvLevels_[BVL_SPHERE_RITTER] = 0;
            bvLevels_[BVL_SPHERE_LARSSON] = 0;
            bvLevels_[BVL_SPHERE_PCA] = 0;
            bvLevels_[BVL_OBB_PCA] = 0;
            bvLevels_[BVL_AABB_PCA] = 0;
            bvLevels_[BVL_ELLIPSE_PCA] = 0;
        }

        if(vertList_)
        {
            delete [] vertList_;
            vertList_ = 0;
        }

        if(boundaryIndexList_)
        {
            delete [] boundaryIndexList_;
            boundaryIndexList_ = 0;
        }
        
        ShaderManager::Instance().GetShader(ST_BOUNDARY,boundaryShader_);
    }

    void Mesh::LoadBVs(Math::Vec3F* offsets,
                       unsigned int offsetCount)
    {
        bool noOffsetGiven = false;
        if(!offsets || (offsetCount == 0))
        {
            noOffsetGiven = true;
            offsets = new Math::Vec3F(Math::Vec3F::Zero);
            offsetCount = 1;
        }

        const Phy::BoundingAABB* aabb = reinterpret_cast<const Phy::BoundingAABB*>(bvLevels_[BVL_AABB]->Shape());
        const Phy::BoundingSphere* sphere = reinterpret_cast<const Phy::BoundingSphere*>(bvLevels_[BVL_SPHERE_RITTER]->Shape());

        // Loop through each position translation
        for(unsigned int i = 0; i < offsetCount; ++i)
        {
            Math::Vec3F offsetVec(offsets[i].X(),offsets[i].Y(),offsets[i].Z());
            Phy::BoundingAABB* newAABB = new Phy::BoundingAABB(*aabb);
            newAABB->Center(newAABB->Center() + offsetVec);
            
            Phy::BoundingSphere* newSphere = new Phy::BoundingSphere(*sphere);
            newSphere->Center(newSphere->Center() + offsetVec);

            Phy::AABBTree::Instance().PushBoundingVolume(newAABB);
            Phy::BoundingSphereTree::Instance().PushBoundingVolume(newSphere);
        }
    }

    void Mesh::Render( Shader* shader,
                       Camera* camera,
                       Math::Vec3F* offsets,
                       unsigned int offsetCount)
    {
        ID3D10Device* device = API::Instance().Device();
        D3DXMATRIX modelToWorld = API::Instance().ModelToWorld();
        D3DXMATRIX worldToView  = camera->WorldToView();
        D3DXMATRIX viewToPersp  = API::Instance().ViewToPersp();
        D3DXMATRIX translation;

        if(renderMesh_)
        {
            std::list<unsigned int>::iterator it = stripSizes_->begin();
            device->IASetPrimitiveTopology(RenderMethodArray[renderMethod_]);

            bool noOffsetGiven = false;
            if(!offsets || (offsetCount == 0))
            {
                noOffsetGiven = true;
                offsets = new Math::Vec3F(Math::Vec3F::Zero);
                offsetCount = 1;
            }

            for(unsigned int i = 0; i < stripSizes_->size(); ++i, ++it)
            {
                // Set the index buffer to active in the input assembler so it can be rendered.
                device->IASetIndexBuffer(indexBuffer_[i], DXGI_FORMAT_R32_UINT, 0);

                // Loop through each position translation
                for(unsigned int j = 0; j < offsetCount; ++j)
                {
                    D3DXMatrixTranslation(&translation,offsets[j].X(), offsets[j].Y(), offsets[j].Z());
                    
                    shader->Render(*it,
                                   modelToWorld,
                                   translation,
                                   worldToView,
                                   viewToPersp);
                }
            }

            if(noOffsetGiven)
                delete offsets;
        }

        if(renderBoundaryEdges_ && boundaryIndexBuffer_)
        {
            device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
            device->IASetIndexBuffer(boundaryIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);
            
            // Loop through each position translation
            for(unsigned int j = 0; j < offsetCount; ++j)
            {
                D3DXMatrixTranslation(&translation,offsets[j].X(), offsets[j].Y(), offsets[j].Z());
                boundaryShader_->Render(boundaryIndexCount_,
                                        modelToWorld,
                                        translation,
                                        worldToView,
                                        viewToPersp);
            }
        }
    }
    
    void Mesh::RenderBoundingVolume( Camera* camera,
                                     Math::Vec3F* positions,
                                     unsigned int positionCount)
    {
        if(bvLevels_[currBVLevel_])
            bvLevels_[currBVLevel_]->Render(camera,positions, positionCount);
    }

    void Mesh::Unload()
    {
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

        if(boundaryIndexBuffer_)
        {
            boundaryIndexBuffer_->Release();
            boundaryIndexBuffer_ = 0;
        }

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
            delete [] vertList_;
            vertList_ = 0;
        }

        if(boundaryIndexList_)
        {
            delete [] boundaryIndexList_;
            boundaryIndexList_ = 0;
        }

        for(unsigned int i = 0; i < BVL_TOTAL; ++i)
        {
            delete bvLevels_[i];
            bvLevels_[i] = 0;
        }
    }

    void Mesh::RenderSetup()
    {
        ID3D10Device* device = API::Instance().Device();
        unsigned int stride = sizeof(Vertex);
        unsigned int offset = 0;

        device->IASetVertexBuffers(0, 1, &vertBuffer_, &stride, &offset);
    }

    void Mesh::RenderBoundingVolumeSetup()
    {
        bvLevels_[currBVLevel_]->RenderSetup();
    }

    //--------------------------------------------------------------------------
    
    void Mesh::RenderMesh( bool val )
    {
        renderMesh_ = val;
    }

    bool Mesh::RenderMesh() const
    {
        return renderMesh_;
    }

    void Mesh::RenderBoundaryEdges( bool val )
    {
        renderBoundaryEdges_ = val;
    }

    bool Mesh::RenderBoundaryEdges() const
    {
        return renderBoundaryEdges_;
    }

    void Mesh::BVLevel(unsigned int val) 
    {
        currBVLevel_ = val;
    }

    unsigned int Mesh::BVLevel() const
    {
        return currBVLevel_;
    }
}
    
#endif