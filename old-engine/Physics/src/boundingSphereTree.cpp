#include "boundingSphereTree.h"
#include "boundingSphere.h"
#include "boundingVolumeFactory.h"
#include "mathDef.h"

#include <set>

namespace Phy
{
    BoundingSphereTree::BoundingSphereTree()
    {
    }

    BoundingSphereTree::BoundingSphereTree( const BoundingSphereTree& val )
        :   BoundingVolumeTree(val)
    {
    }

    BoundingSphereTree::~BoundingSphereTree()
    {
    }

    BoundingVolume* BoundingSphereTree::constructBoundingVolume(const std::list<BoundingVolume*>& bvList)
    {
        Phy::BoundingSphere* sphere = new Phy::BoundingSphere;
        Math::Vec3F minVec(Math::FLOAT_MAX,Math::FLOAT_MAX,Math::FLOAT_MAX);
        Math::Vec3F maxVec(-Math::FLOAT_MAX,-Math::FLOAT_MAX,-Math::FLOAT_MAX);
        Math::Vec3F center(Math::Vec3F::Zero);
        std::list<BoundingVolume*>::const_iterator it = bvList.begin();
        unsigned int totalVertCount = 0;

        // Calculate center
        for(; it != bvList.end(); ++it)
        {
            BoundingSphere* currSphere = reinterpret_cast<BoundingSphere*>(*it);
            center += currSphere->Center();
            totalVertCount += currSphere->VertCount();
        }
        center = center / static_cast<float>(bvList.size());
        it = bvList.begin();

        // Calculate radius
        for(; it != bvList.end(); ++it)
        {
            BoundingSphere* currSphere = reinterpret_cast<BoundingSphere*>(*it);
            Math::Vec3F dirVec(currSphere->Center() - center);
            Math::Vec3F maxPos;
            Math::Vec3F minPos;

            if(std::abs(dirVec.SquareLength()) < Math::EPSILON)
            {
                maxPos = currSphere->Center() + Math::Vec3F(1,1,1).Normalized() * currSphere->Radius();
                minPos = currSphere->Center() - Math::Vec3F(1,1,1).Normalized() * currSphere->Radius();
            }
            else
            {
                Math::Vec3F dirVecNorm(dirVec.Normalized());
                maxPos = dirVec + dirVecNorm * currSphere->Radius();
                minPos = dirVec - dirVecNorm * currSphere->Radius();
            }
            sphere->Grow(maxPos);
            sphere->Grow(minPos);
        }
        sphere->VertCount(totalVertCount);
        sphere->Center(center);
        sphere->Radius(sphere->Radius() * 0.8f);
        return sphere;
    }

    void BoundingSphereTree::generateTopDownMethod(unsigned int level,
                                                   const std::list<BoundingVolume*>& pendingList,
                                                   Util::BinaryTree<BoundingVolume*>*& tree,
                                                   TerminateCondition* terminatingCondition)
    {
        BoundingSphere* sphere = reinterpret_cast<BoundingSphere*>(constructBoundingVolume(pendingList));

        tree = new Util::BinaryTree<BoundingVolume*>;
        tree->Data(sphere);

        // Make sure there is at least a node in the tree
        if((*terminatingCondition)(level,sphere->VertCount()))
            return;

        if(pendingList.size() == 1)
            return;

        std::list<BoundingVolume*> left, right;

        splitBoundingVolumeList(sphere,
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

    void BoundingSphereTree::generateBottomUpMethod(unsigned int level,
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
            BoundingSphere* bSphere = findNodesToMerge(bv0->Data(),bvQueue,bv1It);

            // Create parent tree node and update child nodes
            Util::BinaryTree<BoundingVolume*>* parent = Util::BinaryTree<BoundingVolume*>::CreateTree(root,bv0,*bv1It,0,bSphere);
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
        root->Data(reinterpret_cast<BoundingSphere*>(constructBoundingVolume(bvList)));
        tree = root;
    }

    BoundingSphere* BoundingSphereTree::findNodesToMerge(BoundingVolume* bv0,
                                                         const std::list<Util::BinaryTree<BoundingVolume*>*>& compareList,
                                                         std::list<Util::BinaryTree<BoundingVolume*>*>::const_iterator& bv1)
    {
        BoundingSphere* smallestSphere = 0;
        float radius = Math::FLOAT_MAX;
        std::list<Util::BinaryTree<BoundingVolume*>*>::const_iterator it = compareList.begin();

        for(; it != compareList.end(); ++it)
        {
            std::list<BoundingVolume*> bvList;
            bvList.push_back(bv0);
            bvList.push_back((*it)->Data());

            BoundingSphere* sphere = reinterpret_cast<BoundingSphere*>(constructBoundingVolume(bvList));
            float newRadius = sphere->Radius();
            if(newRadius < radius)
            {
                bv1 = it;
                radius = newRadius;
                if(smallestSphere)
                    delete smallestSphere;
                smallestSphere = sphere;
            }
            else
                delete sphere;
        }
        return smallestSphere;
    }
}