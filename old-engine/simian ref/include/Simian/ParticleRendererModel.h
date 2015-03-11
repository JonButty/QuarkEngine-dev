#ifndef SIMIAN_PARTICLERENDERERMODEL_H_
#define SIMIAN_PARTICLERENDERERMODEL_H_

#include "SimianPlatform.h"
#include "ParticleRenderer.h"
#include "Delegate.h"

namespace Simian
{
    class Model;
    class Material;
    class GraphicsDevice;

    class SIMIAN_EXPORT ParticleRendererModel: public ParticleRenderer
    {
    private:
        Model* model_;
        Material* material_;
        GraphicsDevice* device_;
        bool local_;
    public:
        Simian::Model* Model() const;
        void Model(Simian::Model* val);

        Simian::Material* Material() const;
        void Material(Simian::Material* val);

        GraphicsDevice* Device() const;
        void Device(GraphicsDevice* val);

        bool Local() const;
        void Local(bool val);
    private:
        void render_(Simian::DelegateParameter&);
    public:
        ParticleRendererModel(ParticleSystem* parent);

        void Load();
        void Unload();
        void Update( f32 );
        void Render();
        void Render( RenderQueue&, u8, const Simian::Matrix& );
    };
}

#endif
