/************************************************************************/
/*!
\file		CCamera.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CCAMERA_H_
#define SIMIAN_CCAMERA_H_

#include "CCameraBase.h"

namespace Simian
{
    class SIMIAN_EXPORT CCamera: public CCameraBase
    {
    private:
        Simian::Radian fovy_;
        f32 near_;
        f32 far_;

        // factory plant
        static FactoryPlant<EntityComponent, CCamera> factoryPlant_;
    private:
        void updatePosition_(DelegateParameter& param);
    public:
        const Simian::Angle& Fovy() const;
        void Fovy(const Simian::Angle& val);

        Simian::f32 Near() const;
        void Near(Simian::f32 val);

        Simian::f32 Far() const;
        void Far(Simian::f32 val);
    public:
        CCamera();

        void OnSharedLoad();
        void OnAwake();

        void Serialize(FileObjectSubNode& data);
        void Deserialize(const FileObjectSubNode& data);
    };
}

#endif
