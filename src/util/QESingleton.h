/*!*****************************************************************************

\class 	QESingleton
\date 	2015/03/28
\author	Jonathan Butt
\brief	

*******************************************************************************/
#ifndef QESINGLETON_H
#define QESINGLETON_H

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