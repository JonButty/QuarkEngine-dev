#ifndef KTREE_H
#define KTREE_H

namespace Util
{
    template <typename T, unsigned int BranchCount>
    class KTree
    {
    public:
        KTree()
            :   root_(0),
                parent_(0),
                size_(0)
        {
            for(unsigned int i = 0; i < BranchCount; ++i)
                children_[i] = 0;
        }

        KTree(const KTree& val)
        {
        }

        virtual ~KTree()
        {
        }

        void Clear()
        {
        }
    protected:
        KTree<T,BranchCount>* root() const
        {
            return root_;
        }

        void root(KTree<T,BranchCount>* val)
        {
            root_ = val;
        }

        KTree<T,BranchCount>* parent() const
        {
            return parent_;
        }

        void parent(KTree<T,BranchCount>* val)
        {
            parent_ = val;
        }

        KTree<T,BranchCount>* child(unsigned int index) const
        {
            return children_[index];
        }

        void child(KTree<T,BranchCount>* val,
                   unsigned int index)
        {
            children_[index] = val;
        }

        T data() const
        {
            return data_;
        }

        void data(const T& val)
        {
            data_ = val;
        }

        unsigned int size() const
        {
            return size_;
        }

        void size(unsigned int val)
        {
            size_ = val;
        }
    protected:
        void recalcSize()
        {
            size_ = 0;
            for(unsigned int i = 0; i < BranchCount; ++i)
            {
                if(children_[i])
                {
                    children_[i]->recalcSize();
                    size_ += children_[i]->Size();
                }
            }
        }
    private:
        KTree<T,BranchCount>* root_;
        KTree<T,BranchCount>* parent_;
        KTree<T,BranchCount>* children_[BranchCount];
        T data_;
        unsigned int size_;
    };
}

#endif