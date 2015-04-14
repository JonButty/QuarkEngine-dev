/************************************************************************/
/*!
\file		COrthoCamera.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CORTHOCAMERA_H_
#define SIMIAN_CORTHOCAMERA_H_

#include "SimianPlatform.h"
#include "CCameraBase.h"
#include "Vector2.h"

namespace Simian
{
    class SIMIAN_EXPORT COrthoCamera: public CCameraBase
    {
    private:
        Vector2 size_;
        f32 near_;
        f32 far_;

        static FactoryPlant<EntityComponent, COrthoCamera> factoryPlant_;
    public:
        const Simian::Vector2& Size() const;
        void Size(const Simian::Vector2& val);

        Simian::f32 Near() const;
        void Near(Simian::f32 val);

        Simian::f32 Far() const;
        void Far(Simian::f32 val);
    private:
        void updatePosition_(DelegateParameter& param);
    public:
        COrthoCamera();

        void OnSharedLoad();
        void OnAwake();

        void Serialize(FileObjectSubNode& data);
        void Deserialize(const FileObjectSubNode& data);
    };
}

#endif
