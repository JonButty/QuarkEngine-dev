#ifndef BOUNDINGSPHERETREE_H
#define BOUNDINGSPHERETREE_H

#include "configPhysicsDLL.h"
#include "singleton.h"
#include "boundingVolumeTree.h"
#include "boundingSphere.h"

namespace Phy
{
    class ENGINE_PHYSICS_EXPORT BoundingSphereTree
        :   public BoundingVolumeTree,
            public Util::Singleton<BoundingSphereTree>
    {
    public:
        BoundingSphereTree();
        BoundingSphereTree(const BoundingSphereTree& val);
        ~BoundingSphereTree();
    private:
        BoundingVolume* constructBoundingVolume(const std::list<BoundingVolume*>& bvList);
        void generateTopDownMethod(unsigned int level,
                                   const std::list<BoundingVolume*>& pendingList,
                                   Util::BinaryTree<BoundingVolume*>*& tree,
                                   TerminateCondition* terminatingCondition);
        void generateBottomUpMethod(unsigned int level,
                                    const std::list<BoundingVolume*>& pendingList,
                                    Util::BinaryTree<BoundingVolume*>*& tree,
                                    TerminateCondition* terminatingCondition);
        
        BoundingSphere* findNodesToMerge(BoundingVolume* bv0,
                                         const std::list<Util::BinaryTree<BoundingVolume*>*>& compareList,
                                         std::list<Util::BinaryTree<BoundingVolume*>*>::const_iterator& bv1);
    };
}

#endif