#ifndef MESHDX10_H
#define MESHDX10_H

#include "configGraphicsDLL.h"
#include "vector3.h"

#include <d3d10.h>
#include <d3dx10math.h>
#include <map>
#include <list>

namespace GFX
{
    class DebugShape;
    struct Vertex;
    struct Face;
    struct HalfEdge;
    class Shader;
    class ShaderBoundary;
    class Camera;
    class Color;

    class ENGINE_GRAPHICS_EXPORT Mesh
    {
    public:
        enum RenderMethod
        {
            RM_TRILIST,
            RM_TRISTRIP,
            RM_LINELIST,
            RM_LINESTRIP,
            RM_POINTLIST,
            RM_TOTAL
        };
        enum BoundingVolumeLevels
        {
            BVL_AABB,
            BVL_SPHERE_CENTROID,
            BVL_SPHERE_RITTER,
            BVL_SPHERE_LARSSON,
            BVL_SPHERE_PCA,
            BVL_OBB_PCA,
            BVL_AABB_PCA,
            BVL_ELLIPSE_PCA,
            BVL_TOTAL
        };
    public:
        Mesh(unsigned int type,
             Vertex*& vertList,
             unsigned int vertCount,
             unsigned int**& indexList,
             std::list<unsigned int>*& stripSizes,
             unsigned int* boundaryIndexList,
             unsigned int boundaryIndexCount,
             RenderMethod method = RM_TRISTRIP);
        Mesh(const Mesh& val);
        ~Mesh();
        void Load(bool generateBV = true);
        void LoadBVs(Math::Vec3F* offsets,
                     unsigned int offsetCount);
        void Render(Shader* shader,
                    Camera* camera,
                    Math::Vec3F* offsets,
                    unsigned int offsetCount);
        void RenderBoundingVolume(Camera* camera,
                                  Math::Vec3F* position,
                                  unsigned int positionCount);
        void Unload();
        void RenderSetup();
        void RenderBoundingVolumeSetup();
    public:
        bool RenderMesh() const;
        void RenderMesh(bool val);
        bool RenderBoundaryEdges() const;
        void RenderBoundaryEdges(bool val);
        void BVLevel(unsigned int val);
        unsigned int BVLevel() const;
    private:
        bool renderMesh_;
        bool renderBoundaryEdges_;
        ID3D10Buffer* vertBuffer_;
        ID3D10Buffer** indexBuffer_;
        ID3D10Buffer* boundaryIndexBuffer_;
        ShaderBoundary* boundaryShader_;
        Vertex* vertList_;
        unsigned int** indexList_;
        unsigned int* boundaryIndexList_;
        std::list<unsigned int>* stripSizes_;
        unsigned int type_;
        unsigned int renderMethod_;
        unsigned int indexCount_;
        unsigned int vertCount_;
        unsigned int boundaryIndexCount_;
        unsigned int currBVLevel_;
        DebugShape* bvLevels_[BVL_TOTAL];
        static const D3D_PRIMITIVE_TOPOLOGY RenderMethodArray[RM_TOTAL];
    };
}

#endif