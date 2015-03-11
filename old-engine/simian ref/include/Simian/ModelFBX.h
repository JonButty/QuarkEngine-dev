#ifndef SIMIAN_MODEL_H_
#define SIMIAN_MODEL_H_

#if 0
#include "SimianPlatform.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Color.h"
#include "Matrix.h"
#include "VertexFormat.h"

#include <vector>

namespace fbxsdk_2013_1
{
    class FbxNode;
    class FbxScene;
}

namespace Simian
{
    class DataLocation;
    class VertexBuffer;
    class IndexBuffer;
    class GraphicsDevice;

    class Mesh;

    struct SIMIAN_EXPORT ModelVertex
    {
        f32 X, Y, Z;
        f32 NX, NY, NZ;
        u32 Diffuse;
        f32 U, V;
        u8 BoneIndex[4];
        f32 Weight[4];

        static const VertexFormat Format;

        ModelVertex(
            const Simian::Vector3& position = Simian::Vector3::Zero, 
            const Simian::Vector3& normal   = Simian::Vector3::Zero,
            const Simian::Color& color      = Simian::Color(), 
            const Simian::Vector2& uv       = Simian::Vector2::Zero,
            u8* boneIndices                 = 0,
            f32* weights                    = 0)
            : X(position.X()),
              Y(position.Y()),
              Z(position.Z()),
              NX(normal.X()),
              NY(normal.Y()),
              NZ(normal.Z()),
              Diffuse(color.ColorU32()),
              U(uv.X()),
              V(uv.Y())
        {
            if (boneIndices)
                memcpy(BoneIndex, boneIndices, sizeof(u8) * 4);
            if (weights)
                memcpy(Weight, weights, sizeof(f32) * 4);
        }
    };

    class SIMIAN_EXPORT Joint
    {
    private:
        std::string name_;
        Joint* parent_;
        std::vector<Joint*> children_;
        Simian::Matrix local_;
        Simian::Matrix world_;
        u8 index_;
    public:
        std::string Name() const;
        Joint* Parent() const;
        Simian::u8 Index() const;

        const Simian::Matrix& Local() const;
        void Local(const Simian::Matrix& val);
        
        const Simian::Matrix& World() const;
    public:
        Joint(const std::string& name, u8 index);

        void AddJoint(Joint* joint);

        void RecomputeWorld();

        friend class Model;
    };

    class SIMIAN_EXPORT Skeleton
    {
    private:
        Joint* root_;
        std::vector<Joint*> joints_;
    public:
        u32 JointCount() const;
    public:
        Skeleton(Joint* root, const std::vector<Joint*>& joints);
        ~Skeleton();

        Joint* JointByName(const std::string& joint) const;
    };

    class SIMIAN_EXPORT Model
    {
    private:
        GraphicsDevice* device_;
        std::vector<Mesh*> meshes_;
        Skeleton* skeleton_;

        std::vector<fbxsdk_2013_1::FbxNode*> meshNodes_;
        std::vector<fbxsdk_2013_1::FbxNode*> jointRoots_;
    public:
        GraphicsDevice& Device() const;
    private:
        void parseNode_(fbxsdk_2013_1::FbxNode* node);
        void parseMesh_(fbxsdk_2013_1::FbxNode* node);
        void parseJoint_(fbxsdk_2013_1::FbxNode* node, Joint* joint, std::vector<Joint*>* joints);
    public:
        Model(GraphicsDevice* device);

        bool LoadImmediate(const DataLocation& data);
        
        bool LoadFile(const DataLocation& data);
        bool LoadModel();

        void Unload();

        void Draw();
    };

    class SIMIAN_EXPORT Mesh
    {
    private:
        Model* parent_;
        u32 vertexBufferSize_;
        u32 indexBufferSize_;
        void* vertexBufferData_;
        void* indexBufferData_;
        VertexBuffer* vertexBuffer_;
        IndexBuffer* indexBuffer_;
        std::vector<Simian::Matrix> bindPose_;
    public:
        Model* Parent() const;
    public:
        Mesh(Model* model, u32 vertexBufferSize, u32 indexBufferSize, void* vbData, void* ibData, const std::vector<Simian::Matrix>& bindPose);
        Mesh(const Mesh& mesh);
        ~Mesh();

        bool Load();
        void Unload();

        void Draw();
    };
}
#endif

#endif
