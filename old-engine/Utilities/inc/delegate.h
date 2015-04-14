#ifndef DELEGATE_H
#define DELEGATE_H

#include "configUtilitiesDLL.h"

namespace Util
{
    struct ENGINE_UTILITIES_EXPORT DelegateParameter
    {
    public:
        template <typename T>
        T& As()
        {
            return *reinterpret_cast<T*>(this);
        }

        template <typename T>
        void As(T*& param)
        {
            param = reinterpret_cast<T*>(this);
        }
    };

    class ENGINE_UTILITIES_EXPORT Delegate
    {
    public:
        Delegate();
        ~Delegate();

        template <typename T>
        Delegate(T*)
        {
        }

        void Call(DelegateParameter& arg = NoParameter);

        template <class T>
        void Call(T* callingClass,
                  DelegateParameter& arg = NoParameter)
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
        static void Callback(void* object,
                             DelegateParameter& data)
        {
            T* callingClass = reinterpret_cast<T*>(object);
            (callingClass->*Method)(data);
        }
    public:
        void operator()(DelegateParameter& arg = NoParameter);

        template <typename T>
        void operator()(T* callingClass,
                        DelegateParameter& arg = NoParameter)
        {
            staticCallback_(callingClass, arg);
        }

        bool operator==(const Delegate& other)
        {
            return callingClass_ == other.callingClass_ && 
                staticCallback_ == other.staticCallback_;
        }
    public:
        static DelegateParameter NoParameter;
    private:
        typedef void (*StaticCallback) (void*, DelegateParameter&);
    private:
        void* callingClass_;
        StaticCallback staticCallback_;
    };
}

#endif