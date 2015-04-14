/*************************************************************************/
/*!
\file		GCDestructiveUI.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCDESTRUCTIVEUI_H_
#define DESCENSION_GCDESTRUCTIVEUI_H_

#include "GCInputListener.h"
#include "Simian/Delegate.h"

namespace Simian
{
    class CTransform;
    class CBitmapText;
}

namespace Descension
{
    class GCDestructiveUI: public GCInputListener
    {
    private:
        Simian::CTransform* transform_;
        Simian::CBitmapText* bitmapText_;

        Simian::DelegateList yesCallbacks_;
        Simian::DelegateList noCallbacks_;
        std::string text_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCDestructiveUI> factoryPlant_;
    public:
        Simian::DelegateList& YesCallbacks();
        Simian::DelegateList& NoCallbacks();

        void Text(const std::string& text);
    private:
        void update_(Simian::DelegateParameter& param);
        void registerButton_(const std::string& name);
        void remove_();
    public:
        GCDestructiveUI();

        void OnSharedLoad();
        void OnAwake();

        void OnTriggered( Simian::DelegateParameter& );
    };
}

#endif
