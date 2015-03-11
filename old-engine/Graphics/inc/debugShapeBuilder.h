#ifndef DEBUGSHAPEBUILDER_H
#define DEBUGSHAPEBUILDER_H

#include "singleton.h"
#include "configGraphicsDLL.h"
#include "matrix4.h"
#include "vector3.h"
#include "binaryTree.h"

namespace Phy
{
    class BoundingVolume;
}

namespace GFX
{
    class DebugShape;
    class DebugAABB;
    class Color;
    struct Vertex;

    class ENGINE_GRAPHICS_EXPORT DebugShapeBuilder
        :   public Util::Singleton<DebugShapeBuilder>
    {
    public:
        DebugShapeBuilder();
        DebugShape* BuildAABB(const Vertex* vertList,
                              unsigned int vertCount);
        DebugShape* BuildCentroidSphere(const Vertex* vertList,
                                        unsigned int vertCount);
        DebugShape* BuildRitterSphere(const Vertex* vertList,
                                      unsigned int vertCount);
        DebugShape* BuildLarssonSphere(const Vertex* vertList,
                                       unsigned int vertCount);
        DebugShape* BuildPCASphere(const Vertex* vertList,
                                   unsigned int vertCount);
        
        DebugShape* BuildPCAEllipse(const Vertex* vertList,
                                    unsigned int vertCount);
        DebugShape* BuildPCAOBB(const Vertex* vertList,
                                unsigned int vertCount);
        DebugShape* BuildPCAAABB(const Vertex* vertList,
                                 unsigned int vertCount);
        Util::BinaryTree<DebugShape*>* BuildTree( const Util::BinaryTree<Phy::BoundingVolume*>& tree);
        void DestroyTree(Util::BinaryTree<DebugShape*>*& tree);
        void RenderTree(Util::BinaryTree<DebugShape*>*& tree);
        void RenderTreeNodesWithDepth(unsigned int depth,
                                      Util::BinaryTree<DebugShape*>*& tree);
    private:
        void calculateCovarianceMatrix(const Vertex* vertList,
                                       unsigned int vertCount,
                                       const Math::Vec3F& centroid,
                                       Math::Mtx4F& result);
        void realSchurDecomposition(Math::Mtx4F& covariance,
                                    unsigned int p,
                                    unsigned int q,
                                    float& c,
                                    float& s);
        void jacobi(Math::Mtx4F& a,
                    Math::Mtx4F& v);
        void extremePoints(const Math::Vec3F& dir,
                           const Vertex* vertList,
                           int n,
                           int* imin,
                           int* imax);
        Util::BinaryTree<DebugShape*>* buildTreeRecurrive( const Util::BinaryTree<Phy::BoundingVolume*>& tree,
                                                           unsigned int level,
                                                           Color* colorArray);
        void deleteDebugShapeCallback(Util::DelegateParameter& param);
        void renderDebugShapeCallback(Util::DelegateParameter& param);
        void renderDebugShapeWithMaxDepthCallback(Util::DelegateParameter& param);
    private:
        unsigned int renderTreeNodeMaxDepth_;
    };
}

#endif