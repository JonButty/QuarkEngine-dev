/************************************************************************/
/*!
\file		Mesh.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_MESH_H_
#define SIMIAN_MESH_H_

#include "SimianPlatform.h"
#include "VertexFormat.h"

namespace Simian
{
    class Model;
    struct MeshData;
    class VertexBuffer;
    class IndexBuffer;
    class AnimationController;
    class RenderQueue;

    struct SIMIAN_EXPORT StaticMeshVertex
    {
        f32 X, Y, Z;
        f32 NX, NY, NZ;
        f32 U, V;

        static const VertexFormat Format;
    };

    struct SIMIAN_EXPORT SkinnedMeshVertex
    {
        f32 X, Y, Z;
        f32 NX, NY, NZ;
        f32 U, V;
        u8 BoneIndex[4];
        f32 BoneWeight[4];

        static const VertexFormat Format;
    };

    class SIMIAN_EXPORT Mesh
    {
    private:
        Model* parent_;
        MeshData* data_;
        VertexBuffer* vertexBuffer_;
        void* vertexData_;
    public:
        Mesh(Model* parent);

        bool LoadData(MeshData* data);
        bool LoadData(s8*& memPtr);
        void CacheData(std::ofstream& file);
        bool Load();
        void Unload();

        void Draw(AnimationController* controller);
    };
}

#endif
