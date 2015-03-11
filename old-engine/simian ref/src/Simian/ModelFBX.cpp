#if 0
#include "Simian/Model.h"
#include "Simian/DataLocation.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/VertexBuffer.h"
#include "Simian/IndexBuffer.h"
#include "Simian/LogManager.h"

#include "fbxsdk.h"

#include <sstream>

namespace Simian
{
    static VertexFormat makeModelVertexFormat_()
    {
        VertexFormat format;

        format.AddAttribute(VertexAttribute::VFU_POSITION, VertexAttribute::VFT_FLOAT3);
        format.AddAttribute(VertexAttribute::VFU_NORMAL, VertexAttribute::VFT_FLOAT3);
        format.AddAttribute(VertexAttribute::VFU_COLOR, VertexAttribute::VFT_COLOR);
        format.AddAttribute(VertexAttribute::VFU_TEXTURE, VertexAttribute::VFT_FLOAT2);
        format.AddAttribute(VertexAttribute::VFU_BONEINDEX, VertexAttribute::VFT_UBYTE4);
        format.AddAttribute(VertexAttribute::VFU_BONEWEIGHT, VertexAttribute::VFT_FLOAT4);

        return format;
    }

    const VertexFormat ModelVertex::Format(makeModelVertexFormat_());

    static void makeSimianMatrix_(const FbxMatrix& matrix, Simian::Matrix& out)
    {
        f32 mat[16];
        for (int y = 0; y < 4; ++y)
            for (int x = 0; x < 4; ++x)
                mat[y * 4 + x] = static_cast<f32>(matrix.Get(y, x));
        out = Simian::Matrix(mat);
    }

    void getGlobalTransform_(FbxNode* node, FbxMatrix& out)
    {
        out = node->GetScene()->GetEvaluator()->GetNodeGlobalTransform(node);
    }

    void getLocalTransform_(FbxNode* node, FbxMatrix& out)
    {
        out = node->GetScene()->GetEvaluator()->GetNodeLocalTransform(node);
    }

    Model::Model(GraphicsDevice* device)
        : device_(device),
          skeleton_(0)
    {
    }

    //--------------------------------------------------------------------------

    GraphicsDevice& Model::Device() const
    {
        return *device_;
    }

    //--------------------------------------------------------------------------

    void Model::parseNode_(FbxNode* node)
    {
        // parse nodes recursively
        if (node)
        {
            // detect the node's type
            std::string type = node->GetTypeName();
            if (type == "Mesh")
                meshNodes_.push_back(node);
            else if (type == "LimbNode")
            {
                jointRoots_.push_back(node);
                return; // consume children
            }

            // load children
            for (s32 i = 0; i < node->GetChildCount(); ++i)
            {
                FbxNode* child = node->GetChild(i);
                parseNode_(child);
            }
        }
    }

