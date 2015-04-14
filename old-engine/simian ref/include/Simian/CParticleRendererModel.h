#ifndef SIMIAN_CPARTICLERENDERERMODEL_H_
#define SIMIAN_CPARTICLERENDERERMODEL_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"

namespace Simian
{
    class ParticleRendererModel;
    class Material;
    class Model;

    class SIMIAN_EXPORT CParticleRendererModel: public EntityComponent
    {
    private:
        ParticleRendererModel* renderer_;
        Model* model_;
        std::string modelFile_;
        Material* material_;
        std::string materialFile_;
        bool local_;

        static ModelCache* modelCache_;
        static MaterialCache* materialCache_;
        static GraphicsDevice* graphicsDevice_;
        static FactoryPlant<EntityComponent, CParticleRendererModel> factoryPlant_;
    public:
        static Simian::ModelCache* ModelCache();
        static void ModelCache(Simian::ModelCache* val);

        static Simian::MaterialCache* MaterialCache();
        static void MaterialCache(Simian::MaterialCache* val);

        static Simian::GraphicsDevice* GraphicsDevice();
        static void GraphicsDevice(Simian::GraphicsDevice* val);
    private:
        void update_(Simian::DelegateParameter&);
    public:
        CParticleRendererModel();

        virtual void OnSharedLoad();
        virtual void OnAwake();

        virtual void Serialize( FileObjectSubNode& data );
        virtual void Deserialize( const FileObjectSubNode& data );
    };
}

#endif
