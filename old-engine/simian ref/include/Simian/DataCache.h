/************************************************************************/
/*!
\file		DataCache.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_DATACACHE_H_
#define SIMIAN_DATACACHE_H_

#include "SimianPlatform.h"
#include "DataLocation.h"

#include <string>
#include <map>

namespace Simian
{
    template <class T, class P = void*>
    class DataCache
    {
    protected:
        std::map<std::string, T*> cache_;
    protected:
        S_NIMPL virtual T* createInstance_(const DataLocation& location, const P& params) = 0;
        S_NIMPL virtual void unloadAll_() = 0;
        S_NIMPL virtual void destroyInstance_(T* instance) = 0;
    public:
        T* Load(const DataLocation& location, const P& params = 0)
        {
            std::map<std::string, T*>::iterator i = cache_.find(location.Identifier());
            if (i == cache_.end())
            {
                return cache_[location.Identifier()] = createInstance_(location.Identifier(), params);
            }
            return i->second;
        }

        void UnloadAll()
        {
            unloadAll_();
            for (std::map<std::string, T*>::iterator i = cache_.begin(); i != cache_.end(); ++i)
                destroyInstance_(i->second);
            cache_.clear();
        }
    };
}

#endif
