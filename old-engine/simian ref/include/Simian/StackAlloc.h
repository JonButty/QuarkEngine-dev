/************************************************************************/
/*!
\file		StackAlloc.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_STACKALLOC_H_
#define SIMIAN_STACKALLOC_H_

#include "Simian/SimianPlatform.h"
#include "Simian/Debug.h"

namespace Simian
{
    template <u32 size>
    class StackAlloc
    {
    private:
        s8 buffer_[size];
        u32 top_;
    public:
        StackAlloc()
            : top_(0)
        {
        }

        template <class T>
        T* Alloc()
        {
            // cannot create if it is a higher level
            if (top_ > size - sizeof(T))
            {
                SAssert(false, "Stack allocator exhausted.");
                return 0;
            }
            T* ret = reinterpret_cast<T*>(buffer_ + top_);
            top_ += sizeof(T);
            ret = new(ret)T(); // highly evil use of new.. but unavoidable
            return ret;
        }

        void Clear()
        {
            top_ = 0;
        }
    };
}

#endif
