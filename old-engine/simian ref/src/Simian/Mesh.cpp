/************************************************************************/
/*!
\file		Mesh.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Mesh.h"
#include "Simian/Model.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/VertexBuffer.h"
#include "Simian/Math.h"
#include "Simian/ShaderProgram.h"

namespace Simian
{
    template <class T>
    static T extractBinary_(s8*& memPtr, u32 num = 1)
    {
        T ret = *((T*)memPtr);
        ((T*&)memPtr) += num;
        return ret;
    }

    template <class T, class U>
    static void extractChunk_(T* dest, U& src, u32 size)
    {
        memcpy(dest, src, sizeof(T) * size);
        src += sizeof(T) * size;
    }

    //--------------------------------------------------------------------------

    static VertexFormat staticMeshVertexFormat_()
    {
        VertexFormat format;
        
        format.AddAttribute(Simian::VertexAttribute::VFU_POSITION, Simian::VertexAttribute::VFT_FLOAT3);
        format.AddAttribute(Simian::VertexAttribute::VFU_NORMAL, Simian::VertexAttribute::VFT_FLOAT3);
        format.AddAttribute(Simian::VertexAttribute::VFU_TEXTURE, Simian::VertexAttribute::VFT_FLOAT2);

        return format;
    }

    static VertexFormat skinnedMeshVertexFormat_()
    {
        VertexFormat format;

        format.AddAttribute(Simian::VertexAttribute::VFU_POSITION, Simian::VertexAttribute::VFT_FLOAT3);
        format.AddAttribute(Simian::VertexAttribute::VFU_NORMAL, Simian::VertexAttribute::VFT_FLOAT3);
        format.AddAttribute(Simian::VertexAttribute::VFU_TEXTURE, Simian::VertexAttribute::VFT_FLOAT2);
        format.AddAttribute(Simian::VertexAttribute::VFU_BONEINDEX, Simian::VertexAttribute::VFT_UBYTE4);
        format.AddAttribute(Simian::VertexAttribute::VFU_BONEWEIGHT, Simian::VertexAttribute::VFT_FLOAT4);

        return format;
    }

    const VertexFormat StaticMeshVertex::Format(staticMeshVertexFormat_());
    const VertexFormat SkinnedMeshVertex::Format(skinnedMeshVertexFormat_());

    Mesh::Mesh(Model* parent)
        : parent_(parent),
          data_(0),
          vertexBuffer_(0),
          vertexData_(0)
    {
    }

    bool Mesh::LoadData( MeshData* data )
    {
        data_ = data;

        const VertexFormat& format = data_->Skinned ? SkinnedMeshVertex::Format : StaticMeshVertex::Format;

        vertexData_ = new u8[format.Size() * data_->IndexData.size()];
        if (data_->Skinned)
        {
            std::vector<SkinnedMeshVertex> verts;

            for (u32 i = 0; i < data_->IndexData.size(); ++i)
            {
                IndexValues& values = data_->IndexData[i];
                SkinnedMeshVertex vert;

                Simian::Vector3& pos = data_->Positions[values.Position].Position;

                vert.X = pos.X();
                vert.Y = pos.Y();
                vert.Z = pos.Z();

                parent_->min_.X(Simian::Math::Min(parent_->min_.X(), pos.X()));
                parent_->min_.Y(Simian::Math::Min(parent_->min_.Y(), pos.Y()));
                parent_->min_.Z(Simian::Math::Min(parent_->min_.Z(), pos.Z()));

                parent_->max_.X(Simian::Math::Max(parent_->max_.X(), pos.X()));
                parent_->max_.Y(Simian::Math::Max(parent_->max_.Y(), pos.Y()));
                parent_->max_.Z(Simian::Math::Max(parent_->max_.Z(), pos.Z()));

                memset(vert.BoneIndex, 0, sizeof(u8) * 4);
                memset(vert.BoneWeight, 0, sizeof(f32) * 4);

                u32 numWeights = data_->Positions[values.Position].Weights.size();
                numWeights = numWeights > 4 ? 4 : numWeights;
                f32 sumOfWeights = 0.0f;
                for (u32 j = 0; j < numWeights; ++j)
                {
                    vert.BoneIndex[j] = data_->Positions[values.Position].Weights[j].first;
                    vert.BoneWeight[j] = data_->Positions[values.Position].Weights[j].second;
                    sumOfWeights += data_->Positions[values.Position].Weights[j].second;
                }

                // normalize weights
                f32 invWeight = 1.0f/(sumOfWeights ? sumOfWeights : 1.0f);
                for (u32 j = 0; j < numWeights; ++j)
                    vert.BoneWeight[j] *= invWeight;

                vert.NX = data_->Normals[values.Normal].X();
                vert.NY = data_->Normals[values.Normal].Y();
                vert.NZ = data_->Normals[values.Normal].Z();

                vert.U = data_->UVs[values.UV].X();
                vert.V = 1.0f - data_->UVs[values.UV].Y();

                verts.push_back(vert);
            }

            memcpy(vertexData_, &verts[0], format.Size() * verts.size());
        }
        else
        {
            std::vector<StaticMeshVertex> verts;

            for (u32 i = 0; i < data_->IndexData.size(); ++i)
            {
                IndexValues& values = data_->IndexData[i];
                StaticMeshVertex vert;

                Simian::Vector3& pos = data_->Positions[values.Position].Position;

                vert.X = pos.X();
                vert.Y = pos.Y();
                vert.Z = pos.Z();

                parent_->min_.X(Simian::Math::Min(parent_->min_.X(), pos.X()));
                parent_->min_.Y(Simian::Math::Min(parent_->min_.Y(), pos.Y()));
                parent_->min_.Z(Simian::Math::Min(parent_->min_.Z(), pos.Z()));

                parent_->max_.X(Simian::Math::Max(parent_->max_.X(), pos.X()));
                parent_->max_.Y(Simian::Math::Max(parent_->max_.Y(), pos.Y()));
                parent_->max_.Z(Simian::Math::Max(parent_->max_.Z(), pos.Z()));

                vert.NX = data_->Normals[values.Normal].X();
                vert.NY = data_->Normals[values.Normal].Y();
                vert.NZ = data_->Normals[values.Normal].Z();

                vert.U = data_->UVs[values.UV].X();
                vert.V = 1.0f - data_->UVs[values.UV].Y();

                verts.push_back(vert);
            }

            memcpy(vertexData_, &verts[0], format.Size() * verts.size());
        }

        return true;
    }

    bool Mesh::LoadData( s8*& memPtr )
    {
        // add mesh data to parent
        MeshData* meshData = new MeshData();
        data_ = meshData;
        parent_->meshData_.push_back(meshData);

        // set skinned
        meshData->Skinned = extractBinary_<s8>(memPtr) ? true : false;

        // extract the vertex data
        u32 size = extractBinary_<u32>(memPtr);
        meshData->IndexData.resize(size);
        size = extractBinary_<u32>(memPtr);
        vertexData_ = new u8[size];
        extractChunk_((s8*)vertexData_, memPtr, size);

        return true;
    }

    void Mesh::CacheData( std::ofstream& file )
    {
        s8 skinned = data_->Skinned ? 1 : 0;
        file.write(&skinned, sizeof(s8));

        const VertexFormat& format = data_->Skinned ? SkinnedMeshVertex::Format : StaticMeshVertex::Format;

        u32 size = data_->IndexData.size();
        file.write((s8*)&size, sizeof(u32));
        size = format.Size() * data_->IndexData.size();
        file.write((s8*)&size, sizeof(u32));
        file.write((s8*)vertexData_, size);
    }

    bool Mesh::Load()
    {
        parent_->Device().CreateVertexBuffer(vertexBuffer_);

        const VertexFormat& format = data_->Skinned ? SkinnedMeshVertex::Format : StaticMeshVertex::Format;
        vertexBuffer_->Load(format, data_->IndexData.size());
        vertexBuffer_->Data(vertexData_);
        delete [] vertexData_;
        vertexData_ = 0;

        return true;
    }

    void Mesh::Unload()
    {
        vertexBuffer_->Unload();
        parent_->Device().DestroyVertexBuffer(vertexBuffer_);
    }

    void Mesh::Draw(AnimationController* controller)
    {
        if (controller && parent_->vertexShader_ && data_->Skinned)
        {
            // extract details set vertex shader then draw
            parent_->vertexShader_->Set();
            parent_->Device().Draw(vertexBuffer_, GraphicsDevice::PT_TRIANGLES, data_->IndexData.size()/3);
            parent_->vertexShader_->Unset();
        }
        else    // draw in bind pose
            parent_->Device().Draw(vertexBuffer_, GraphicsDevice::PT_TRIANGLES, data_->IndexData.size()/3);
    }
}
