#ifndef BINARYTREE_H
#define BINARYTREE_H

#include "delegate.h"

namespace Util
{
    template <typename T>
    class BinaryTree
    {
    public:
        struct TreeDelegateParam
            : public DelegateParameter
        {
            TreeDelegateParam()
                :   node_(0)
            {}
            BinaryTree<T>* node_;
        };
    public:
        BinaryTree()
            :   parent_(0),
                left_(0),
                right_(0),
                size_(0),
                dirty_(true)
        {
        }

        BinaryTree(BinaryTree<T>*& root,
                   BinaryTree<T>*& left,
                   BinaryTree<T>*& right,
                   BinaryTree<T>*& parent,
                   T& data)
                   :    left_(left),
                        right_(right),
                        parent_(parent),
                        data_(data),
                        dirty_(true),
                        size_(0)
        {
        }

        BinaryTree(const BinaryTree<T>& val)
            :   left_(0),
                right_(0),
                parent_(0),
                data_(val.data_),
                dirty_(val.dirty_),
                size_(0)
        {
            if(val.left_)
            {
                left_ = new BinaryTree<T>(*val.left_);
                left_->parent_ = this;
            }

            if(val.right_)
            {
                right_ = new BinaryTree<T>(*val.right_);
                right_->parent_ = this;
            }
        }

        ~BinaryTree()
        {
           if(left_)
            {
                //DepthFirst(left_,Delegate::CreateDelegate<BinaryTree<T>,&BinaryTree<T>::deleteNode>(this));
                delete left_;
                left_ = 0;
            }
                
            if(right_)
            {
                //DepthFirst(right_,Delegate::CreateDelegate<BinaryTree<T>,&BinaryTree<T>::deleteNode>(this));
                delete right_;
                right_ = 0;
            }
        }

        bool IsLeaf() const
        {
            return !left_ && !right;
        }

        void Clear()
        {
            if(left_)
            {
                left_->Clear();
                delete left_;
                left_ = 0;
            }
            
            if(right_)
            {
                right_->Clear();
                delete right_;
                right_ = 0;
            }
        }

        void DepthFirst(Delegate func)
        {
            if(left_)
                DepthFirst(left_,func);
            if(right_)
                DepthFirst(right_,func);

            TreeDelegateParam param;
            param.node_ = node;
            func.Call(param);
        }

        void BreadthFirst(Delegate func)
        {
            std::queue<BinaryTree<T>*> valueQueue;

            BinaryTree<T>* tempNode = this;
            while(tempNode)
            {
                TreeDelegateParam param;
                param.node_ = tempNode;
                func.Callback(param);

                if(tempNode->left_)
                    valueQueue.push(tempNode->left_);
                if(tempNode->right_)
                    valueQueue.push(tempNode->right_);
                tempNode = valueQueue.front();
                valueQueue.pop();
            }
        }

        unsigned int MaxDepth() const
        {
            unsigned int leftDepth = 0, rightDepth = 0;
            if(left_)
                leftDepth = left_->MaxDepth() + 1;
            if(right_)
                rightDepth = right_->MaxDepth() + 1;
            return (leftDepth > rightDepth) ? leftDepth : rightDepth;
        }

        static void DepthFirst(BinaryTree<T>* node,
                               Delegate func)
        {
            if(node->left_)
                DepthFirst(node->left_,func);
            if(node->right_)
                DepthFirst(node->right_,func);

            TreeDelegateParam param;
            param.node_ = node;
            func.Call(param);
        }

        static void BreadthFirst(BinaryTree<T>* node,
                                 Delegate func)
        {
            std::queue<BinaryTree<T>*> valueQueue;

            BinaryTree<T>* tempNode = node;
            while(tempNode)
            {
                TreeDelegateParam param;
                param.node_ = tempNode;
                func.Callback(param);

                if(tempNode->left_)
                    valueQueue.push(tempNode->left_);
                if(tempNode->right_)
                    valueQueue.push(tempNode->right_);
                tempNode = valueQueue.front();
                valueQueue.pop();
            }
        }

        static BinaryTree<T>* CreateTree(BinaryTree<T>* root,
                                         BinaryTree<T>* left,
                                         BinaryTree<T>* right,
                                         BinaryTree<T>* parent,
                                         T data)
        {
            return new BinaryTree<T>(root,left,right,parent,data);    
        }

    public:
        BinaryTree<T>* Root() const
        {
            BinaryTree<T>* node = parent_;
            while(node)
                node = node->parent_;
            return node;
        }

        BinaryTree<T>* Left() const
        {
            return left_;
        }

        void Left(BinaryTree<T>* val)
        {
            dirty_ = true;
            left_ = val;
            left_->parent_ = this;
        }

        BinaryTree<T>* Right() const
        {
            return right_;
        }

        void Right(BinaryTree<T>* val)
        {
            dirty_ = true;
            right_ = val;
            right_->parent_ = this;
        } 

        BinaryTree<T>* Parent() const
        {
            return parent_;
        }

        void Parent(BinaryTree<T>*& val)
        {
            parent_ = val;
        }

        T Data() const
        {
            return data_;
        }

        void Data(const T& val)
        {
            data_ = val;
        }

        unsigned int Size() const
        {
            if(dirty_)
            {
                dirty_ = false;
                size_ = 0;

                if(left_)
                {
                    left_->recalcSize();
                    size_ += left_->Size() + 1;
                }

                if(right_)
                {
                    right_->recalcSize();
                    size_ += right_->Size() + 1;
                }
            }
            return size_;
        }
    
    private:
        void deleteNode(DelegateParameter& param)
        {
            BinaryTree<T>* node = param.As<TreeDelegateParam>().node_;

            // This is not the root
            if(node->parent_)
            {
                // This is the left child
                if(node->parent_->left_ == node)
                    node->parent_->left_ = 0;
                else
                    node->parent_->right_ = 0;
                delete node;
            }
        }

    private:
        bool dirty_;
        BinaryTree<T>* left_;
        BinaryTree<T>* right_;
        BinaryTree<T>* parent_;
        unsigned int size_;
        T data_;
    };
}

#endif