/************************************************************************/
/*!
\file		ModelCache.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_MODELCACHE_H_
#define SIMIAN_MODELCACHE_H_

#include "SimianPlatform.h"
#include "DataCache.h"
#include "Delegate.h"

#include <vector>
#include <string>

namespace Simian
{
    class Model;
    class GraphicsDevice;
    
    class SIMIAN_EXPORT ModelCache: public DataCache<Model, DataLocation>
    {
    public:
        struct ModelResource
        {
            Model* ModelRes;
            DataLocation Location;
            DataLocation AnimationLocation;

            ModelResource(Model* model, const DataLocation& data, const DataLocation& animation)
                : ModelRes(model), Location(data), AnimationLocation(animation) {}
        };

        struct LoadedModel
        {
            Simian::Model* Model;
            std::string Location;
            std::string AnimationLocation;
        };

        struct ErrorCallbackParam: public DelegateParameter
        {
            ModelResource* ModelRes;
            bool Handled;

            enum LoadPhase
            {
                LP_FILE,
                LP_MESH
            } Phase;
        };
    private:
        GraphicsDevice* device_;

        std::vector<ModelResource> fileLoadQueue_;
        std::vector<ModelResource> meshLoadQueue_;
        std::vector<LoadedModel> loaded_;

        Delegate errorCallback_;
        Delegate progressCallback_;
    public:
        void ErrorCallback(const Simian::Delegate& val);
        void ProgressCallback(const Simian::Delegate& val);

        bool Invalidated() const;
    private:
        Model* createInstance_( const DataLocation& location, const DataLocation& animationFile );
        void destroyInstance_( Model* instance );
        void unloadAll_();
    public:
        ModelCache(GraphicsDevice* device);

        void LoadFiles();
        void LoadMeshes();

        Model* Load(const DataLocation& location, const DataLocation& animationFile = "");

        void Reload();
    };
}

#endif
