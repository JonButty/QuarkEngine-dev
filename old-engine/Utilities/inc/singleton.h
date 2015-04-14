#ifndef SINGLETON_H
#define SINGLETON_H

#include <cstdlib>

namespace Util
{
    template <class T>
    class Singleton
    {
    public:
        static inline T& Instance()
        {
            return *InstancePtr();
        }

        static inline T* InstancePtr()
        {
            if (!instance_)
            {
                instance_ = new T();
                atexit(Singleton<T>::Destroy);
            }
            return instance_;
        }
    private:
        static void Destroy()
        {
            delete instance_;
        }
    private:
        static T* instance_;
    };

    template <class T>
    T* Singleton<T>::instance_ = 0;
}

#endif