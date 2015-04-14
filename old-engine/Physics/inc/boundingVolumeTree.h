#ifndef BOUNDINGVOLUMETREE_H
#define BOUNDINGVOLUMETREE_H

#include "configPhysicsDLL.h"
#include "binaryTree.h"
#include "vector3.h"

#include <list>

namespace Phy
{
    class BoundingVolume;

    class ENGINE_PHYSICS_EXPORT BoundingVolumeTree
    {
    public:
        static const unsigned int MAX_DEPTH;
        static const unsigned int MIN_VERTEX_COUNT;
    public:
        BoundingVolumeTree();
        BoundingVolumeTree(const BoundingVolumeTree& val);
        virtual ~BoundingVolumeTree();
        void PushBoundingVolume(BoundingVolume* val);
        void Load();
        void Unload();
    public:
        Util::BinaryTree<BoundingVolume*>* GetTopDownTreeMinVertCount() const;
        Util::BinaryTree<BoundingVolume*>* GetTopDownTreeMaxDepth() const;
        Util::BinaryTree<BoundingVolume*>* GetBottomUpTree() const;
    protected:
        struct TerminateCondition
        {
            virtual bool operator()(unsigned int level,
                                    unsigned int vertCount) const = 0;
        };

        struct MinVertexCountCondition
            :   public TerminateCondition
        {
            bool operator()(unsigned int level,
                            unsigned int vertCount) const;
            unsigned int minVertCount_;
        };

        struct MaxDepthCondition
            :   public TerminateCondition
        {
            bool operator()(unsigned int level,
                            unsigned int vertCount) const;
            unsigned int maxDepth_;
        };
    protected:
        void splitBoundingVolumeList(BoundingVolume* boundingVolume,
                                     const std::list<BoundingVolume*>& bvList,
                                     std::list<BoundingVolume*>& left,
                                     std::list<BoundingVolume*>& right);
        virtual BoundingVolume* constructBoundingVolume(const std::list<BoundingVolume*>& bvList) = 0;
        virtual void generateTopDownMethod(unsigned int level,
                                           const std::list<BoundingVolume*>& pendingList,
                                           Util::BinaryTree<BoundingVolume*>*& tree,
                                           TerminateCondition* terminatingCondition) = 0;
        virtual void generateBottomUpMethod(unsigned int level,
                                            const std::list<BoundingVolume*>& pendingList,
                                            Util::BinaryTree<BoundingVolume*>*& tree,
                                            TerminateCondition* terminatingCondition) = 0;
    protected:
        void deleteBoundingVolume(Util::DelegateParameter& param);
    private:
        Math::Vec3F* splitDirOrder_;
        std::list<BoundingVolume*> pendingBoundingVolumeList_;
        Util::BinaryTree<BoundingVolume*>* topDownTreeMinVertCount_;
        Util::BinaryTree<BoundingVolume*>* topDownTreeMaxDepth_;
        Util::BinaryTree<BoundingVolume*>* bottomUpTree_;
    };
}

#endif