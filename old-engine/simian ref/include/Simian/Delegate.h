/************************************************************************/
/*!
\file		Delegate.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_DELEGATE_H_
#define SIMIAN_DELEGATE_H_

#include "SimianPlatform.h"

#include <vector>

namespace Simian
{
    struct SIMIAN_EXPORT DelegateParameter
    {
    public:
        template <class T>
        T& As()
        {
            return *reinterpret_cast<T*>(this);
        }

        template <class T>
        void As(T*& param)
        {
            param = reinterpret_cast<T*>(this);
        }
    };

    class SIMIAN_EXPORT Delegate
    {
    public:
        static DelegateParameter NoParameter;
    private:
        typedef void (*StaticCallback) (void*, DelegateParameter&);
    private:
        void* callingClass_;
        StaticCallback staticCallback_;
    public:
        void operator()(DelegateParameter& arg = NoParameter);

        template <class T>
        void operator()(T* callingClass, DelegateParameter& arg = NoParameter)
        {
            staticCallback_(callingClass, arg);
        }

        bool operator==(const Delegate& other)
        {
            return callingClass_ == other.callingClass_ && 
                   staticCallback_ == other.staticCallback_;
        }
    public:
        Delegate();
        ~Delegate();

        template <class T>
        Delegate(T*)
        {
        }

        void Call(DelegateParameter& arg = NoParameter);

        template <class T>
        void Call(T* callingClass, DelegateParameter& arg = NoParameter)
        {
            staticCallback_(callingClass, arg);
        }

        template <class T, void (T::*Method)(DelegateParameter&)>
        static Delegate CreateDelegate(T* object)
        {
            Delegate delegate;
            delegate.callingClass_ = reinterpret_cast<void*>(object);
            delegate.staticCallback_ = &Callback<T, Method>;
            return delegate;
        }

        template <class T, void (T::*Method)(DelegateParameter&)>
        static void Callback(void* object, DelegateParameter& data)
        {
            T* callingClass = reinterpret_cast<T*>(object);
            (callingClass->*Method)(data);
        }
    };

    class SIMIAN_EXPORT DelegateList
    {
    private:
        std::vector<Delegate> delegates_;
    public:
        void operator()(DelegateParameter& param = Delegate::NoParameter);
        void operator+=(const Delegate& delegate);
        void operator-=(const Delegate& delegate);
    public:
        void Call(DelegateParameter& param = Delegate::NoParameter);
        void AddDelegate(const Delegate& delegate);
        void RemoveDelegate(const Delegate& delegate);
        void Clear();
    };
}

#endif
