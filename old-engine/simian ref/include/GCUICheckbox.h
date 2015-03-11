/*************************************************************************/
/*!
\file		GCUICheckbox.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCUICHECKBOX_H_
#define DESCENSION_GCUICHECKBOX_H_

#include "GCInputListener.h"

namespace Simian
{
    class CSprite;
    class CSoundEmitter;
}

namespace Descension
{
    class GCUICheckbox: public GCInputListener
    {
    private:
        bool checked_;
        Simian::CSprite* tick_;
        Simian::CSoundEmitter* sounds_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCUICheckbox> factoryPlant_;
    public:
        bool Checked() const;
        void Checked(bool val);
    public:
        GCUICheckbox();

        void OnAwake();

        void OnTriggered( Simian::DelegateParameter& );

        void Serialize( Simian::FileObjectSubNode& );
        void Deserialize( const Simian::FileObjectSubNode& );
    };
}

#endif
