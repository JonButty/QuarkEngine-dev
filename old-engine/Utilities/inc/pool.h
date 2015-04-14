#ifndef POOL_H
#define POOL_H

#include "utilAssert.h"

#include <queue>
#include <set>

namespace Util
{
    template <typename T>
    class Pool 
    {
    public:
        Pool()
        {
        }

        Pool(const Pool& val)
            :   pool_(val.pool_),
                borrowed_(val.borrowed_)
        {
        }

        ~Pool()
        {
        }

        void Push(const T& val)
        {
            pool_.push(val);
        }

        T Pop()
        {
            ENGINE_ASSERT_MSG(pool_.size() > 0, "Pool is empty.");
            T ret(pool_.front());
            pool_.pop();
            borrowed_.insert(ret);
            return ret;
        }

        void Return(const T& val)
        {
            std::multiset<T>::iterator it = borrowed_.find(val);
            
            ENGINE_ASSERT_MSG(it != borrowed_.end(), "No such element has been borrowed.");

            pool_.push(*it);
            borrowed_.erase(it);
        }
    private:
        std::queue<T> pool_;
        std::multiset<T> borrowed_;
    };
}

#endif