#include "boundingVolumeTree.h"
#include "boundingVolume.h"
#include "mathDef.h"
#include "binaryTree.h"

namespace Phy
{
    const unsigned int BoundingVolumeTree::MAX_DEPTH = 7;
    const unsigned int BoundingVolumeTree::MIN_VERTEX_COUNT = 500;

    //BoundingVolumeTree::ObjectTypePrimitiveMap BoundingVolumeTree::objTypePrimitiveMap_;

    BoundingVolumeTree::BoundingVolumeTree()
        :   splitDirOrder_(0),
            topDownTreeMinVertCount_(0),
            topDownTreeMaxDepth_(0),
            bottomUpTree_(0)
    {
        splitDirOrder_ = new Math::Vec3F [MAX_DEPTH];
        
        unsigned int i = 0;
        while(true)
        {
            if(i == MAX_DEPTH)
                break;
            splitDirOrder_[i] = Math::Vec3F::UnitX;
            ++i;

            if(i == MAX_DEPTH)
                break;
            splitDirOrder_[i] = Math::Vec3F::UnitY;
            ++i;

            if(i == MAX_DEPTH)
                break;
            splitDirOrder_[i] = Math::Vec3F::UnitZ;
            ++i;
        }
    }

    BoundingVolumeTree::BoundingVolumeTree( const BoundingVolumeTree& )
    {
    }

    BoundingVolumeTree::~BoundingVolumeTree()
    {
        delete [] splitDirOrder_;
    }

    void BoundingVolumeTree::PushBoundingVolume(BoundingVolume* val)
    {
        pendingBoundingVolumeList_.push_back(val);
    }

    void BoundingVolumeTree::Load()
    {
        if(pendingBoundingVolumeList_.size() == 0)
            return;

        MinVertexCountCondition minVertCountCond;
        minVertCountCond.minVertCount_ = MIN_VERTEX_COUNT;

        MaxDepthCondition maxDepthCond;
        maxDepthCond.maxDepth_ = MAX_DEPTH;

        generateTopDownMethod(0,
                              pendingBoundingVolumeList_,
                              topDownTreeMinVertCount_,
                              &minVertCountCond);

        generateTopDownMethod(0,
                              pendingBoundingVolumeList_,
                              topDownTreeMaxDepth_,
                              &maxDepthCond);

        generateBottomUpMethod(0,
                               pendingBoundingVolumeList_,
                               bottomUpTree_,
                               0);

        pendingBoundingVolumeList_.clear();
    }

    void BoundingVolumeTree::Unload()
    {
        Util::Delegate func = Util::Delegate::CreateDelegate<BoundingVolumeTree,&BoundingVolumeTree::deleteBoundingVolume>(this);
        
        if(topDownTreeMaxDepth_)
        {
            Util::BinaryTree<BoundingVolume*>::DepthFirst(topDownTreeMaxDepth_,func);
            topDownTreeMaxDepth_->Clear();
            delete topDownTreeMaxDepth_;
            topDownTreeMaxDepth_ = 0;
        }

        if(topDownTreeMinVertCount_)
        {
            Util::BinaryTree<BoundingVolume*>::DepthFirst(topDownTreeMinVertCount_,func);
            topDownTreeMinVertCount_->Clear();
            delete topDownTreeMinVertCount_;
            topDownTreeMinVertCount_ = 0;
        }

        if(bottomUpTree_)
        {
            Util::BinaryTree<BoundingVolume*>::DepthFirst(bottomUpTree_,func);
            bottomUpTree_->Clear();
            delete bottomUpTree_;
            bottomUpTree_ = 0;
        }
    }

/*
    void BoundingVolumeTree::Clear()
    {
        Util::Delegate func = Util::Delegate::CreateDelegate<BoundingVolumeTree,&BoundingVolumeTree::deleteBoundingVolume>(this);
        Util::BinaryTree<BoundingVolume*>::DepthFirst(&topDownTreeMaxDepth_,func);
        Util::BinaryTree<BoundingVolume*>::DepthFirst(&topDownTreeMaxVertCount_,func);
        Util::BinaryTree<BoundingVolume*>::DepthFirst(&bottomUpTreeMaxDepth_,func);
        Util::BinaryTree<BoundingVolume*>::DepthFirst(&bottomUpTreeMaxPrimitiveCount_,func);
        topDownTreeMaxDepth_.Clear();
        topDownTreeMaxVertCount_.Clear();
        bottomUpTreeMaxPrimitiveCount_.Clear();
        bottomUpTreeMaxPrimitiveCount_.Clear();
    }

    void BoundingVolumeTree::PointPushBegin( unsigned int objType )
    {
        ObjectTypePrimitiveMap::iterator it = objTypePrimitiveMap_.insert(std::pair<unsigned int,PrimitiveList>(objType,PrimitiveList(pointComp))).first;
        currPrimitives_ = &it->second;
    }

    void BoundingVolumeTree::PointPush( const Math::Vec3F& pos )
    {
        currPrimitives_->insert(pos);
    }

    void BoundingVolumeTree::PointPushEnd()
    {
        currPrimitives_ = NULL;
    }

    void BoundingVolumeTree::GenerateTopDown( unsigned int* objTypes,
                                              unsigned int objCount,
                                              Math::Vec3F* objPos)
    {
        topDownTreeMaxVertCount_.Clear();
        topDownTreeMaxDepth_.Clear();
        PrimitiveList scenePrimitiveList(pointComp);

        // Generate the scene's point cloud
        for(unsigned int objIndex = 0; objIndex < objCount; ++objIndex)
        {
            PrimitiveList* pList = &objTypePrimitiveMap_.find(objTypes[objIndex])->second;
            PrimitiveList::iterator it = pList->begin();
            for(; it != pList->end(); ++it)
                scenePrimitiveList.insert(*it + objPos[objIndex]);
        }
        topDown(&topDownTreeMaxVertCount_,
                scenePrimitiveList.begin(),
                scenePrimitiveList.end(),
                scenePrimitiveList.size(),
                0,
                &minVertCount);
        topDown(&topDownTreeMaxDepth_,
                scenePrimitiveList.begin(),
                scenePrimitiveList.end(),
                scenePrimitiveList.size(),
                0,
                &maxTreeDepth);
    }

    void BoundingVolumeTree::GenerateBottomUp()
    {

    }
    */
    //--------------------------------------------------------------------------

