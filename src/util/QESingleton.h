/********************************************************************
	created:	2015/01/27
	filename: 	singleton.h
	author:		Jonathan Butt
	purpose:	
*********************************************************************/

#ifndef SINGLETON_H
#define SINGLETON_H

#include <cstdlib>

template <class T>
class QESingleton
{
public:

    static T& Instance()
    {
        return *InstancePtr();
    }

    static T* InstancePtr()
    {
        if(!instance_)
        {
            instance_ = new T();
            atexit(QESingleton<T>::destroy_);
        }
        return instance_;
    }

private:

    static void destroy_()
    {
        delete instance_;
    }

private:

    static T* instance_;

};

template <typename T>
T* QESingleton<T>::instance_ = 0;

#endif