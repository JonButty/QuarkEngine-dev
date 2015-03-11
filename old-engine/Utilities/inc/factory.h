#ifndef FACTORY_H
#define FACTORY_H

#include "utilAssert.h"

namespace Util
{
    // Used to create objects
    // Holds info of the base type of the object
    template <typename T>
    class FactoryPlantBase
    {
    public:

        // Pass in a factory to register this factory plant with
        template <typename V>
        FactoryPlantBase(V* factory = 0,
                         unsigned int type = 0)
        {
            if (factory)
                factory->RegisterFactoryPlant(type, this);
        }

        virtual ~FactoryPlantBase() {}
        virtual T* CreateInstance() = 0;
        virtual void DestroyInstance(T*& instance) = 0;
        virtual void Copy(T*& destination, T* source) const = 0;
    };

    // Default factory plant used to create objects
    // Holds info of the inherited type of the object
    template <typename T, typename U>
    class FactoryPlant
        :   public FactoryPlantBase<T>
    {
    public:
        template <typename V>
        FactoryPlant(V* factory = 0,
                     unsigned int type = 0)
                     :  FactoryPlantBase(factory, type)
        {   
        }
        
        T* CreateInstance()  
        {
            U* ret = new U();
            onCreateInstance(ret);
            return ret;
        }

        void DestroyInstance(T*& instance)
        {
            U* ret = dynamic_cast<U*>(instance);
            ENGINE_ASSERT_MSG(NULL != ret, "Type of input object is not an inherited type of factory plant output object.");
            
            onDestroyInstance(ret);
            delete instance;
            instance = 0;
        }

        // Overload inherited class' assignment operator for deep copy operation
        void Copy(T*& destination,
                  T* source) const
        {
            *reinterpret_cast<U*&>(destination) = *reinterpret_cast<U*>(source);
        }
    protected:

        // Used for setup operations for the inherited class
        virtual void onCreateInstance(U*& ){}

        // Used for teardown operations for the inherited class
        virtual void onDestroyInstance(U*& ){}
    };

    // Holds all Factory Plants that create objects of base class typename T
    template <typename T, unsigned int MaxTypes>
    class Factory
    {
    public:
        Factory()
        {
            for(unsigned int i = 0; i < MaxTypes; ++i)
                factoryPlants_[i] = 0;
        }

        // Adds a Factory Plant to the vector of Factory Plants
        void RegisterFactoryPlant(unsigned int type,
                                  FactoryPlantBase<T>* plant)
        {
            ENGINE_ASSERT_MSG(type < MaxTypes, "Invalid factory type");
            factoryPlants_[type] = plant;
        }

        void CopyInstance(unsigned int type,
                          T*& destination,
                          T* source)
        {
            factoryPlants_[type]->Copy(destination, source);
        }

        // Typename V must be an object which is inherited from typename T
        template <typename V>
        void CreateInstance(unsigned int type,
                            V*& instance)
        {
            V* ret = dynamic_cast<V*>(factoryPlants_[type]->CreateInstance());
            ENGINE_ASSERT_MSG(NULL != ret, "Type of input object is not an inherited type of factory plant output object.");
            
            instance = reinterpret_cast<V*>(ret);
            onCreateInstance(instance);
        }

        template <typename V>
        void DestroyInstance(unsigned int type,
                             V*& instance)
        {
            ENGINE_ASSERT_MSG(instance, "Invalid pointer passed.");
            onDestroyInstance(instance);
            factoryPlants_[type]->DestroyInstance(instance);
        }

    protected:
        // Used for setup operations for the base class
        virtual void onCreateInstance(T*) {}

        // Used for teardown operations for the base class
        virtual void onDestroyInstance(T*) {}

    private:
        FactoryPlantBase<T>* factoryPlants_[MaxTypes];
    };
}

#endif