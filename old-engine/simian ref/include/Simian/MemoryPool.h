/*****************************************************************************/
/*!
\file		MemoryPool.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_MEMORYPOOL_H_
#define SIMIAN_MEMORYPOOL_H_

#include <algorithm>
#include <vector>
#include <list>
#include "Debug.h"

namespace Simian
{
    /** A finite pool of objects in memory. */
    template <class T>
    class MemoryPool
    {
    private:
        size_t capacity_;
        std::vector<T*> pool_;
        std::list<T*> borrowed_;
    public:
        /** Gets the pool vector. */
        const std::vector<T*>& Pool() const
        {
            return pool_;
        }

        /** Gets the borrowed vector.
            @remarks
                The borrowed vector contains objects that were fetched from the
                pool and not yet returned.
        */
        const std::list<T*>& Borrowed() const
        {
            return borrowed_;
        }

        /** Gets the current size of the pool. */
        size_t Size() const
        {
            return (s32)pool_.size();
        }

        /** Gets the maximum capacity of the pool. */
        size_t Capacity() const
        {
            return capacity_;
        }
    public:
        /** Creates a MemoryPool with the size specified. */
        MemoryPool(size_t size)
        : capacity_(size)
        {
            pool_.reserve(size);
            for (size_t i = 0; i < size; ++i)
                pool_.push_back(new T());
        }

        /** Called when a MemoryPool is being destructed. */
        virtual ~MemoryPool()
        {
            for (size_t i = 0; i < pool_.size(); ++i)
                delete pool_[i];
            for (std::list<T*>::iterator i = borrowed_.begin(); i != borrowed_.end(); ++i)
                delete *i;
            pool_.clear();
            borrowed_.clear();
        }

        /** Fetches an element from the MemoryPool.
            @remarks
                Fetched elements are added to the borrowed list and removed 
                from the pool list.
            @see
                Pool, Borrowed
        */
        T* Fetch()
        {
            SAssert(pool_.size() > 0, "The memory pool is empty.");
            T* ret = pool_.back();
            pool_.pop_back();
            borrowed_.push_back(ret);
            return ret;
        }

        /** Returns an element to the memory pool.
            @remarks
                Returned elements are added to the pool list and removed 
                from the borrowed list.
            @see
                Pool, Borrowed
        */
        void Return(T* element)
        {
            SAssert(pool_.size() < capacity_, "The memory pool is full.");
            SAssert(std::find(borrowed_.begin(), borrowed_.end(), element) != borrowed_.end(), 
                    "Element is not in memory pool.");
            pool_.push_back(element);
            borrowed_.remove(element);
        }
    };
}

#endif
