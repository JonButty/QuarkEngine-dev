/*****************************************************************************/
/*!
\file		Singleton.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_SINGLETON_H_
#define SIMIAN_SINGLETON_H_

#include <cstdlib>

namespace Simian
{
    /** A generic singleton utility class implemented using templates. */
    template <class T>
    class Singleton
    {
    private:
        /** The static instance of the singleton. AKA the true object. */
        static T* instance_;

    private:
        /** Destroys the singleton instance.
            @remarks
                Manual invocation of this function is not needed/possible. It is
                automatically invoked when the program exits.
        */
        static void Destroy()
        {
            delete instance_;
        }

    public:
        /** Retrieves the instance to the singleton.
            @remarks
                If an instance does not exist, it is created then returned.
            @see
                Singleton::InstancePtr
        */
        static inline T& Instance()
        {
            return *InstancePtr();
        }

        /** Retrieves a pointer to the instance of the singleton.
            @see
                Singleton::Instance
        */
        static inline T* InstancePtr()
        {
            if (!instance_)
            {
                instance_ = new T();
                atexit(Singleton<T>::Destroy);
            }
            return instance_;
        }
    };

    template <class T>
    T* Singleton<T>::instance_ = 0;
}

#endif
