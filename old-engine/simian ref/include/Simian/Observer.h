/*****************************************************************************/
/*!
\file		Observer.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_OBSERVER_H_
#define SIMIAN_OBSERVER_H_

#include <list>

#if SIMIAN_COMPILER == SIMIAN_COMPILER_MSVC
#define SCallListeners(observer, type, func, ...) SCallListenersPtr((&(observer)), type, func, __VA_ARGS__)
#define SCallListenersPtr(observer, type, func, ...) \
for (Simian::Observer<type>::ListenerList::const_iterator i_ = observer->Listeners().begin(); i_ != observer->Listeners().end(); ++i_) \
{ \
    (*i_)->func(__VA_ARGS__); \
}
#else
#define SCallListeners(observer, type, func, params...) SCallListenersPtr(&(observer), func, params)
#define SCallListenersPtr(observer, type, func, params...) \
    for (Simian::Observer<type>::ListenerList::const_iterator i_ = observer->Listeners().begin(); i_ != observer->Listeners().end(); ++i_) \
{ \
    (*i_)->func(params); \
}
#endif

namespace Simian
{
    /** Observer style event dispatcher.
        @remarks
            A simple container that holds a list of listeners.
    */
    template <class T>
    class Observer
    {
    public:
        /** Listener list specific to an observer. */
        typedef std::list<T*> ListenerList;

    private:
        ListenerList listeners_;

    public:
        /** Gets the list of subscribed listeners. */
        inline const ListenerList& Listeners() const
        {
            return listeners_;
        }

    public:
        /** Subscribes a listener to the observer's events. */
        inline void AddListener(T* listener)
        {
            listeners_.push_back(listener);
        }

        /** Unsubscribes a listener to the observer's events. */
        inline void RemoveListener(T* listener)
        {
            listeners_.remove(listener);
        }

        /** Unsubscribes all listeners from the observer's events. */
        inline void RemoveAllListeners()
        {
            listeners_.clear();
        }
    };
}

#endif
