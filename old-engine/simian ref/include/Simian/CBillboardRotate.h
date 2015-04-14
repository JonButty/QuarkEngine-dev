/************************************************************************/
/*!
\file		CBillboardRotate.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CBILLBOARDROTATE_H_
#define SIMIAN_CBILLBOARDROTATE_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"

namespace Simian
{
    class CTransform;
    class Camera;

    class SIMIAN_EXPORT CBillboardRotate: public EntityComponent
    {
    private:
        CTransform* transform_;
        Camera* camera_;
        f32 spin_;

        static FactoryPlant<EntityComponent, CBillboardRotate> factoryPlant_;
    private:
        void update_(DelegateParameter& param);
    public:
        CBillboardRotate();

        void OnSharedLoad();
        void OnAwake();
        void OnInit();

        void Serialize( FileObjectSubNode& );
        void Deserialize( const FileObjectSubNode& );
    };
}

#endif
