#include "Simian/ParticleRendererModel.h"
#include "Simian/RenderQueue.h"
#include "Simian/ParticleSystem.h"
#include "Simian/Model.h"
#include "Simian/Angle.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/Texture.h"

namespace Simian
{
    ParticleRendererModel::ParticleRendererModel( ParticleSystem* parent )
        : ParticleRenderer(parent),
          model_(0),
          material_(0),
          device_(0),
          local_(false)
    {
    }

    //--------------------------------------------------------------------------

    Simian::Model* ParticleRendererModel::Model() const
    {
        return model_;
    }

    void ParticleRendererModel::Model( Simian::Model* val )
    {
        model_ = val;
    }

    Simian::Material* ParticleRendererModel::Material() const
    {
        return material_;
    }

    void ParticleRendererModel::Material( Simian::Material* val )
    {
        material_ = val;
    }

    GraphicsDevice* ParticleRendererModel::Device() const
    {
        return device_;
    }

    void ParticleRendererModel::Device( GraphicsDevice* val )
    {
        device_ = val;
    }

    bool ParticleRendererModel::Local() const
    {
        return local_;
    }

    void ParticleRendererModel::Local( bool val )
    {
        local_ = val;
    }

    //--------------------------------------------------------------------------

    void ParticleRendererModel::render_( Simian::DelegateParameter& )
    {
        Render();
    }

    //--------------------------------------------------------------------------

    void ParticleRendererModel::Load()
    {
        // doesn't actually need to load anything
    }

    void ParticleRendererModel::Unload()
    {
        // so it doesn't unload either
    }

    void ParticleRendererModel::Update( f32 )
    {
        // we don't really need to update anything
    }

    void ParticleRendererModel::Render()
    {
        // make sure it was initialised correctly
        if (!model_ || !material_)
            return;

        Simian::Matrix worldMatrix = device_->World();

        // render the model once from every particle
        for (unsigned i = 0; i < ParentSystem()->ActiveParticles().size(); ++i)
        {
            Particle* const p = ParentSystem()->ActiveParticles()[i];

            Vector3 position = p->Position;
            position -= local_ ? p->Origin : 0;
            
            // compute the world matrix (TRS)
            Simian::Matrix world = Simian::Matrix::Translation(position) *
                Simian::Matrix::RotationEuler(Simian::Radian(p->Rotation.X()), 
                Simian::Radian(p->Rotation.Y()), Simian::Radian(p->Rotation.Z())) *
                Simian::Matrix::Scale(p->Scale);

            // set the device's world matrix
            device_->World(local_ ? worldMatrix * world : world);

            // materials have to have at least one pass and a texture stage
            // for color interpolation
            if (material_ && material_->Size() == 1 && 
                (*material_)[0].TextureStages().size() == 1)
            {
                Texture* texture = (*material_)[0].TextureStages()[0].Texture();
                texture->TextureState(Simian::Texture::TSF_CONSTANT, 
                                      p->Color.ColorU32());
            }

            // draw the model
            model_->Draw(NULL);
        }
    }

    void ParticleRendererModel::Render( RenderQueue& renderQueue, u8 layer, 
                                        const Simian::Matrix& world )
    {
        renderQueue.AddRenderObject(RenderObject(*material_, layer, world,
            Simian::Delegate::CreateDelegate<ParticleRendererModel, &ParticleRendererModel::render_>(this),
            NULL));
    }
}
