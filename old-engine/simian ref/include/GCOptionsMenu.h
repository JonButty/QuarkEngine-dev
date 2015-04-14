/*************************************************************************/
/*!
\file		GCOptionsMenu.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCOPTIONSMENU_H_
#define DESCENSION_GCOPTIONSMENU_H_

#include "GCInputListener.h"

#include "Simian/Vector2.h"
#include <vector>

namespace Simian
{
    class CTransform;
    class CBitmapText;
}

namespace Descension
{
    class GCUICheckbox;
    class GCUISlider;

    class GCOptionsMenu: public GCInputListener
    {
    private:
        Simian::CTransform* transform_;
        Simian::CBitmapText* bitmapText_;
        GCUICheckbox* fullscreenCheckbox_;
        GCUISlider* musicSlider_;
        GCUISlider* soundSlider_;

        Simian::Delegate closeCallback_;
        Simian::u32 currentResolution_;
        std::vector<Simian::Vector2> resolutions_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCOptionsMenu> factoryPlant_;
    public:
        void CloseCallback(const Simian::Delegate& val);
    private:
        void update_(Simian::DelegateParameter&);
        void registerButton_(const std::string& name);
        void loadOptions_();
        void saveOptions_();
    public:
        GCOptionsMenu();

        void OnSharedLoad();
        void OnAwake();
        void OnInit();

        void OnTriggered( Simian::DelegateParameter& );

        void Serialize( Simian::FileObjectSubNode& );
        void Deserialize( const Simian::FileObjectSubNode& );
    };
}

#endif
