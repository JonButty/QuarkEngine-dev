/************************************************************************/
/*!
\file		RenderQueue.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/RenderQueue.h"
#include "Simian/Material.h"
#include "Simian/Camera.h"
#include "Simian/Utility.h"
#include "Simian/GraphicsDevice.h"

#include <algorithm>
#include "Simian/Shader.h"

namespace Simian
{
    static const u32 RENDERQUEUE_INITIAL_SIZE = 128;

    RenderQueue::RenderQueue(GraphicsDevice* device)
        : device_(device)
    {
        queue_.resize(RL_TOTAL);
        for (u32 i = 0; i < RL_TOTAL; ++i)
            queue_[i].reserve(RENDERQUEUE_INITIAL_SIZE);
    }

    //--------------------------------------------------------------------------

    GraphicsDevice& RenderQueue::Device() const
    {
        return *device_;
    }

    const std::vector<Camera*>& RenderQueue::Cameras() const
    {
        return cameras_;
    }

    Material* RenderQueue::ActiveMaterial() const
    {
        return activeMaterial_;
    }

    Material::Pass* RenderQueue::ActivePass() const
    {
        return activePass_;
    }

    //--------------------------------------------------------------------------

    void RenderQueue::AddRenderObject( const RenderObject& renderObject )
    {
        queue_[renderObject.Layer].push_back(renderObject);
        queue_[renderObject.Layer].back().ParentQueue = this;
    }

    static bool cameraSort_(Camera* const a, Camera* const b)
    {
        return a->Layers() < b->Layers();
    }

    void RenderQueue::AddCamera( Camera* camera )
    {
        cameras_.push_back(camera);

        // sort cameras by layer
        std::sort(cameras_.begin(), cameras_.end(), cameraSort_);
    }

    void RenderQueue::RemoveCamera( Camera* camera )
    {
        // remove camera from rendertextures
        if (camera->RenderTextureName() != "")
            Material::RenderTextures()[camera->RenderTextureName()] = 0;

        std::vector<Camera*>::iterator i = std::find(cameras_.begin(), cameras_.end(), camera);
        if (i != cameras_.end())
            cameras_.erase(i);
    }

    void RenderQueue::ClearCameras()
    {
        cameras_.clear();
    }

    void RenderQueue::Draw()
    {
        for (u32 i = 0; i < cameras_.size(); ++i)
        {
            Camera& camera = *cameras_[i];

            device_->Projection(camera.Projection());
            device_->View(camera.View());

            camera.Set();

            for (u32 j = 0; j < RL_TOTAL; ++j)
            {
                u32 layers = camera.Layers();
                std::vector<RenderObject>& queue = queue_[j];
                if (!queue.size() || !SIsFlagSet(layers, j))
                    continue;
                
                activeMaterial_ = queue[0].Material;
                activePass_ = &(*activeMaterial_)[0];

                activeMaterial_->Set();
                activePass_->Set();

                for (u32 k = 0; k < queue.size(); ++k)
                    queue[k].ComputeSortKey(camera.View());

                // sort based on sort key
                std::sort(queue.begin(), queue.end());

                // loop through the sorted queue
                for (u32 k = 0; k < queue.size(); ++k)
                {
                    if (activeMaterial_ != queue[k].Material)
                    {
                        activeMaterial_->Unset();
                        activeMaterial_ = queue[k].Material;
                        activeMaterial_->Set();
                    }
                    for (u32 l = 0; l < activeMaterial_->Size(); ++l)
                    {
                        if (activePass_ != &(*queue[k].Material)[l])
                        {
                            activePass_->Unset();
                            activePass_ = &(*queue[k].Material)[l];
                            activePass_->Set();
                        }
                        device_->World(queue[k].World);
                        RenderObjectDrawParameter param(&queue[k]);
                        if (activePass_->Shader())
                            activePass_->Shader()->Update();
                        queue[k].DrawCallback(param);
                    }
                }

                if (activePass_)
                    activePass_->Unset();
                if (activeMaterial_)
                    activeMaterial_->Unset();
            }

            camera.Unset();
        }

        // clear out the render queue
        for (u32 i = 0; i < RL_TOTAL; ++i)
            queue_[i].clear();
    }
}