    Util::BinaryTree<BoundingVolume*>* BoundingVolumeTree::GetTopDownTreeMinVertCount() const
    {
        return topDownTreeMinVertCount_;
    }

    Util::BinaryTree<BoundingVolume*>* BoundingVolumeTree::GetTopDownTreeMaxDepth() const
    {
        return topDownTreeMaxDepth_;
    }

    Util::BinaryTree<BoundingVolume*>* BoundingVolumeTree::GetBottomUpTree() const
    {
        return bottomUpTree_;
    }

    //--------------------------------------------------------------------------

    bool BoundingVolumeTree::MinVertexCountCondition::operator()(unsigned int level,
                                                                 unsigned int vertCount) const
    {
        return vertCount <= minVertCount_;
    }

    bool BoundingVolumeTree::MaxDepthCondition::operator()(unsigned int level,
                                                           unsigned int vertCount) const
    {
        return level >= maxDepth_;
    }

    //--------------------------------------------------------------------------

    void BoundingVolumeTree::splitBoundingVolumeList(BoundingVolume* boundingVolume,
                                                     const std::list<BoundingVolume*>& bvList,
                                                     std::list<BoundingVolume*>& left,
                                                     std::list<BoundingVolume*>& right)
    {
        unsigned int level = 0;

        // If even, iterate until split has the same bvs on both sides
        if(bvList.size() % 2 == 0)
        {
            do
            {
                left.clear();
                right.clear();

                float median = boundingVolume->Center().Dot(splitDirOrder_[level]);
                std::list<BoundingVolume*>::const_iterator it = bvList.begin();
                for(; it != bvList.end(); ++it)
                {
                    BoundingVolume* currBV = *it;
                    if(currBV->Center().Dot(splitDirOrder_[level]) < median)
                        left.push_back(currBV);
                    else
                        right.push_back(currBV);
                }
                ++level;
            }
            while(level != 3 && (left.size() != right.size()));
        }
        else
        {
            do
            {
                left.clear();
                right.clear();

                float median = boundingVolume->Center().Dot(splitDirOrder_[level]);
                std::list<BoundingVolume*>::const_iterator it = bvList.begin();
                for(; it != bvList.end(); ++it)
                {
                    BoundingVolume* currBV = *it;
                    if(currBV->Center().Dot(splitDirOrder_[level]) < median)
                        left.push_back(currBV);
                    else
                        right.push_back(currBV);
                }
                ++level;
            }
            while(level != 3 && (left.size() == 0 || right.size() == 0));
        }
    }

    void BoundingVolumeTree::deleteBoundingVolume( Util::DelegateParameter& param)
    {
        Util::BinaryTree<BoundingVolume*>* node = param.As<Util::BinaryTree<BoundingVolume*>::TreeDelegateParam>().node_;

        if(node->Data())
        {
            delete node->Data();
            node->Data(0);
        }
    }
}