/************************************************************************/
/*!
\file		TemplateFactory.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_TEMPLATEFACTORY_H_
#define SIMIAN_TEMPLATEFACTORY_H_

#include "SimianPlatform.h"
#include "Debug.h"
#include "Delegate.h"
#include <vector>

namespace Simian
{
    // In charge of creating instances
    template <class T>
    class FactoryPlantBase
    {
    public:
        virtual ~FactoryPlantBase() {}
        virtual T* CreateInstance() const = 0;
        virtual void Copy(T*& destination, T* source) const = 0;
    };
    
    template <class T, class U>
    class FactoryPlant: public FactoryPlantBase<T>
    {
    public:
        template <class V>
        FactoryPlant(V* factory = 0, u32 type = 0)
        {
            if (factory)
                factory->RegisterFactoryPlant(type, this);
        }

        T* CreateInstance() const 
        {
            return new U();
        }

        void Copy(T*& destination, T* source) const
        {
            *reinterpret_cast<U*&>(destination) = *reinterpret_cast<U*>(source);
        }
    };

    // Holds all Factory Plants
    template <class T>
    class Factory
    {
    public:
        template <class U>
        struct Plant
        {
            typedef Simian::FactoryPlant<T, U> Type;
        };
    private:
        bool userTypes_;
        std::vector<FactoryPlantBase<T>*> factoryPlants_;
    public:
        Factory(u32 size, bool userTypes = false)
            : userTypes_(userTypes),
              factoryPlants_(size, 0)
        {
        }

        // Adds a Factory Plant to the vector of Factory Plants
        void RegisterFactoryPlant(u32 type, FactoryPlantBase<T>* plant)
        {
            if (userTypes_)
            {
                if (type >= factoryPlants_.size())
                    factoryPlants_.resize(type + 1, 0);
            }
            else
            {
                SAssert(type < factoryPlants_.size(), "Invalid factory type.");
            }
            factoryPlants_[type] = plant;
        }

        void CopyInstance(u32 type, T*& destionation, T* source)
        {
            factoryPlants_[type]->Copy(destionation, source);
        }

        template <class V>
        void CreateInstance(u32 type, V*& instance)
        {
            instance = reinterpret_cast<V*>(factoryPlants_[type]->CreateInstance());
            OnCreateInstance(instance);
        }

        template <class V>
        void DestroyInstance(V*& instance)
        {
            OnDestroyInstance(instance);
            delete instance;
            instance = 0;
        }

        virtual void OnCreateInstance(T*) {}

        virtual void OnDestroyInstance(T*) {}
    };
}

#endif
