/************************************************************************/
/*!
\file		CSoundListener.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/

#ifndef SIMIAN_CSOUNDLISTENER_H_
#define SIMIAN_CSOUNDLISTENER_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"

namespace Simian
{
    class CTransform;

    class SIMIAN_EXPORT CSoundListener: public EntityComponent
    {
    private:
        CTransform* transform_;

        f32 roll_;
        f32 doppler_;
        f32 distance_;

        static FactoryPlant<EntityComponent, CSoundListener> factoryPlant_;
    public:
        Simian::f32 Roll() const;
        void Roll(Simian::f32 val);

        Simian::f32 Doppler() const;
        void Doppler(Simian::f32 val);

        Simian::f32 Distance() const;
        void Distance(Simian::f32 val);
    private:
        void update_(DelegateParameter& param);
    public:
        CSoundListener();

        void OnSharedLoad();
        void OnAwake();

        void Serialize( FileObjectSubNode& );
        void Deserialize( const FileObjectSubNode& );
    };
}

#endif
