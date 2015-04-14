/************************************************************************/
/*!
\file		ModelCache.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/ModelCache.h"
#include "Simian/Model.h"
#include "Simian/LogManager.h"

namespace Simian
{
    static const u32 INITIAL_QUEUE_SIZE = 64;

    ModelCache::ModelCache( GraphicsDevice* device )
        : device_(device)
    {
        fileLoadQueue_.reserve(INITIAL_QUEUE_SIZE);
        meshLoadQueue_.reserve(INITIAL_QUEUE_SIZE);
        loaded_.reserve(INITIAL_QUEUE_SIZE);
    }

    //--------------------------------------------------------------------------

    void ModelCache::ErrorCallback( const Simian::Delegate& val )
    {
        errorCallback_ = val;
    }

    void ModelCache::ProgressCallback( const Simian::Delegate& val )
    {
        progressCallback_ = val;
    }

    bool ModelCache::Invalidated() const
    {
        return fileLoadQueue_.size() || meshLoadQueue_.size();
    }

    //--------------------------------------------------------------------------

    Model* ModelCache::createInstance_( const DataLocation& location, const DataLocation& animationFile )
    {
        Model* model = new Model(device_);

        // create a model resource for file load queue
        fileLoadQueue_.push_back(ModelResource(model, location, animationFile));

        return model;
    }

    void ModelCache::unloadAll_()
    {
        // unload everything that has been loaded and delete the rest.
        for (u32 i = 0; i < loaded_.size(); ++i)
            loaded_[i].Model->Unload();
        loaded_.clear();
        fileLoadQueue_.clear();
        meshLoadQueue_.clear();
    }

    void ModelCache::destroyInstance_( Model* instance )
    {
        delete instance;
    }

    //--------------------------------------------------------------------------

    void ModelCache::LoadFiles()
    {
        // process the file load queue
        for (u32 i = 0; i < fileLoadQueue_.size(); ++i)
        {
            ModelResource& res = fileLoadQueue_[i];
            if (!res.ModelRes->LoadFile(res.Location, res.AnimationLocation))
            {
                // error
                ErrorCallbackParam param;
                param.ModelRes = &res;
                param.Phase = ErrorCallbackParam::LP_FILE;
                param.Handled = false;

                errorCallback_(param);

                if (!param.Handled)
                {
                    // unhandled error, print warning
                    SWarn(std::string("Unhandled model load file failure. Location: ") + res.Location.Identifier());
                    continue;
                }
            }
            
            // add to the mesh load queue
            meshLoadQueue_.push_back(res);
        }

        // clear out the queue
        fileLoadQueue_.clear();
    }

    void ModelCache::LoadMeshes()
    {
        // process the file load queue
        for (u32 i = 0; i < meshLoadQueue_.size(); ++i)
        {
            ModelResource& res = meshLoadQueue_[i];
            if (!res.ModelRes->LoadModel())
            {
                // error
                ErrorCallbackParam param;
                param.ModelRes = &res;
                param.Phase = ErrorCallbackParam::LP_MESH;
                param.Handled = false;

                errorCallback_(param);

                if (!param.Handled)
                {
                    // unhandled error, print warning
                    SWarn(std::string("Unhandled model load mesh failure. Location: ") + res.Location.Identifier());
                    continue;
                }
            }

            // progress callback!
            progressCallback_();

            // add to the mesh load queue
            LoadedModel model = { res.ModelRes, res.Location.Identifier(), res.AnimationLocation.Identifier() };
            loaded_.push_back(model);
        }

        // clear out the queue
        meshLoadQueue_.clear();
    }

    Model* ModelCache::Load( const DataLocation& location, const DataLocation& animationFile )
    {
        return DataCache<Model, DataLocation>::Load(location, animationFile);
    }

    void ModelCache::Reload()
    {
        for (u32 i = 0; i < loaded_.size(); ++i)
        {
            loaded_[i].Model->Unload();
            fileLoadQueue_.push_back(ModelResource(
                loaded_[i].Model,
                loaded_[i].Location,
                loaded_[i].AnimationLocation));
        }
        loaded_.clear();

        // process load queues
        LoadFiles();
        LoadMeshes();
    }
}
