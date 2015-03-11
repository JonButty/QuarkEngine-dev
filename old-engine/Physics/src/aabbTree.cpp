#include "aabbTree.h"
#include "boundingAABB.h"
#include "boundingVolumeFactory.h"
#include "mathDef.h"

#include <set>

namespace Phy
{
    AABBTree::AABBTree()
    {
    }

    AABBTree::AABBTree( const AABBTree& val )
        :   BoundingVolumeTree(val)
    {
    }

    AABBTree::~AABBTree()
    {
    }

    BoundingVolume* AABBTree::constructBoundingVolume(const std::list<BoundingVolume*>& bvList)
    {
        BoundingAABB* aabb = new BoundingAABB;
        Math::Vec3F minVec(Math::FLOAT_MAX,Math::FLOAT_MAX,Math::FLOAT_MAX);
        Math::Vec3F maxVec(-Math::FLOAT_MAX,-Math::FLOAT_MAX,-Math::FLOAT_MAX);
        unsigned int totalVertCount = 0;
        std::list<BoundingVolume*>::const_iterator it = bvList.begin();

        for(; it != bvList.end(); ++it)
        {
            const BoundingAABB* currAABB = reinterpret_cast<const BoundingAABB*>(*it);
            Math::Vec3F pos(currAABB->MaxPt());
            minVec.X(pos.X() < minVec.X() ? pos.X() : minVec.X());
            minVec.Y(pos.Y() < minVec.Y() ? pos.Y() : minVec.Y());
            minVec.Z(pos.Z() < minVec.Z() ? pos.Z() : minVec.Z());

            maxVec.X(pos.X() > maxVec.X() ? pos.X() : maxVec.X());
            maxVec.Y(pos.Y() > maxVec.Y() ? pos.Y() : maxVec.Y());
            maxVec.Z(pos.Z() > maxVec.Z() ? pos.Z() : maxVec.Z());

            pos = currAABB->MinPt();
            minVec.X(pos.X() < minVec.X() ? pos.X() : minVec.X());
            minVec.Y(pos.Y() < minVec.Y() ? pos.Y() : minVec.Y());
            minVec.Z(pos.Z() < minVec.Z() ? pos.Z() : minVec.Z());

            maxVec.X(pos.X() > maxVec.X() ? pos.X() : maxVec.X());
            maxVec.Y(pos.Y() > maxVec.Y() ? pos.Y() : maxVec.Y());
            maxVec.Z(pos.Z() > maxVec.Z() ? pos.Z() : maxVec.Z());
            totalVertCount += currAABB->VertCount();
        }
        aabb->VertCount(totalVertCount);
        aabb->MinPt(minVec);
        aabb->MaxPt(maxVec);
        aabb->Center((minVec + maxVec) / 2.0f);
        return aabb;
    }

    void AABBTree::generateTopDownMethod(unsigned int level,
                                         const std::list<BoundingVolume*>& pendingList,
                                         Util::BinaryTree<BoundingVolume*>*& tree,
                                         TerminateCondition* terminatingCondition)
    {
        BoundingAABB* aabb = reinterpret_cast<BoundingAABB*>(constructBoundingVolume(pendingList));
        
        tree = new Util::BinaryTree<BoundingVolume*>;
        tree->Data(aabb);
        
        // Make sure there is at least a node in the tree
        if((*terminatingCondition)(level,aabb->VertCount()))
            return;

        if(pendingList.size() == 1)
            return;

        std::list<BoundingVolume*> left, right;

        splitBoundingVolumeList(aabb,
                                pendingList,
                                left,
                                right);

        if(left.size() > 0)
        {
            Util::BinaryTree<BoundingVolume*>* leftNode = 0;
            generateTopDownMethod(level + 1,
                                  left,
                                  leftNode,
                                  terminatingCondition);
            if(leftNode)
                tree->Left(leftNode);
        }

        if(right.size() > 0)
        {
            Util::BinaryTree<BoundingVolume*>* rightNode = 0;
            generateTopDownMethod(level + 1,
                                  right,
                                  rightNode,
                                  terminatingCondition);
            if(rightNode)
                tree->Right(rightNode);
        }
    }

    void AABBTree::generateBottomUpMethod(unsigned int level,
                                          const std::list<BoundingVolume*>& pendingList,
                                          Util::BinaryTree<BoundingVolume*>*& tree,
                                          TerminateCondition* terminatingCondition)
    {
        Util::BinaryTree<BoundingVolume*>* root = Util::BinaryTree<BoundingVolume*>::CreateTree(0,0,0,0,0);

        // bv queue
        std::list<Util::BinaryTree<BoundingVolume*>*> bvQueue;
        std::list<BoundingVolume*>::const_iterator pendingIt = pendingList.begin();
        for(; pendingIt != pendingList.end(); ++pendingIt)
        {
            Util::BinaryTree<BoundingVolume*>* leaf = Util::BinaryTree<BoundingVolume*>::CreateTree(root,0,0,0,*pendingIt);
            bvQueue.push_back(leaf);
        }

        // merge until there are two nodes left
        while(bvQueue.size() > 2)
        {
            // Take the first BV and find the other bv in the list that will
            // generate the smallest bv after merging
            std::list<Util::BinaryTree<BoundingVolume*>*>::const_iterator bv0It = bvQueue.begin();
            Util::BinaryTree<BoundingVolume*>* bv0 = *bv0It;
            std::list<Util::BinaryTree<BoundingVolume*>*>::const_iterator bv1It;
            bvQueue.erase(bv0It);

            // Find and merge the bvs
            BoundingAABB* aabb = findNodesToMerge(bv0->Data(),bvQueue,bv1It);

            // Create parent tree node and update child nodes
            Util::BinaryTree<BoundingVolume*>* parent = Util::BinaryTree<BoundingVolume*>::CreateTree(root,bv0,*bv1It,0,aabb);
            bv0->Parent(parent);
            (*bv1It)->Parent(parent);

            // Add merged bv to the back of the bv queue and remove the 2 bvs 
            // from queue
            bvQueue.push_back(parent);
            bvQueue.erase(bv1It);
        }
        
        // Assign remaining 2 nodes with root
        root->Left(*bvQueue.begin());
        root->Right(*(++bvQueue.begin()));

        std::list<BoundingVolume*> bvList;
        bvList.push_back(root->Left()->Data());
        bvList.push_back(root->Right()->Data());
        root->Data(reinterpret_cast<BoundingAABB*>(constructBoundingVolume(bvList)));
        tree = root;
    }

    BoundingAABB* AABBTree::findNodesToMerge(BoundingVolume* bv0,
                                             const std::list<Util::BinaryTree<BoundingVolume*>*>& compareList,
                                             std::list<Util::BinaryTree<BoundingVolume*>*>::const_iterator& bv1)
    {
        BoundingAABB* smallestAABB = 0;
        float radius = Math::FLOAT_MAX;
        std::list<Util::BinaryTree<BoundingVolume*>*>::const_iterator it = compareList.begin();
        
        for(; it != compareList.end(); ++it)
        {
            std::list<BoundingVolume*> bvList;
            bvList.push_back(bv0);
            bvList.push_back((*it)->Data());
            
            BoundingAABB* aabb = reinterpret_cast<BoundingAABB*>(constructBoundingVolume(bvList));
            float newRadius = (aabb->MaxPt() - aabb->MinPt()).Length() * 0.5f;
            if(newRadius < radius)
            {
                bv1 = it;
                radius = newRadius;
                if(smallestAABB)
                    delete smallestAABB;
                smallestAABB = aabb;
            }
            else
                delete aabb;
        }
        return smallestAABB;
    }
}