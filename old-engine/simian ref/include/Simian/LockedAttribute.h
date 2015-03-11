/************************************************************************/
/*!
\file		LockedAttribute.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_LOCKEDATTRIBUTE_H_
#define SIMIAN_LOCKEDATTRIBUTE_H_

#include "SimianPlatform.h"
#include "Lock.h"

namespace Simian
{
    template <typename T>
    class LockedAttribute
    {
    private:
        volatile Lock lock_;
        volatile T attribute_;
    private:
        LockedAttribute(const LockedAttribute<T>& locked);
        LockedAttribute<T>& operator=(const LockedAttribute<T>&);
    public:
        LockedAttribute(T initialValue = T())
            : attribute_(initialValue)
        {
        }

        void Set(const T& value)
        {
            lock_.Acquire();
            attribute_ = value;
            lock_.Release();
        }

        T Get()
        {
            lock_.Acquire();
            T ret = (T)attribute_;
            lock_.Release();
            return ret;
        }
    };

    // specialization for pointers (dereferences)
    template <typename T>
    class LockedAttribute<T*>
    {
    private:
        volatile Lock lock_;
        volatile T* attribute_;
    public:
        LockedAttribute(T* initialValue)
            : attribute_(initialValue)
        {
        }

        void Set(const T& value)
        {
            lock_.Acquire();
            *attribute_ = value;
            lock_.Release();
        }

        T Get()
        {
            lock_.Acquire();
            T ret = (T)*attribute_;
            lock_.Release();
            return ret;
        }
    };
}

#endif
