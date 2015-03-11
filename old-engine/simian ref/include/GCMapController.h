#ifndef DESCENSION_GCMAPCONTROLLER_H_
#define DESCENSION_GCMAPCONTROLLER_H_

#include "Simian/EntityComponent.h"


namespace Simian
{
    
}

namespace Descension
{
    class GCMapController : public Simian::EntityComponent
    {
    private:
        Simian::u32 *tiles_;
        Simian::u32 height_;
        Simian::u32 width_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCMapController> factoryPlant_;
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCMapController();

        void OnSharedLoad();
        void OnAwake();
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
