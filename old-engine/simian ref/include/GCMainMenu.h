/*************************************************************************/
/*!
\file		GCMainMenu.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCMAINMENU_H_
#define DESCENSION_GCMAINMENU_H_

#include "GCInputListener.h"

namespace Simian
{
    class CTransform;
    class CSprite;
}

namespace Descension
{
    class GCUIButton;

    class GCMainMenu: public GCInputListener
    {
    private:
        struct UIElement
        {
            Simian::CTransform* Transform;
            Simian::CSprite* Sprite;

            Simian::Vector2 OriginalSize;
            Simian::Vector2 Position;
            Simian::f32 Height;
        };
    private:
        std::vector<UIElement> uiElements_;
        
        bool ignoreInput_;
        std::string lastAction_;
        bool fading_;
        Simian::f32 timer_;

        std::vector<GCUIButton*> buttons_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCMainMenu> factoryPlant_;
    private:
        void update_(Simian::DelegateParameter&);

        void disableButtons_();
        void getUIElement_(UIElement& element, const std::string& name);
        void registerTal_(const std::string& name);
        void registerTalChild_(const std::string& name);
        void closeOptions_(Simian::DelegateParameter&);

        void newGameYes_(Simian::DelegateParameter&);
        void quitYes_(Simian::DelegateParameter&);
        void quitNo_(Simian::DelegateParameter&);
    public:
        GCMainMenu();

        void OnSharedLoad();
        void OnAwake();

        void OnTriggered( Simian::DelegateParameter& );

        void Serialize( Simian::FileObjectSubNode& data );
        void Deserialize( const Simian::FileObjectSubNode& data );
    };
}

#endif
