/*************************************************************************/
/*!
\file		GCEnemyDrop.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCENEMYDROP_H_
#define DESCENSION_GCENEMYDROP_H_

#include "Simian/EntityComponent.h"

namespace Simian
{
    class CTransform;
    class CModel;
}

namespace Descension
{
    static const Simian::f32 GRAVITATIONAL_ACCELERATION = 5.0f;

    class GCAttributes;

    class GCEnemyDrop: public Simian::EntityComponent
    {
    private:
        Simian::u32 healthDrop_;
        Simian::u32 randomHealthDrop_;

        Simian::u32 manaDrop_;
        Simian::u32 randomManaDrop_;

        Simian::u32 expDrop_;
        Simian::u32 randomExpDrop_;

        Simian::u32 totalDrops_;

        GCAttributes* attributes_;
        Simian::CTransform* transform_;
        Simian::CModel* model_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCEnemyDrop> factoryPlant_;
    private:
        void animateDrop_(Simian::Entity* drop);
        void onDeath_(Simian::DelegateParameter& param);
    public:
        GCEnemyDrop();

        void OnAwake();
        void OnDeinit();

        void Serialize( Simian::FileObjectSubNode& data );
        void Deserialize( const Simian::FileObjectSubNode& data );
    };
}

#endif
