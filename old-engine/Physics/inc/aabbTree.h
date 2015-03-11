#ifndef AABBTREE_H
#define AABBTREE_H

#include "configPhysicsDLL.h"
#include "singleton.h"
#include "boundingVolumeTree.h"
#include "boundingAABB.h"

namespace Phy
{
    class ENGINE_PHYSICS_EXPORT AABBTree
        :   public BoundingVolumeTree,
            public Util::Singleton<AABBTree>
    {
    public:
        AABBTree();
        AABBTree(const AABBTree& val);
        ~AABBTree();
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
        BoundingAABB* findNodesToMerge(BoundingVolume* bv0,
                                       const std::list<Util::BinaryTree<BoundingVolume*>*>& compareList,
                                       std::list<Util::BinaryTree<BoundingVolume*>*>::const_iterator& bv1);
    };
}

#endif