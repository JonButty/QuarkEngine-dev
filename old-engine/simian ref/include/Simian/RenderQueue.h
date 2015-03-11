/************************************************************************/
/*!
\file		RenderQueue.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_RENDERQUEUE_H_
#define SIMIAN_RENDERQUEUE_H_

#include "SimianPlatform.h"
#include "Delegate.h"
#include "Matrix.h"
#include "Vector3.h"
#include "Material.h"

#include <vector>

namespace Simian
{
    class RenderQueue;
    class GraphicsDevice;
    class Camera;

    struct SIMIAN_EXPORT RenderObject
    {
    public:
        typedef u64 SortKey;
    public:
        RenderQueue* ParentQueue;
        SortKey Key;
        u8 Layer;
        Simian::Material* Material;
        Simian::Matrix World;
        Delegate DrawCallback;
        void* UserData;
    public:
        bool operator<(const RenderObject& other) const;
    public:
        RenderObject(Simian::Material& material, u8 layer, const Matrix& world, const Delegate& drawCallback, void* userData);

        // compute sort key
        void ComputeSortKey(const Matrix& view);
    };

    struct SIMIAN_EXPORT RenderObjectDrawParameter: public DelegateParameter
    {
        RenderObject* Object;
        RenderObjectDrawParameter(RenderObject* object)
            : Object(object) {}
    };

    class SIMIAN_EXPORT RenderQueue
    {
    public:
        enum RenderLayer
        {
            RL_PRESCENE,
            RL_SCENE,
            RL_POSTSCENE,
            RL_PREUI,
            RL_UI,
            RL_POSTUI,
            RL_TOTAL
        };
    private:
        GraphicsDevice* device_;
        std::vector<std::vector<RenderObject> > queue_;
        std::vector<Camera*> cameras_;
        Material* activeMaterial_;
        Material::Pass* activePass_;
    public:
        GraphicsDevice& Device() const;
        const std::vector<Camera*>& Cameras() const;
        Material* ActiveMaterial() const;
        Material::Pass* ActivePass() const;
    public:
        RenderQueue(GraphicsDevice* device);

        void AddRenderObject(const RenderObject& renderObject);
        void AddCamera(Camera* camera);
        void RemoveCamera(Camera* camera);
        void ClearCameras();
        void Draw();
    };
}

#endif
