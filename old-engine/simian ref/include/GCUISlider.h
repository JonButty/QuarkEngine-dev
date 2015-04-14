/*************************************************************************/
/*!
\file		GCUISlider.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCUISLIDER_H_
#define DESCENSION_GCUISLIDER_H_

#include "GCInputListener.h"

namespace Simian
{
    class CTransform;
    class CSoundEmitter;
}

namespace Descension
{
    class GCUISlider: public GCInputListener
    {
    private:
        Simian::CTransform* transform_;
        Simian::CTransform* control_;
        Simian::CSoundEmitter* sounds_;

        Simian::f32 value_;
        Simian::f32 limit_;
        
        bool moving_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCUISlider> factoryPlant_;
    public:
        Simian::f32 Value() const;
        void Value(Simian::f32 val);

        Simian::f32 Limit() const;
        void Limit(Simian::f32 val);
    private:
        void update_(Simian::DelegateParameter&);
    public:
        GCUISlider();

        void OnSharedLoad();
        void OnAwake();

        void OnTriggered( Simian::DelegateParameter& );

        void Serialize( Simian::FileObjectSubNode& );
        void Deserialize( const Simian::FileObjectSubNode& );
    };
}

#endif
