/************************************************************************/
/*!
\file		CopyPointer.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_COPYPOINTER_H_
#define SIMIAN_COPYPOINTER_H_

namespace Simian
{
    template <class T>
    class CopyPointer
    {
    private:
        T* ptr_;
    public:
        CopyPointer& operator=(const CopyPointer& other)
        {
            *ptr_ = *other.ptr_;
            return *this;
        }

        T* operator->() const
        {
            return ptr_;
        }

        T& operator*()
        {
            return *ptr_;
        }

        const T& operator*() const
        {
            return *ptr_;
        }
    public:
        CopyPointer()
            : ptr_(0)
        {
        }

        void Set(T* ptr)
        {
            ptr_ = ptr;
        }

        T*& Get()
        {
            return ptr_;
        }
    };
}

#endif
