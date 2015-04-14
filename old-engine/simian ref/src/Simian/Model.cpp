/************************************************************************/
/*!
\file		Model.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Model.h"
#include "Simian/Utility.h"
#include "Simian/DataLocation.h"
#include "Simian/Joint.h"
#include "Simian/AnimationKey.h"
#include "Simian/AnimationController.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/LogManager.h"
#include "Simian/ShaderProgram.h"

#include "tinyxml.h"

#include <memory>
#include <algorithm>
#include <sstream>
#include <map>
#include <fstream>

namespace Simian
{
    static TiXmlElement* findDescendant_(TiXmlElement* parent, const std::string& key, const std::string& value)
    {
        for (TiXmlElement* node = parent->FirstChildElement(); node; node = node->NextSiblingElement())
        {
            if (node->Attribute(key.c_str()))
            {
                std::string name = node->Attribute(key.c_str());
                if (name == value.substr(1))
                    return node;
            }
        }
        return NULL;
    }

    static void parseFloatArray_(TiXmlElement* farray, std::vector<f32>& vec)
    {
        std::stringstream ss;
        std::string stotal = farray->Attribute("count");
        ss << stotal;
        u32 total;
        ss >> total;
        ss.clear();
        ss << farray->GetText();
        for (u32 i = 0; i < total; ++i)
        {
            f32 f;
            ss >> f;
            vec.push_back(f);
        }
    }

    static void parseFloatArray_(TiXmlElement* farray, std::vector<MeshVertex>& vec)
    {
        std::stringstream ss;
        std::string stotal = farray->Attribute("count");
        ss << stotal;
        u32 total;
        ss >> total;
        ss.clear();
        ss << farray->GetText();
        for (u32 i = 0; i < total/3; ++i)
        {
            MeshVertex vertex;

            f32 x, y, z;
            ss >> x;
            ss >> y;
            ss >> z;

            vertex.Position = Simian::Vector3(x, y, z);

            vec.push_back(vertex);
        }
    }

    template <int count, class V, class T>
    static void parseFloatArray_(TiXmlElement* farray, std::vector<T>& vec)
    {
        std::stringstream ss;
        std::string stotal = farray->Attribute("count");
        ss << stotal;
        u32 total;
        ss >> total;
        ss.clear();
        ss << farray->GetText();
        for (u32 i = 0; i < total/count; ++i)
        {
            vec.push_back(T());
            for (s32 j = 0; j < count; ++j)
            {
                V val;
                ss >> val;
                vec[i][j] = val;
            }
        }
    }

    static void parseMatrix_(TiXmlElement* matrix, Simian::Matrix& out)
    {
        f32 mat[16];
        std::stringstream ss;
        ss << matrix->GetText();
        for (u32 i = 0; i < 16; ++i)
            ss >> mat[i];
        out = Simian::Matrix(mat);
    }

    static void parseNameArray_(TiXmlElement* nameArray, std::vector<std::string>& names)
    {
        std::stringstream ss;
        ss << nameArray->Attribute("count");
        u32 count;
        ss >> count;
        ss.clear();
        ss << nameArray->GetText();

        for (u32 i = 0; i < count; ++i)
        {
            std::string name;
            ss >> name;
            names.push_back(name);
        }
    }

    static void parseMatrixArray_(TiXmlElement* matrixArray, std::vector<Simian::Matrix>& matrices)
    {
        std::stringstream ss;
        ss << matrixArray->Attribute("count");
        u32 count;
        ss >> count;
        ss.clear();
        ss << matrixArray->GetText();

        for (u32 i = 0; i < count/16; ++i)
        {
            f32 mat[16];
            for (u32 j = 0; j < 16; ++j)
                ss >> mat[j];
            matrices.push_back(Simian::Matrix(mat));
        }
    }

    Model::Model(GraphicsDevice* device)
        : flags_(0),
          device_(device),
          root_(0),
          scene_(0),
          geometry_(0),
          controllers_(0),
          skin_(0),
          animations_(0),
          jointRoot_(0),
          skeleton_(0),
          vertexShader_(0)
    {
    }

    //--------------------------------------------------------------------------

    GraphicsDevice& Model::Device() const
    {
        return *device_;
    }

    const AnimationFrames* Model::AnimationFrame( const std::string& name ) const
    {
        std::map<std::string, AnimationFrames>::const_iterator iter = animationFrames_.find(name);
        if (iter != animationFrames_.end())
            return &iter->second;
        return NULL;
    }

    Skeleton* Model::Skeleton() const
    {
        return skeleton_;
    }

    const Simian::Vector3& Model::Min() const
    {
        return min_;
    }

    const Simian::Vector3& Model::Max() const
    {
        return max_;
    }

    //--------------------------------------------------------------------------

    bool Model::parseScene_()
    {
        for (TiXmlElement* child = scene_->FirstChildElement(); child; child = child->NextSiblingElement())
        {
            std::string type;
            if (child->Attribute("type"))
                type = child->Attribute("type");

            // this is the root joint
            if (type == "JOINT")
            {
                jointRoot_ = child;
                continue;
            }

            TiXmlElement* geometry = child->FirstChildElement("instance_geometry");
            if (geometry)
            {
                geometry = findDescendant_(geometry_, "id", geometry->Attribute("url"));
                if (geometry)
                    parseMesh_(geometry, false);
                continue;
            }

            // get instance_controller
            TiXmlElement* controller = child->FirstChildElement("instance_controller");
            if (controller)
            {
                controller = findDescendant_(controllers_, "id", controller->Attribute("url"));
                if (controller)
                    parseController_(controller);
                continue;
            }
        }

        buildSkeleton_();
        parseAnimations_();

        return true;
    }

    bool Model::parseController_( TiXmlElement* controller )
    {
        // find root
        skin_ = controller->FirstChildElement("skin");

        if (!skin_)
            return false;

        if (!skin_->Attribute("source"))
            return false;

        std::string meshName = skin_->Attribute("source");
        
        TiXmlElement* mesh = findDescendant_(geometry_, "id", meshName);
        if (!mesh)
            return false;
        
        parseMesh_(mesh, true);

        return true;
    }

    bool Model::parseMesh_( TiXmlElement* meshNode, bool skinned )
    {
        meshNode = meshNode->FirstChildElement("mesh");

        if (!meshNode)
            return false;

        // no sources?!
        if (!meshNode->FirstChildElement("source"))
            return false;

        MeshData* data = new MeshData();

        // HACK: name detection here is probably not a good idea
        for (TiXmlElement* source = meshNode->FirstChildElement("source"); source; source = source->NextSiblingElement())
        {
            if (source->Attribute("id"))
            {
                std::string name = source->Attribute("id");
                std::transform(name.begin(), name.end(), name.begin(), tolower);
                if (name.find("position") != std::string::npos)
                    parseFloatArray_(source->FirstChildElement("float_array"), data->Positions);
                else if (name.find("normal") != std::string::npos)
                    parseFloatArray_<3, f32>(source->FirstChildElement("float_array"), data->Normals);
                else if (name.find("uv") != std::string::npos)
                    parseFloatArray_<2, f32>(source->FirstChildElement("float_array"), data->UVs);
            }
        }

        const u8 POSITION = 0x01;
        const u8 NORMAL = 0x02;
        const u8 UV = 0x04;
        u8 vertComponents = 0;
        TiXmlElement* verticesNode = meshNode->FirstChildElement("vertices");
        for (TiXmlElement* input = verticesNode->FirstChildElement(); input; input = input->NextSiblingElement())
        {
            std::string semantic = input->Attribute("semantic") ? input->Attribute("semantic") : "";
            semantic = SToLower(semantic);
            if (semantic == "position")
                vertComponents |= POSITION;
            else if (semantic == "normal")
                vertComponents |= NORMAL;
            else if (semantic == "texcoord")
                vertComponents |= UV;
        }

        // find triangles
        TiXmlElement* triangles = meshNode->FirstChildElement("triangles");
        std::stringstream ss;
        ss << triangles->Attribute("count");
        u32 triCount;
        ss >> triCount;
        ss.clear();

        // HACK: kind of improved it but could be more dynamic.
        triangles = triangles->FirstChildElement("p");
        ss << triangles->GetText();
        for (u32 i = 0; i < triCount * 3; ++i)
        {
            IndexValues value;
            // position is definitely inside
            ss >> value.Position;

            // check if normal is in
            if (vertComponents & NORMAL)
                value.Normal = value.Position;
            else
                ss >> value.Normal;

            // then check if uv is in
            if (vertComponents & UV)
                value.UV = value.Position;
            else
                ss >> value.UV;

            // something when wrong.. bail!
            if (value.Position > data->Positions.size() || value.Normal > data->Normals.size() || value.UV > data->UVs.size())
            {
                delete data;
                return false;
            }

            data->IndexData.push_back(value);
        }

        data->Skinned = skinned;

        // everything should have worked
        meshData_.push_back(data);

        return true;
    }

    Joint* Model::parseJoint_( TiXmlElement* jointNode, std::vector<Joint*>& joints )
    {
        Simian::Matrix bindPose;
        parseMatrix_(jointNode->FirstChildElement("matrix"), bindPose);

        if (!jointNode->Attribute("id"))
            return NULL;

        Joint* ret = new Joint(jointNode->Attribute("id"), joints.size(), bindPose);
        joints.push_back(ret);

        for (TiXmlElement* child = jointNode->FirstChildElement(); child; child = child->NextSiblingElement())
        {
            if (child->Attribute("type") && std::string(child->Attribute("type")) == "JOINT")
            {
                Joint* childJoint = parseJoint_(child, joints);
                if (childJoint)
                    ret->AddChild(childJoint);
            }
        }

        return ret;
    }

    bool Model::buildSkeleton_()
    {
        // must have a skin to build a skeleton
        if (!skin_)
            return false;

        TiXmlElement* root = jointRoot_;

        std::vector<Joint*> joints;
        Joint* rootJoint = parseJoint_(root, joints);

        // we dont actually have a skeleton.. boo!
        if (!rootJoint)
            return false;

        skeleton_ = new Simian::Skeleton(rootJoint, joints);
        if (!skeleton_->Load())
        {
            delete skeleton_;
            skeleton_ = 0;
            return false;
        }

        if (!skin_->FirstChildElement("source"))
        {
            skeleton_->Unload();
            delete skeleton_;
            skeleton_ = 0;
            return false;
        }

        std::vector<std::string> jointNames;
        std::vector<Simian::Matrix> invBindPose;
        std::vector<f32> weights;

        for (TiXmlElement* source = skin_->FirstChildElement("source"); source; source = source->NextSiblingElement())
        {
            if (source->Value() && std::string(source->Value()) == "source" && source->Attribute("id"))
            {
                std::string name = source->Attribute("id");
                std::transform(name.begin(), name.end(), name.begin(), tolower);
                if (name.find("joints") != std::string::npos)
                    parseNameArray_(source->FirstChildElement("Name_array"), jointNames);
                else if (name.find("matrices") != std::string::npos)
                    parseMatrixArray_(source->FirstChildElement("float_array"), invBindPose);
                else if (name.find("weights") != std::string::npos)
                    parseFloatArray_(source->FirstChildElement("float_array"), weights);
            }
        }

        // none of these arrays can be 0..
        if (!jointNames.size() || !invBindPose.size() || !weights.size())
        {
            skeleton_->Unload();
            delete skeleton_;
            skeleton_ = 0;
            return false;
        }

        std::vector<std::vector<std::pair<u32, u32> > > jointData;

        TiXmlElement* weightsNode = skin_->FirstChildElement("vertex_weights");
        {
            std::stringstream vcountSS, vSS;
            vcountSS << weightsNode->FirstChildElement("vcount")->GetText();
            vSS << weightsNode->FirstChildElement("v")->GetText();

            for (u32 i = 0; i < meshData_[0]->Positions.size(); ++i)
            {
                u32 count;
                vcountSS >> count;

                std::vector<std::pair<u32, u32> > vertWeights;
                for (u32 j = 0; j < count; ++j)
                {
                    u32 joint, weight;
                    vSS >> joint;
                    vSS >> weight;
                    vertWeights.push_back(std::make_pair(joint, weight));
                }
                jointData.push_back(vertWeights);
            }
        }

        std::map<std::string, Joint*> jointMap;

        // parse the joint names (and assign invbindposes)
        for (u32 i = 0; i < jointNames.size(); ++i)
        {
            Joint* joint = skeleton_->FindJointByName(jointNames[i]);

            if (joint)
            {
                jointMap[jointNames[i]] = joint;
                joint->invBindPose_ = invBindPose[i];
            }
        }

        // assign weights to vertices
        for (u32 i = 0; i < meshData_.size(); ++i)
        {
            for (u32 j = 0; j < meshData_[i]->Positions.size(); ++j)
            {
                for (u32 k = 0; k < jointData[j].size(); ++k)
                {
                    std::string jointName = jointNames[jointData[j][k].first];
                    std::map<std::string, Joint*>::iterator iter = jointMap.find(jointName);
                    
                    if (iter != jointMap.end())
                    {
                        u8 boneIndex = static_cast<u8>(iter->second->Id());
                        f32 weight = weights[jointData[j][k].second];
                        meshData_[i]->Positions[j].Weights.push_back(std::make_pair(boneIndex, weight));
                    }
                }
            }
        }

        return true;
    }

    bool Model::parseAnimations_()
    {
        if (!skeleton_)
            return false;

        for (TiXmlElement* animation = animations_->FirstChildElement(); animation; animation = animation->NextSiblingElement())
        {
            if (!animation->Attribute("name"))
                continue;
            std::string name = animation->Attribute("name");
            
            // look for a joint
            Joint* joint = skeleton_->FindJointByName(name);

            if (!joint)
                continue;
            
            // get the inside animation node
            TiXmlElement* currentAnimation = animation->FirstChildElement("animation");

            if (!currentAnimation)
                continue;

            std::vector<f32> times_;
            std::vector<Simian::Matrix> matrices_;

            // find sources
            for (TiXmlElement* source = currentAnimation->FirstChildElement("source"); source; source = source->NextSiblingElement())
            {
                if (source->Value() && std::string(source->Value()) == "source" && source->Attribute("id"))
                {
                    std::string id = source->Attribute("id");
                    std::transform(id.begin(), id.end(), id.begin(), tolower);
                    if (id.find("matrix") != std::string::npos)
                    {
                        if (id.find("input") != std::string::npos)
                            parseFloatArray_(source->FirstChildElement("float_array"), times_);
                        else if (id.find("output") != std::string::npos)
                            parseMatrixArray_(source->FirstChildElement("float_array"), matrices_);
                    }
                }
            }

            for (u32 i = 0; i < times_.size(); ++i)
                joint->keyFrames_.push_back(AnimationKey(times_[i], matrices_[i]));
            std::sort(joint->keyFrames_.begin(), joint->keyFrames_.end());
        }
        return true;
    }

    bool Model::loadAnimationFrames_( const DataLocation& animationFile )
    {
        TiXmlDocument doc;

        doc.Parse(std::string(animationFile.Memory(), animationFile.Memory() + animationFile.Size()).c_str());
        if (doc.Error())
            return false;

        // TODO: change this to serializer when it is done
        TiXmlElement* root = doc.RootElement();

        if (!root)
            return false;

        TiXmlElement* animations = root->FirstChildElement("Animations");

        if (!animations)
            return false;

        for (TiXmlElement* animation = animations->FirstChildElement(); animation; animation = animation->NextSiblingElement())
        {
            std::stringstream ss;
            AnimationFrames animationFrames;

            if (animation->FirstChildElement("Name"))
                animationFrames.Name = animation->FirstChildElement("Name")->GetText();
            else
                continue;

            if (animation->FirstChildElement("Start"))
            {
                ss << animation->FirstChildElement("Start")->GetText();
                ss >> animationFrames.Start;
                --animationFrames.Start;
                ss.clear();
            }
            else
                continue;

            if (animation->FirstChildElement("End"))
            {
                ss << animation->FirstChildElement("End")->GetText();
                ss >> animationFrames.End;
                --animationFrames.End;
                ss.clear();
            }
            else
                continue;

            animationFrames.Fps = 24;
            if (animation->FirstChildElement("FPS"))
            {
                ss << animation->FirstChildElement("FPS")->GetText();
                ss >> animationFrames.Fps;
                ss.clear();
            }

            animationFrames_[animationFrames.Name] = animationFrames;
        }

        return true;
    }

    void Model::drawRenderQueue_( DelegateParameter& param )
    {
        AnimationController* controller = reinterpret_cast<AnimationController*>(
            param.As<RenderObjectDrawParameter>().Object->UserData);
        Draw(controller);
    }

    //--------------------------------------------------------------------------

    bool Model::LoadImmediate( const DataLocation& data )
    {
        if (!LoadFile(data))
            return false;
        if (!LoadModel())
        {
            UnloadFile();
            return false;
        }
        return true;
    }

    bool Model::LoadFile( const DataLocation& data, const DataLocation& animationFile )
    {
        // check if data is a bin file
        if (SToLower(data.Identifier().substr(data.Identifier().length() - 3)) == "bin")
            return LoadBinaryFile(data, animationFile);

        // try to open binary equivalent
        DataLocation binary(data.Identifier() + ".bin");
        if (binary)
            return LoadBinaryFile(binary, animationFile);

        // unable to open the file...
        if (!data)
            return false;

        // all else failed load normal xml
        TiXmlDocument doc;

        doc.Parse(std::string(data.Memory(), data.Memory() + data.Size()).c_str());

        // check if there was an error
        if (doc.Error())
            return false;

        // parse the document
        TiXmlElement* root = doc.RootElement();

        // load the root node
        if (!root)
            return false;

        scene_ = root->FirstChildElement("library_visual_scenes");

        // must have a scene
        if (!scene_)
            return false;

        scene_ = scene_->FirstChildElement("visual_scene");

        if (!scene_)
            return false;

        controllers_ = root->FirstChildElement("library_controllers");

        geometry_ = root->FirstChildElement("library_geometries");

        animations_ = root->FirstChildElement("library_animations");
        
        if (!parseScene_())
            return false;

        // load animation frames
        if (animationFile && !loadAnimationFrames_(animationFile))
            return false;

        // prepare the min and max limits
        if (meshData_.size() && meshData_[0]->Positions.size())
        {
            min_ = meshData_[0]->Positions[0].Position;
            max_ = min_;
        }

        // load the data for constructing a mesh on the gfx card
        for (u32 i = 0; i < meshData_.size(); ++i)
        {
            Mesh* mesh = new Mesh(this);
            if (!mesh->LoadData(meshData_[i]))
            {
                delete mesh;
                continue;
            }
            meshes_.push_back(mesh);
        }

        // normalize min and max
        if (skeleton_)
        {
            Matrix world = skeleton_->Root()->BindPose() * 
                Matrix::Scale(1, 1, -1) * skeleton_->Root()->InvBindPose();
            world.Transform(min_);
            world.Transform(max_);
        }

        // save binary data
#if SIMIAN_MODELCACHING == 1
        CacheBinaryData(data.Identifier() + ".bin");
#endif

        // yay! loaded
        SFlagSet(flags_, MF_FILELOADED);
        return true;
    }

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

    bool Model::LoadBinaryFile( const DataLocation& data, const DataLocation& animationFile)
    {
        // load binary format in data
        s8* memPtr = data.Memory();

        u32 size = extractBinary_<u32>(memPtr);
        meshes_.resize(size);

        for (u32 i = 0; i < meshes_.size(); ++i)
        {
            meshes_[i] = new Mesh(this);
            meshes_[i]->LoadData(memPtr);
        }

        min_ = extractBinary_<Vector3>(memPtr);
        max_ = extractBinary_<Vector3>(memPtr);

        if (extractBinary_<s8>(memPtr))
            skeleton_ = new Simian::Skeleton(memPtr);

        // load animation frames
        if (animationFile && !loadAnimationFrames_(animationFile))
            return false;

        SFlagSet(flags_, MF_FILELOADED);

        return true;
    }

    void Model::CacheBinaryData( const std::string& location )
    {
        // store mesh data and skeleton
        std::ofstream file;
        file.open(location, std::ios::binary);

        if (!file)
            return;

        // write number of mesh data to store
        u32 size = meshes_.size();
        file.write((s8*)&size, sizeof(u32));

        for (u32 i = 0; i < meshes_.size(); ++i)
            meshes_[i]->CacheData(file);

        // cache min and max points
        file.write((s8*)&min_, sizeof(Vector3));
        file.write((s8*)&max_, sizeof(Vector3));

        s8 hasSkeleton = skeleton_ ? 1 : 0;
        file.write((s8*)&hasSkeleton, sizeof(s8));
        if (hasSkeleton)
            skeleton_->CacheBinaryData(file);
    }

    void Model::UnloadFile()
    {
        if (SIsFlagSet(flags_, MF_FILELOADED))
        {
            if (skeleton_)
                skeleton_->Unload();
            delete skeleton_;
            skeleton_ = 0;

            // unload file
            for (u32 i = 0; i < meshData_.size(); ++i)
                delete meshData_[i];
            meshData_.clear();
        }
        SFlagUnset(flags_, MF_FILELOADED);
    }

    bool Model::LoadModel()
    {
        for (u32 i = 0; i < meshes_.size(); ++i)
            meshes_[i]->Load();

        // load bone shader
        if (!device_->CreateVertexProgram(vertexShader_))
        {
            SWarn("Failed to create skinning shader.");
            for (u32 i = 0; i < meshes_.size(); ++i)
                meshes_[i]->Unload();
            return false;
        }

        if (!vertexShader_->Load("Shaders/skinned_animation.vs", "vsmain", "vs_1_1"))
        {
            // failed to load vertex shader, destroy it
            device_->DestroyVertexProgram(vertexShader_);
            SWarn("Failed to load skinning shader.");
            for (u32 i = 0; i < meshes_.size(); ++i)
                meshes_[i]->Unload();
            return false;
        }

        // get the wvp and bones parameters
        wvp_ = vertexShader_->Parameter("WVP");
        world_ = vertexShader_->Parameter("World");
        view_ = vertexShader_->Parameter("View");
        bones_ = vertexShader_->Parameter("Bones");

        SFlagSet(flags_, MF_MODELLOADED);
        return true;
    }

    void Model::UnloadModel()
    {
        if (SIsFlagSet(flags_, MF_MODELLOADED))
        {
            vertexShader_->Unload();
            device_->DestroyVertexProgram(vertexShader_);

            // unload model
            for (u32 i = 0; i < meshes_.size(); ++i)
            {
                meshes_[i]->Unload();
                delete meshes_[i];
            }
            meshes_.clear();
        }
        SFlagUnset(flags_, MF_MODELLOADED);
    }

    void Model::Unload()
    {
        UnloadFile();
        UnloadModel();
    }

    AnimationController* Model::CreateAnimationController()
    {
        return new AnimationController(this, skeleton_->Root());
    }

    void Model::DestroyAnimationController( AnimationController*& controller )
    {
        delete controller;
        controller = 0;
    }

    void Model::Draw(AnimationController* controller)
    {
        if (controller)
        {
            Simian::Matrix wvp = Device().Projection() * Device().View() * Device().World();
            wvp_.Set(wvp);
            world_.Set(Device().World());
            view_.Set(Device().View());

            // create array of inversed bone matrices
            bones_.Set(&controller->BoneMatrices()[0], controller->BoneMatrices().size());
        }

        for (u32 i = 0; i < meshes_.size(); ++i)
            meshes_[i]->Draw(controller);
    }

    void Model::Draw( RenderQueue& renderQueue, Material& material, const Matrix& transform, u8 layer, AnimationController* controller )
    {
        renderQueue.AddRenderObject(RenderObject(material, layer, transform, 
            Delegate::CreateDelegate<Model, &Model::drawRenderQueue_>(this), controller));
    }
}
