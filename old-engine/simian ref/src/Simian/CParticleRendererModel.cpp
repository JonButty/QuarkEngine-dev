#include "Simian/CParticleRendererModel.h"
#include "Simian/EngineComponents.h"
#include "Simian/EnginePhases.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleRendererModel.h"
#include "Simian/ParticleSystem.h"
#include "Simian/MaterialCache.h"
#include "Simian/ModelCache.h"

namespace Simian
{
    ModelCache* CParticleRendererModel::modelCache_ = 0;
    MaterialCache* CParticleRendererModel::materialCache_ = 0;
    GraphicsDevice* CParticleRendererModel::graphicsDevice_ = 0;
    FactoryPlant<EntityComponent, CParticleRendererModel> CParticleRendererModel::factoryPlant_(
        &GameFactory::Instance().ComponentFactory(), C_PARTICLERENDERERMODEL);

    CParticleRendererModel::CParticleRendererModel()
        : renderer_(0),
          model_(0),
          material_(0),
          local_(false)
    {
    }

    //--------------------------------------------------------------------------

    Simian::ModelCache* CParticleRendererModel::ModelCache()
    {
        return modelCache_;
    }

    void CParticleRendererModel::ModelCache( Simian::ModelCache* val )
    {
        modelCache_ = val;
    }

    Simian::MaterialCache* CParticleRendererModel::MaterialCache()
    {
        return materialCache_;
    }

    void CParticleRendererModel::MaterialCache( Simian::MaterialCache* val )
    {
        materialCache_ = val;
    }

    Simian::GraphicsDevice* CParticleRendererModel::GraphicsDevice()
    {
        return graphicsDevice_;
    }

    void CParticleRendererModel::GraphicsDevice( Simian::GraphicsDevice* val )
    {
        graphicsDevice_ = val;
    }

    //--------------------------------------------------------------------------

    void CParticleRendererModel::update_( Simian::DelegateParameter& param )
    {
        EntityUpdateParameter* p;
        param.As(p);
        renderer_->Update(p->Dt);
    }

    //--------------------------------------------------------------------------

    void CParticleRendererModel::OnSharedLoad()
    {
        model_ = modelCache_->Load(modelFile_);
        material_ = materialCache_->Load(materialFile_);

        RegisterCallback(P_UPDATE, Delegate::CreateDelegate<CParticleRendererModel,
            &CParticleRendererModel::update_>(this));
    }

    void CParticleRendererModel::OnAwake()
    {
        CParticleSystem* system;
        ComponentByType(C_PARTICLESYSTEM, system);

        renderer_ = system->ParticleSystem()->AddRenderer<ParticleRendererModel>();
        renderer_->Device(graphicsDevice_);
        renderer_->Model(model_);
        renderer_->Material(material_);
        renderer_->Local(local_);
    }

    void CParticleRendererModel::Serialize( FileObjectSubNode& data )
    {
        data.AddData("ModelFile", modelFile_);
        data.AddData("MaterialFile", materialFile_);
        data.AddData("Local", local_);
    }

    void CParticleRendererModel::Deserialize( const FileObjectSubNode& data )
    {
        data.Data("ModelFile", modelFile_, modelFile_);
        data.Data("MaterialFile", materialFile_, materialFile_);
        data.Data("Local", local_, local_);

        if (renderer_)
            renderer_->Local(local_);
    }
}