    template <class T, class U>
    T GetElement(U* elementList, int index)
    {
        u32 mindex = 0;
        if (elementList->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
            mindex = index;
        else if (elementList->GetMappingMode() == FbxLayerElement::eByControlPoint)
            mindex = index;

        if (elementList->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
            mindex = elementList->GetIndexArray()[mindex];

        return elementList->GetDirectArray().GetAt(mindex);
    }

    void Model::parseMesh_( fbxsdk_2013_1::FbxNode* node )
    {
        FbxMesh* mesh = node->GetMesh();

        //u32 vertexCount = mesh->GetControlPointsCount();
        u32 vertexCount = mesh->GetPolygonCount() * 3;
        u32 indexCount = mesh->GetPolygonCount() * 3;

        std::vector<Simian::ModelVertex> vertices;
        vertices.reserve(vertexCount);

        std::vector<u16> indices;
        indices.reserve(indexCount);

        FbxVector4* controlPoints = mesh->GetControlPoints();

        FbxLayerElementNormal* normals = mesh->GetElementNormal();
        FbxLayerElementVertexColor* colors = mesh->GetElementVertexColor();
        FbxLayerElementUV* uvs = mesh->GetElementUV();

#if 0
        vertices.resize(vertexCount);
        for (u32 i = 0; i < indexCount; ++i)
        {
            int index = mesh->GetPolygonVertices()[i];
            indices.push_back(static_cast<u16>(index));

            vertices[index].X = static_cast<float>(controlPoints[index][0]);
            vertices[index].Y = static_cast<float>(controlPoints[index][1]);
            vertices[index].Z = static_cast<float>(controlPoints[index][2]);

            if (normals)
            {
                FbxVector4 mnormal = GetElement<FbxVector4>(normals, i);
                vertices[index].NX = static_cast<f32>(mnormal[0]);
                vertices[index].NY = static_cast<f32>(mnormal[1]);
                vertices[index].NZ = static_cast<f32>(mnormal[2]);
            }

            if (colors)
            {
                FbxColor mcolor = GetElement<FbxColor>(colors, i);
                vertices[index].Diffuse = Simian::Color(
                    static_cast<f32>(mcolor.mRed),
                    static_cast<f32>(mcolor.mGreen),
                    static_cast<f32>(mcolor.mBlue),
                    static_cast<f32>(mcolor.mAlpha)).ColorU32();
            }

            if (uvs)
            {
                FbxVector2 muv = GetElement<FbxVector2>(uvs, i);
                vertices[index].U = static_cast<f32>(muv[0]);
                vertices[index].V = 1.0f - static_cast<f32>(muv[1]);
            }
        }
#endif
        std::vector<std::vector<u32> > indexTable(mesh->GetControlPointsCount());

        vertices.resize(indexCount);
        for (u32 i = 0; i < indexCount; ++i)
        {
            int index = mesh->GetPolygonVertices()[i];
            indices.push_back(static_cast<u16>(index));
            indexTable[index].push_back(i);

            vertices[i].X = static_cast<float>(controlPoints[index][0]);
            vertices[i].Y = static_cast<float>(controlPoints[index][1]);
            vertices[i].Z = static_cast<float>(controlPoints[index][2]);

            if (normals)
            {
                FbxVector4 mnormal = GetElement<FbxVector4>(normals, i);
                vertices[i].NX = static_cast<f32>(mnormal[0]);
                vertices[i].NY = static_cast<f32>(mnormal[1]);
                vertices[i].NZ = static_cast<f32>(mnormal[2]);
            }

            if (colors)
            {
                FbxColor mcolor = GetElement<FbxColor>(colors, i);
                vertices[i].Diffuse = Simian::Color(
                    static_cast<f32>(mcolor.mRed),
                    static_cast<f32>(mcolor.mGreen),
                    static_cast<f32>(mcolor.mBlue),
                    static_cast<f32>(mcolor.mAlpha)).ColorU32();
            }

            if (uvs)
            {
                FbxVector2 muv = GetElement<FbxVector2>(uvs, i);
                vertices[i].U = static_cast<f32>(muv[0]);
                vertices[i].V = 1.0f - static_cast<f32>(muv[1]);
            }
        }

        FbxMatrix world;
        getGlobalTransform_(mesh->GetNode(), world);

        std::vector<u8> currentBoneIndex;
        currentBoneIndex.resize(vertexCount, 0);

        std::vector<Simian::Matrix> bindPose(skeleton_->JointCount());

        FbxDeformer* deformer = mesh->GetDeformer(0);
        if (deformer && deformer->GetDeformerType() == FbxDeformer::eSkin)
        {
            FbxSkin* skin = FbxCast<FbxSkin>(deformer);
            for (int i = 0; i < skin->GetClusterCount(); ++i)
            {
                FbxCluster* cluster = skin->GetCluster(i);
                Joint* joint = skeleton_->JointByName(cluster->GetLink()->GetName());
                if (!joint)
                    continue;

                for (int j = 0; j < cluster->GetControlPointIndicesCount(); ++j)
                {
                    u32 index = cluster->GetControlPointIndices()[j];
                    if (currentBoneIndex[index] < 4)
                    {
                        u32 bindex = currentBoneIndex[index]++;

                        for (u32 k = 0; k < indexTable[index].size(); ++k)
                        {
                            u32 vindex = indexTable[index][k];
                            vertices[vindex].BoneIndex[bindex] = joint->Index();
                            vertices[vindex].Weight[bindex] = 
                                static_cast<f32>(skin->GetControlPointBlendWeights()[j]);
                        }
                    }
                }

                FbxAMatrix bindMatrix;
                cluster->GetTransformMatrix(bindMatrix);
                //bindMatrix *= world;

                makeSimianMatrix_(bindMatrix, bindPose[joint->Index()]);
            }
        }

        meshes_.push_back(new Simian::Mesh(this, vertexCount, indexCount, &vertices[0], &indices[0], bindPose));
    }

    void Model::parseJoint_( fbxsdk_2013_1::FbxNode* node, Joint* parent, std::vector<Joint*>* joints)
    {
        if (!parent)
            joints = new std::vector<Joint*>();

        Joint* joint = new Joint(node->GetName(), static_cast<u8>(joints->size()));
        joints->push_back(joint);

        for (int i = 0; i < node->GetChildCount(); ++i)
        {
            FbxNode* child = node->GetChild(i);

            // add any child bones
            if (std::string(child->GetTypeName()) == "LimbNode")
                parseJoint_(child, joint, joints);
        }

        if (!parent)
        {
            skeleton_ = new Skeleton(joint, *joints);
            delete joints;
        }
    }

    //--------------------------------------------------------------------------

    bool Model::LoadImmediate( const DataLocation& data )
    {
        if (!LoadFile(data))
            return false;
        return LoadModel();
    }

    bool Model::LoadFile( const DataLocation& data )
    {
        FbxManager* manager = FbxManager::Create();

        FbxIOSettings* settings = FbxIOSettings::Create(manager, IOSROOT);
        manager->SetIOSettings(settings);

        FbxImporter* importer = FbxImporter::Create(manager, "");

        if (!importer->Initialize(data.Identifier().c_str(), -1, manager->GetIOSettings()))
        {
            manager->Destroy();
            return false;
        }

        FbxScene* scene = FbxScene::Create(manager, "");
        importer->Import(scene);
        importer->Destroy();

        FbxNode* root = scene->GetRootNode();
        parseNode_(root);

        // build skeleton
        if (jointRoots_.size() > 0)
            parseJoint_(jointRoots_[0], 0, 0);

        // then meshes
        for (u32 i = 0; i < meshNodes_.size(); ++i)
            parseMesh_(meshNodes_[i]);

        scene->Destroy();
        manager->Destroy();

        return true;
    }

    bool Model::LoadModel()
    {
        for (u32 i = 0; i < meshes_.size(); ++i)
        {
            if (!meshes_[i]->Load())
            {
                for (u32 j = 0; j < i; ++j)
                {
                    meshes_[i]->Unload();
                    delete meshes_[i];
                }
                meshes_.clear();
                return false;
            }
        }
        return true;
    }

    void Model::Unload()
    {
        for (u32 i = 0; i < meshes_.size(); ++i)
        {
            meshes_[i]->Unload();
            delete meshes_[i];
        }
        meshes_.clear();

        delete skeleton_;
        skeleton_ = 0;
    }

    void Model::Draw()
    {
        for (u32 i = 0; i < meshes_.size(); ++i)
            meshes_[i]->Draw();
    }

    //--------------------------------------------------------------------------

    Mesh::Mesh( Model* model, u32 vertexBufferSize, u32 indexBufferSize, void* vbData, void* ibData, const std::vector<Simian::Matrix>& bindPose )
        : parent_(model),
          vertexBufferSize_(vertexBufferSize),
          indexBufferSize_(indexBufferSize),
          vertexBufferData_(new u8[ModelVertex::Format.Size() * vertexBufferSize]),
          indexBufferData_(new u8[sizeof(u16) * indexBufferSize]),
          vertexBuffer_(0),
          indexBuffer_(0),
          bindPose_(bindPose)
    {
        memcpy(vertexBufferData_, vbData, ModelVertex::Format.Size() * vertexBufferSize);
        memcpy(indexBufferData_, ibData, sizeof(u16) * indexBufferSize);
    }

    Mesh::Mesh( const Mesh& mesh )
        : parent_(mesh.parent_),
          vertexBufferSize_(mesh.vertexBufferSize_),
          indexBufferSize_(mesh.indexBufferSize_),
          vertexBufferData_(new u8[mesh.vertexBufferSize_]),
          indexBufferData_(new u8[mesh.indexBufferSize_]),
          vertexBuffer_(0),
          indexBuffer_(0)
    {
        memcpy(vertexBufferData_, mesh.vertexBufferData_, ModelVertex::Format.Size() * vertexBufferSize_);
        memcpy(indexBufferData_, mesh.indexBufferData_, sizeof(u16) * indexBufferSize_);
    }

    Mesh::~Mesh()
    {
        delete [] indexBufferData_;
        indexBufferData_ = 0;

        delete [] vertexBufferData_;
        vertexBufferData_ = 0;
    }

    //--------------------------------------------------------------------------
    
    Model* Mesh::Parent() const
    {
        return parent_;
    }

    //--------------------------------------------------------------------------

    bool Mesh::Load()
    {
        if (!parent_->Device().CreateVertexBuffer(vertexBuffer_))
            return false;
        if (!vertexBuffer_->Load(Simian::ModelVertex::Format, vertexBufferSize_, true))
        {
            parent_->Device().DestroyVertexBuffer(vertexBuffer_);
            return false;
        }
        vertexBuffer_->Data(vertexBufferData_);

        if (!parent_->Device().CreateIndexBuffer(indexBuffer_))
        {
            vertexBuffer_->Unload();
            parent_->Device().DestroyVertexBuffer(vertexBuffer_);
            return false;
        }
        if (!indexBuffer_->Load(Simian::IndexBuffer::IBT_U16, indexBufferSize_))
        {
            parent_->Device().DestroyIndexBuffer(indexBuffer_);
            vertexBuffer_->Unload();
            parent_->Device().DestroyVertexBuffer(vertexBuffer_);
            return false;
        }
        indexBuffer_->Data(indexBufferData_);

        delete [] indexBufferData_;
        indexBufferData_ = 0;

        delete [] vertexBufferData_;
        vertexBufferData_ = 0;

        return true;
    }

    void Mesh::Unload()
    {
        indexBuffer_->Unload();
        parent_->Device().DestroyIndexBuffer(indexBuffer_);

        vertexBuffer_->Unload();
        parent_->Device().DestroyVertexBuffer(vertexBuffer_);
    }

    void Mesh::Draw()
    {
#if 0
        parent_->Device().DrawIndexed(vertexBuffer_, indexBuffer_, Simian::GraphicsDevice::PT_TRIANGLES, indexBufferSize_/3);
#endif
        parent_->Device().Draw(vertexBuffer_, Simian::GraphicsDevice::PT_TRIANGLES, indexBufferSize_/3);
    }

    //--------------------------------------------------------------------------

    Joint::Joint( const std::string& name, u8 index )
        : name_(name),
          parent_(0),
          index_(index)
    {
    }

    //--------------------------------------------------------------------------

    std::string Joint::Name() const
    {
        return name_;
    }

    Joint* Joint::Parent() const
    {
        return parent_;
    }

    Simian::u8 Joint::Index() const
    {
        return index_;
    }

    const Simian::Matrix& Joint::Local() const
    {
        return local_;
    }

    void Joint::Local( const Simian::Matrix& val )
    {
        local_ = val;
    }

    const Simian::Matrix& Joint::World() const
    {
        return world_;
    }

    //--------------------------------------------------------------------------

    void Joint::AddJoint( Joint* joint )
    {
        joint->parent_ = this;
        children_.push_back(joint);
    }

    Joint* Skeleton::JointByName( const std::string& joint ) const
    {
        for (std::vector<Joint*>::const_iterator i = joints_.begin(); i != joints_.end(); ++i)
        {
            if ((*i)->Name() == joint)
                return (*i);
        }
        return NULL;
    }

    void Joint::RecomputeWorld()
    {
        world_ = local_;

        if (parent_)
            world_ = parent_->World() * world_;

        for (u32 i = 0; i < children_.size(); ++i)
            children_[i]->RecomputeWorld();
    }

    //--------------------------------------------------------------------------

    Skeleton::Skeleton( Joint* root, const std::vector<Joint*>& joints )
        : root_(root),
          joints_(joints)
    {
    }

    Skeleton::~Skeleton()
    {
        for (u32 i = 0; i < joints_.size(); ++i)
            delete joints_[i];
        joints_.clear();
        root_ = 0;
    }

    //--------------------------------------------------------------------------

    Simian::u32 Skeleton::JointCount() const
    {
        return joints_.size();
    }
}
#endif
