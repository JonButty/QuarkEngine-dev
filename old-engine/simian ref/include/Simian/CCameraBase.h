/************************************************************************/
/*!
\file		CCameraBase.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CCAMERABASE_H_
#define SIMIAN_CCAMERABASE_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"
#include "ExplicitType.h"
#include "Color.h"
#include "Vector3.h"
#include "Angle.h"

#include <vector>

namespace Simian
{
    class Camera;
    class RenderTexture;
    class GraphicsDevice;
    class CTransform;
    class RenderQueue;

    class SIMIAN_EXPORT CCameraBase: public EntityComponent
    {
    protected:
        CTransform* transform_;

        // actual camera object
        ExplicitType<Camera*> camera_;
        ExplicitType<RenderTexture*> renderTexture_;
        std::string renderTextureName_;

        Simian::Color clearColor_;
        Simian::Vector3 up_;

        u32 layers_;

        static CCameraBase* sceneCamera_;
        static GraphicsDevice* device_;
        static RenderQueue* renderQueue_;

        bool uiCamera_;
        bool composite_;
        std::vector<std::string> compositors_;
    public:
        const Simian::Color& ClearColor() const;
        void ClearColor(const Simian::Color& val);
        
        const Simian::Vector3& Up() const;
        void Up(const Simian::Vector3& val);

        static GraphicsDevice* Device();
        static void Device(GraphicsDevice* val);

        static Simian::RenderQueue* RenderQueue();
        static void RenderQueue(Simian::RenderQueue* val);

        Simian::Camera& Camera() const;

        template <class T>
        static void SceneCamera(T*& cam) 
        { 
            cam = reinterpret_cast<T*>(sceneCamera_);
        }
    public:
        CCameraBase();
        ~CCameraBase();

        void OnAwake();
        void OnDeinit();

        void SetAsSceneCamera();

        void Serialize(FileObjectSubNode& data);
        void Deserialize(const FileObjectSubNode& data);
    };
}

#endif
