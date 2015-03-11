/************************************************************************/
/*!
\file		Model.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_MODEL_H_
#define SIMIAN_MODEL_H_

#include "SimianPlatform.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Color.h"
#include "Mesh.h"
#include "Skeleton.h"
#include "DataLocation.h"
#include "Delegate.h"
#include "RenderQueue.h"
#include "ShaderParameter.h"

#include <vector>
#include <map>

class TiXmlElement;

namespace Simian
{
    class DataLocation;
    class GraphicsDevice;
    class AnimationController;
    class RenderQueue;
    class VertexProgram;
    
    struct SIMIAN_EXPORT AnimationFrames
    {
        std::string Name;
        u32 Start, End;
        u32 Fps;
    };

    struct SIMIAN_EXPORT IndexValues
    {
        u32 Position;
        u32 Normal;
        u32 UV;
    };

    struct SIMIAN_EXPORT MeshVertex
    {
        Simian::Vector3 Position;
        std::vector<std::pair<u8, f32> > Weights;
    };

    struct SIMIAN_EXPORT MeshData
    {
        std::vector<MeshVertex> Positions;
        std::vector<Simian::Vector3> Normals;
        std::vector<Simian::Vector2> UVs;
        std::vector<IndexValues> IndexData;
        bool Skinned;
    };

    class SIMIAN_EXPORT Model
    {
    public:
        enum ModelFlags
        {
            MF_FILELOADED,
            MF_MODELLOADED
        };
    private:
        GraphicsDevice* device_;
        u8 flags_;

        TiXmlElement* root_;
        TiXmlElement* scene_;
        TiXmlElement* geometry_;
        TiXmlElement* controllers_;
        TiXmlElement* skin_;
        TiXmlElement* animations_;
        TiXmlElement* jointRoot_;

        std::vector<MeshData*> meshData_;
        std::vector<Mesh*> meshes_;
        Skeleton* skeleton_;

        std::map<std::string, AnimationFrames> animationFrames_;

        Vector3 min_;
        Vector3 max_;

        VertexProgram* vertexShader_;
        ShaderParameter wvp_;
        ShaderParameter world_;
        ShaderParameter view_;
        ShaderParameter bones_;
    public:
        GraphicsDevice& Device() const;
        Skeleton* Skeleton() const;
        const AnimationFrames* AnimationFrame(const std::string& name) const;
        const Simian::Vector3& Min() const;
        const Simian::Vector3& Max() const;
    private:
        bool parseScene_();
        bool parseController_(TiXmlElement* controller);
        bool parseMesh_(TiXmlElement* meshNode, bool skinned = false);
        Joint* parseJoint_(TiXmlElement* jointNode, std::vector<Joint*>& joints);
        bool buildSkeleton_();
        bool parseAnimations_();
        bool loadAnimationFrames_(const DataLocation& animationFile);

        void drawRenderQueue_(DelegateParameter& param);
    public:
        Model(GraphicsDevice* device);

        bool LoadImmediate(const DataLocation& data);
        
        bool LoadFile(const DataLocation& data, const DataLocation& animationFile = "");
        bool LoadBinaryFile(const DataLocation& data, const DataLocation& animationFile = "");
        void CacheBinaryData(const std::string& location);
        void UnloadFile();

        bool LoadModel();
        void UnloadModel();

        void Unload();

        AnimationController* CreateAnimationController();
        void DestroyAnimationController(AnimationController*& controller);

        void Draw(AnimationController* controller = 0);
        void Draw(RenderQueue& renderQueue, Material& material, const Matrix& transform, u8 layer = RenderQueue::RL_SCENE, AnimationController* controller = 0);

        friend class Mesh;
    };
}

#endif
