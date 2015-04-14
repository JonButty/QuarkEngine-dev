#ifndef SYSTEMDX_H
#define SYSTEMDX_H

#include "singleton.h"
#include "os.h"
#include "configGraphicsDLL.h"
#include "color.h"
#include "inputDevice.h"
#include "meshTypes.h"
#include "shaderTypes.h"
#include "vector3.h"

#include <string>

namespace Util
{
    template<typename T>
    class BinaryTree;
}

namespace GFX
{
    class Mesh;
    class LightPoint;
    class DebugShape;
    class Camera;

    class ENGINE_GRAPHICS_EXPORT System
        :   public Util::Singleton<System>
    {
    public:
        System();
        ~System();
        void Load();
        void Update(float dt);
        void Unload();
    private:
        void renderBasis(Camera* cam);
    private:
        bool lockLightToCam_;
        bool renderBV_;
        Mesh* basisMesh_;
        Math::Vec3F* currMeshesPos_;
        LightPoint* currLight_;
        unsigned int currMeshesPosCount_;
        unsigned int renderTreeAtDepth_;
        MeshType currMesh_;
        ShaderType currShader_;
        Util::BinaryTree<DebugShape*>* currTreeToRender_;
        Util::BinaryTree<DebugShape*>* topDownAABBTreeMaxDepth_; 
        Util::BinaryTree<DebugShape*>* topDownAABBTreeMinVertCount_; 
        Util::BinaryTree<DebugShape*>* bottomUpAABBTree_;
        Util::BinaryTree<DebugShape*>* topDownBoundingSphereTreeMaxDepth_; 
        Util::BinaryTree<DebugShape*>* topDownBoundingSphereTreeMinVertCount_; 
        Util::BinaryTree<DebugShape*>* bottomUpBoundingSphereTree_;
    };
}

#endif